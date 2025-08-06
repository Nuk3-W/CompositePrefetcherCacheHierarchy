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
    
    auto& stats = levelStats_[level];
    
    if (accessType == AccessType::Read) {
        stats.reads_++;
    } else {
        stats.writes_++;
    }
    
    if (result == CacheResult::Hit) {
        stats.hits_++;
    } else {
        stats.misses_++;
        if (accessType == AccessType::Read) {
            stats.readMisses_++;
        } else {
            stats.writeMisses_++;
        }
    }
    
    if (result == CacheResult::Evict) {
        stats.evictions_++;
    }
    
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

void StatisticsManager::recordHit(std::size_t level) {
    ensureLevelExists(level);
    levelStats_[level].hits_++;
    totalAccesses_++;
}

void StatisticsManager::recordMiss(std::size_t level) {
    ensureLevelExists(level);
    levelStats_[level].misses_++;
    totalAccesses_++;
}

void StatisticsManager::recordEviction(std::size_t level) {
    ensureLevelExists(level);
    levelStats_[level].evictions_++;
}

void StatisticsManager::recordWriteBack(std::size_t level) {
    recordWriteback(level);
}

void StatisticsManager::printStats() const {
    std::cout << "\n=== Cache Statistics ===\n";
    std::cout << "Total Accesses: " << totalAccesses_ << "\n\n";

    for (std::size_t i = 0; i < levelStats_.size(); ++i) {
        const auto& stats = levelStats_[i];
        uint64_t total = stats.hits_ + stats.misses_;
        
        if (total == 0) continue;

        std::cout << "Level " << (i + 1) << " Cache:\n";
        std::cout << "  Hits: " << stats.hits_ << " (" 
                  << std::fixed << std::setprecision(2) 
                  << (static_cast<double>(stats.hits_) / total * 100.0) << "%)\n";
        std::cout << "  Misses: " << stats.misses_ << " (" 
                  << std::fixed << std::setprecision(2) 
                  << (static_cast<double>(stats.misses_) / total * 100.0) << "%)\n";
        std::cout << "  Evictions: " << stats.evictions_ << "\n";
        std::cout << "  Writebacks: " << stats.writebacks_ << "\n";
        std::cout << "  Hit Rate: " << std::fixed << std::setprecision(2) 
                  << (static_cast<double>(stats.hits_) / total * 100.0) << "%\n\n";
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
} 