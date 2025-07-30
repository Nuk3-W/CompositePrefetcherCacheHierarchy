#pragma once

#include "Config/CacheParams.h"
#include "Core/CacheContainerManager.h"
#include "Core/CacheBlock.h"
#include "Core/Types.h"
#include <vector>

class CacheHierarchyManager {
public:
    LevelManager(const CacheParams& params);
    ~LevelManager() = default;

    AccessResult access(Address addr, AccessType type);
    void handleCacheEviction(CacheBlock& block, Address addr);

private:
    std::vector<CacheContainerManager> caches_;
    std::vector<CacheContainerManager> vCaches_;
}