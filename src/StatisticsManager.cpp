#include "Core/StatisticsManager.h"
#include <iostream>
#include <iomanip>

StatisticsManager& StatisticsManager::getInstance() {
    static StatisticsManager instance;
    return instance;
}

void StatisticsManager::ensureLevelExists(std::size_t level) {
    if (level >= levelStats_.size()) {
        levelStats_.resize(level + 1);
    }
}

void StatisticsManager::recordAccess(std::size_t level, AccessType accessType, CacheResult result) {
    ensureLevelExists(level);
    auto& s = levelStats_[level];
    const bool isRead = (accessType == AccessType::Read);
    const bool isHit = (result == CacheResult::Hit);
    const bool isEvct = (result == CacheResult::Evict);
    s.reads_ += isRead;
    s.writes_ += !isRead;
    s.hits_ += isHit;
    s.misses_ += !isHit;
    s.readMisses_ += (!isHit && isRead);
    s.writeMisses_ += (!isHit && !isRead);
    s.evictions_ += isEvct;
    totalAccesses_++;
}

void StatisticsManager::recordSwapRequest(std::size_t level) {
    ensureLevelExists(level);
    levelStats_[level].swapRequests_++;
}

void StatisticsManager::recordSwap(std::size_t level) {
    ensureLevelExists(level);
    levelStats_[level].swaps_++;
}

void StatisticsManager::recordWriteback(std::size_t level) {
    ensureLevelExists(level);
    levelStats_[level].writebacks_++;
}

void StatisticsManager::recordSuperBlockAccess(std::size_t level, CacheResult result) {
    ensureLevelExists(level);
    superBlockHits_ += (result == CacheResult::Hit);
    superBlockMisses_ += (result == CacheResult::Miss);
}

void StatisticsManager::recordPrefetchHit(uint32_t strategyId) {
    switch (strategyId) {
    case 0: prefetchStats_.noopHits_++; break;
    case 1: prefetchStats_.sequentialHits_++; break;
    case 2: prefetchStats_.markovHits_++; break;
    default: break;
    }
}

void StatisticsManager::printDetailedStats() const {
    std::cout << "\n=== Detailed Cache Statistics ===\n";
    
    for (std::size_t i = 0; i < levelStats_.size(); ++i) {
        const auto& stats = levelStats_[i];
        uint64_t totalAccesses = stats.reads_ + stats.writes_;
        
        if (totalAccesses == 0) continue;
        
        std::cout << "Level " << (i + 1) << " Cache Statistics:\n";
        std::cout << "  Reads: " << stats.reads_ << "\n";
        std::cout << "  Read Misses: " << stats.readMisses_ << "\n";
        std::cout << "  Writes: " << stats.writes_ << "\n";
        std::cout << "  Write Misses: " << stats.writeMisses_ << "\n";
        std::cout << "  Swap Requests: " << stats.swapRequests_ << "\n";
        
        if (totalAccesses > 0) {
            double swapRequestRate = static_cast<double>(stats.swapRequests_) / totalAccesses;
            std::cout << "  Swap Request Rate (SRR): " << std::fixed << std::setprecision(4) 
                      << swapRequestRate << "\n";
        }
        
        std::cout << "  Swaps: " << stats.swaps_ << "\n";
        
        if (totalAccesses > 0) {
            uint64_t totalMisses = stats.readMisses_ + stats.writeMisses_;
            double combinedMissRate = static_cast<double>(totalMisses - stats.swaps_) / totalAccesses;
            std::cout << "  Combined Miss Rate: " << std::fixed << std::setprecision(4) 
                      << combinedMissRate << "\n";
        }
        
        std::cout << "  Writebacks: " << stats.writebacks_ << "\n";
        
        if (stats.reads_ > 0) {
            double missRate = static_cast<double>(stats.readMisses_) / stats.reads_;
            std::cout << "  Miss Rate: " << std::fixed << std::setprecision(4) 
                      << missRate << "\n";
        }
        
        std::cout << "\n";
    }

    std::cout << "  Super Block Hits: " << superBlockHits_ << "\n";
    std::cout << "  Super Block Misses: " << superBlockMisses_ << "\n";
    std::cout << "  Prefetch Hits: Seq=" << prefetchStats_.sequentialHits_
              << ", Markov=" << prefetchStats_.markovHits_
              << ", Noop=" << prefetchStats_.noopHits_ << "\n";
} 