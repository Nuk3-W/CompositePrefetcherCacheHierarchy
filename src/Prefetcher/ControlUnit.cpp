#include "Prefetcher/ControlUnit.h"


ControlUnit::ControlUnit(const ControlUnitParams& params, const unsigned long blockSize) : 
    superBlockTracker_(params.kTrackerSize_),
    currentPrefetcher_(PrefetchType::Sequential),
    prefetcher_(blockSize) {
    
    const uint32_t blockOffsetBits = static_cast<uint32_t>(std::log2(blockSize));
    const uint32_t superBlockBits = 32 - blockOffsetBits - params.superBlockBits_;
    
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

void ControlUnit::updateThresholdOnMiss(int markovTotal, int markovHit, int sequentialTotal, int sequentialHit) {

    // [TODO] add ghb data access to this function to offload it from the prefetcher helps seperate concerns
    if (markovTotal == 0 || sequentialTotal == 0) return;

    double markovPerc = static_cast<double>(markovHit) / markovTotal;
    double sequentialPerc = static_cast<double>(sequentialHit) / sequentialTotal;
    double threshHoldMultiplier = markovPerc / sequentialPerc;
    
    // I will be changing the logic here to make it more accurate

    /*
    current issues are how the life of the program is currently always showing in thrshhold 
    values so possiblt shortening the memory of the control unit would help
    or just using some basic dampeners to stop the prominence of old data
    */
    threshold_ = threshold_ * threshHoldMultiplier;
    // we need to keep bounds to avoid over fitting to old data if it was prominent
    if (threshold_ > 511) threshold_ = 511;
    if (threshold_ < 1) threshold_ = 1;
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




