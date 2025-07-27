#include "Prefetcher/ControlUnit.h"


ControlUnit::ControlUnit(const ControlUnitParams& params, const unsigned long blockSize) : 
    superBlockTracker_(params.kTrackerSize_),
    currentPrefetcher_(PrefetchType::Sequential),
    prefetcher_(blockSize) {
    
    const uint32_t blockOffsetBits = static_cast<uint32_t>(std::log2(blockSize));
    const uint32_t superBlockBits = blockOffsetBits + params.superBlockBits_;

    superBlockMask_ = (~0UL) << superBlockBits;
    
    // Initialize LRU counters (0 = most recent, kTrackerSize-1 = oldest)
    for (uint32_t i = 0; i < superBlockTracker_.size(); ++i) {
        superBlockTracker_[i].lruCounter_ = i;
    }
}

void ControlUnit::updateTrackerOnAccess(Address currentAddr) {
    Address superBlockAddr = extractSuperBlock(currentAddr);
    
    const auto foundIndex = findSuperBlockEntry(superBlockAddr);
    
    if (foundIndex) {
        // Found existing entry - increment counter and update LRU
        auto& entry = superBlockTracker_[*foundIndex];
        ++entry.hitCounter_;
        updateLRU(*foundIndex);
        
        currentPrefetcher_ = (entry.hitCounter_ > threshold_) ? 
            PrefetchType::Sequential : PrefetchType::Markov;
    } else {
        // Not found - evict LRU entry and insert new one
        int victimIndex = getVictimLRU();
        auto& victim = superBlockTracker_[victimIndex];
        
        victim.superBlockAddr_ = superBlockAddr;
        victim.hitCounter_ = 0; // Start with 0 hits
        updateLRU(victimIndex);
    }
}

void ControlUnit::updateThresholdOnMiss(Address missAddr) {
    prefetcher_.updateGHB(missAddr);
    updateOnMiss();  // Always track the miss
    
    // Use recent performance data for threshold calculation (prevents stale data)
    double markovHitRate = calculateRecentHitRate(PrefetchType::Markov);
    double sequentialHitRate = calculateRecentHitRate(PrefetchType::Sequential);
    
    if (sequentialHitRate == 0.0) return;
    double threshHoldMultiplier = markovHitRate / sequentialHitRate;
    
    threshold_ = threshold_ * threshHoldMultiplier;

    //std::cout << "Threshold: " << threshold_ << std::endl;
    
    // Keep bounds to avoid extreme values
    if (threshold_ > 511) threshold_ = 511;
    if (threshold_ < 1) threshold_ = 1;
}

void ControlUnit::updateOnHit() {
    if (currentPrefetcher_ == PrefetchType::Sequential) {
        ++prefetchStats_.sequentialTotal;
        ++prefetchStats_.sequentialHits;
    } else {
        ++prefetchStats_.markovTotal;
        ++prefetchStats_.markovHits;
    }
    addToQueue(true, currentPrefetcher_);
}

void ControlUnit::updateOnMiss() {
    if (currentPrefetcher_ == PrefetchType::Sequential) {
        ++prefetchStats_.sequentialTotal;
    } else {
        ++prefetchStats_.markovTotal;
    }
    addToQueue(false, currentPrefetcher_);
}

void ControlUnit::resetStats() {
    prefetchStats_ = PrefetchStats{};
    recentPerformance_.clear();
}

void ControlUnit::addToQueue(bool isHit, PrefetchType type) {
    recentPerformance_.push_back({isHit, type});
    if (recentPerformance_.size() > QUEUE_SIZE) {
        recentPerformance_.pop_front();
    }
}

double ControlUnit::calculateRecentHitRate(PrefetchType type) const {
    if (recentPerformance_.empty()) return 0.0;
    
    int hits = 0;
    int total = 0;
    
    for (const auto& entry : recentPerformance_) {
        if (entry.second == type) {
            ++total;
            if (entry.first) { // isHit
                ++hits;
            }
        }
    }
    
    return total > 0 ? static_cast<double>(hits) / total : 0.0;
}

void ControlUnit::printQueueStats() const {
    std::cout << "Recent Performance Queue (" << recentPerformance_.size() << "/" << QUEUE_SIZE << " entries):\n";
    
    double markovRate = calculateRecentHitRate(PrefetchType::Markov);
    double sequentialRate = calculateRecentHitRate(PrefetchType::Sequential);
    
    std::cout << "  Markov Hit Rate: " << (markovRate * 100) << "%\n";
    std::cout << "  Sequential Hit Rate: " << (sequentialRate * 100) << "%\n";
    std::cout << "  Current Threshold: " << threshold_ << "\n";
}


Address ControlUnit::extractSuperBlock(Address addr) const {
    return addr & superBlockMask_;
}

std::optional<int> ControlUnit::findSuperBlockEntry(Address superBlockAddr) const {
    for (uint32_t i = 0; i < superBlockTracker_.size(); ++i) {
        if (superBlockTracker_[i].superBlockAddr_ == superBlockAddr) {
            return static_cast<int>(i);
        }
    }
    return std::nullopt;
}

int ControlUnit::getVictimLRU() const {
    uint32_t maxLru = 0;
    int victimIndex = 0;
    
    for (uint32_t i = 0; i < superBlockTracker_.size(); ++i) {
        if (superBlockTracker_[i].lruCounter_ > maxLru) {
            maxLru = superBlockTracker_[i].lruCounter_;
            victimIndex = static_cast<int>(i);
        }
    }
    return victimIndex;
}

void ControlUnit::updateLRU(int accessedIndex) {
    uint32_t accessedLru = superBlockTracker_[accessedIndex].lruCounter_;
    
    // Increment LRU for all entries that were more recent than accessed entry
    for (uint32_t i = 0; i < superBlockTracker_.size(); ++i) {
        if (superBlockTracker_[i].lruCounter_ < accessedLru) {
            ++superBlockTracker_[i].lruCounter_;
        }
    }
    
    // Make accessed entry most recent (LRU = 0)
    superBlockTracker_[accessedIndex].lruCounter_ = 0;
}

Address ControlUnit::prefetch(Address addr) {
    return prefetcher_.prefetch(addr, currentPrefetcher_);
}

Address ControlUnit::readPrefetchedAddress() const {
    return prefetcher_.getPrefetchedAddress();
}

void ControlUnit::printStats() const {
    std::cout << "=== Global Statistics ===\n";
    std::cout << "Markov Stats:\n";
    std::cout << "  Total: " << prefetchStats_.markovTotal << "\n";
    std::cout << "  Hits: " << prefetchStats_.markovHits << "\n";
    if (prefetchStats_.markovTotal > 0) {
        double markovGlobalRate = static_cast<double>(prefetchStats_.markovHits) / prefetchStats_.markovTotal;
        std::cout << "  Global Hit Rate: " << (markovGlobalRate * 100) << "%\n";
    }
    
    std::cout << "Sequential Stats:\n";
    std::cout << "  Total: " << prefetchStats_.sequentialTotal << "\n";
    std::cout << "  Hits: " << prefetchStats_.sequentialHits << "\n";
    if (prefetchStats_.sequentialTotal > 0) {
        double sequentialGlobalRate = static_cast<double>(prefetchStats_.sequentialHits) / prefetchStats_.sequentialTotal;
        std::cout << "  Global Hit Rate: " << (sequentialGlobalRate * 100) << "%\n";
    }
    
    std::cout << "\n=== Recent Performance (Last " << QUEUE_SIZE << " entries) ===\n";
    printQueueStats();
}



