#pragma once

#include "Config/CacheParams.h"
#include "Core/CacheLRUReplacement.h"
#include "Core/CacheContainer.h"
#include "Core/CacheBlock.h"
#include "Core/Types.h"
#include <optional>
#include <functional>

class CacheContainerManager {
public:
    explicit CacheContainerManager(const CacheParams& params);
    ~CacheContainerManager() = default;

    std::optional<std::reference_wrapper<CacheBlock>> findAddr(Address addr);
    std::reference_wrapper<CacheBlock> evict(Address addr);

private:
    CacheContainer cache_;
    CacheLRUReplacement replacementPolicy_;
};