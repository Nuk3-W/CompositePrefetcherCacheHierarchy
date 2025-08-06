#pragma once

#include <string>
#include <vector>
#include <optional>
#include "CacheParams.h"

namespace Config {

    // Level cache configuration - combines main cache and optional victim cache
    struct LevelCacheParams {
        CacheParams mainCache_{};
        std::optional<CacheParams> victimCache_{};
        
        LevelCacheParams(const CacheParams& main) : mainCache_(main) {}
        LevelCacheParams(const CacheParams& main, const CacheParams& victim) 
            : mainCache_(main), victimCache_(victim) {}
    };

    // Cache hierarchy configuration parameters
    struct CacheHierarchyParams {
        uint32_t blockSize_{};
        std::vector<LevelCacheParams> levels_{};
    };
} 