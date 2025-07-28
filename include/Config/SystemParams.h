#pragma once

#include <string>
#include <vector>

#include "CacheParams.h"
#include "PrefetcherParams.h"

namespace Config {

    // System-wide configuration that aggregates all components
    struct SystemCacheParams {
        uint32_t blockSize_{};
        std::string traceFile_{};
    
        std::vector<CacheParams> caches_{};
        std::vector<CacheParams> vCaches_{};
    
        ControlUnitParams controlUnit_{};
    };

} 