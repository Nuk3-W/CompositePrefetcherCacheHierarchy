#include "Core/MemoryController.h"

MemoryController::MemoryController(const Config::SystemParams& params) :
    prefetchController_(params.controlParams_.trackerParams_.size_ > 0 ? std::make_optional(PrefetchController(params.controlParams_)) : std::nullopt) {
    caches_.reserve(params.caches_.size());
    for (std::size_t i = 0; i < params.caches_.size(); ++i) {
        Config::CacheParams victimParams = (i < params.vCaches_.size()) ? params.vCaches_[i] : Config::CacheParams{};
        caches_.emplace_back(params.caches_[i], victimParams, i);
    }
}

void MemoryController::read(Address addr) {
    if (prefetchController_) {
        prefetchController_->updateTrackerOnAccess(addr);
    }
    AccessResult result = caches_[rootLevelIndex_].read(addr);
    if (Utils::isType<Hit>(result)) return;
    handleCacheMiss(addr, result, AccessType::Read);
}

void MemoryController::write(Address addr) {
    if (prefetchController_) {
        prefetchController_->updateTrackerOnAccess(addr);
    }
    AccessResult result = caches_[rootLevelIndex_].write(addr);
    if (Utils::isType<Hit>(result)) return;
    handleCacheMiss(addr, result, AccessType::Write);
}

void MemoryController::handleCacheMiss(Address addr, AccessResult cacheResult, AccessType accessType) {


    if (Utils::isType<Evict>(cacheResult)) {
        evictionHandler_.processEviction(cacheResult, rootLevelIndex_, caches_);
    }
    probeLowerLevelsAndInstall(addr);
    
    Utils::getBlock(cacheResult).initialize(addr, accessType); 

    if (prefetchController_) {
        prefetchController_->updateOnMiss(addr);
        prefetchController_->prefetch(addr);
    }
}

void MemoryController::probeLowerLevelsAndInstall(Address addr) {
    for (std::size_t level = firstLowerLevelIndex_; level < caches_.size(); ++level) {
        AccessResult cacheResult = caches_[level].read(addr);
        
        if (Utils::isType<Hit>(cacheResult)) return;
        if (Utils::isType<Evict>(cacheResult)) {
            evictionHandler_.processEviction(cacheResult, level, caches_);
        }

        Utils::getBlock(cacheResult).initialize(addr, AccessType::Read);
    }
}

