#pragma once

#include <cstdint>
#include "CacheParams.h"

namespace Config {

    // Control unit configuration for prefetching
    struct ControlUnitParams {
        CacheParams trackerParams_{};
        uint32_t prefetchBlockSize_{};
        double alpha_{0.5};
        double enableThreshold_{0.65};
        double disableThreshold_{0.35};
    };

} 