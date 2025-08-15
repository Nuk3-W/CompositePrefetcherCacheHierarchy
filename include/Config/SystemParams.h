#pragma once

#include <string>
#include <vector>
#include "CacheParams.h"
#include "PrefetcherParams.h"

namespace Config {

    // System-wide configuration for the memory controller
    struct SystemParams {
        std::string traceFile_{};
        uint32_t blockSize_{};
        std::vector<CacheParams> caches_{};
        std::vector<CacheParams> vCaches_{};
        ControlUnitParams controlParams_{};
    };

} 