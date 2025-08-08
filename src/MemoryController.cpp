#include "Core/MemoryController.h"

MemoryController::MemoryController(const Config::SystemParams& params) {
    for (std::size_t i = 0; i < params.caches_.size(); ++i) {
        Config::CacheParams victimParams = (i < params.vCaches_.size()) ? params.vCaches_[i] : Config::CacheParams{};
        caches_.emplace_back(params.caches_[i], victimParams, i);
    }
}

void MemoryController::read(Address addr) {
    access(addr, [](LevelCache& cache, Address address) { return cache.read(address); }, AccessType::Read);
}

void MemoryController::write(Address addr) {
    access(addr, [](LevelCache& cache, Address address) { return cache.write(address); }, AccessType::Write);
}

void MemoryController::access(Address addr, std::function<AccessResult(LevelCache&, Address)> accessFunc, AccessType accessType) {
    AccessResult result = accessFunc(caches_[l1CacheIndex_], addr);
    
    if (Utils::isType<Hit>(result)) return;
    
    handleCacheMiss(addr, result, accessType);
}

void MemoryController::handleCacheMiss(Address addr, AccessResult cacheResult, AccessType accessType) {
    if (Utils::isType<Evict>(cacheResult)) {
        evictionHandler_.processEviction(cacheResult, l1CacheIndex_, caches_);
    }
    
    pullFromLowerLevels(addr);
    auto& block = Utils::getBlock(cacheResult);
    insertBlock(block, addr, accessType);
}

void MemoryController::pullFromLowerLevels(Address addr) {
    for (std::size_t level = l2CacheStart_; level < caches_.size(); ++level) {
        AccessResult cacheResult = caches_[level].read(addr);
        
        if (Utils::isType<Hit>(cacheResult)) return;
        
        
        if (Utils::isType<Evict>(cacheResult)) {
            evictionHandler_.processEviction(cacheResult, level, caches_);
        }

        auto& block = Utils::getBlock(cacheResult);
        insertBlock(block, addr, AccessType::Read);
    }
}

void MemoryController::insertBlock(CacheBlock& block, Address addr, AccessType accessType) {
    block.initialize(addr, accessType);
}


