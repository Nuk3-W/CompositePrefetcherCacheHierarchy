#pragma once

#include "Config/CacheParams.h"
#include "Core/CacheLRUReplacement.h"
#include "Core/CacheContainer.h"
#include "Core/CacheBlock.h"
#include "Core/Types.h"


class CacheContainerManager {
public:
    CacheContainerManager(const Config::CacheParams& params);
    ~CacheContainerManager() = default;

    AccessResult read(Address addr);
    AccessResult write(Address addr);
    
private:
    AccessResult evict(Address addr);

private:
    CacheContainer cache_;
    CacheLRUReplacement replacementPolicy_;
};