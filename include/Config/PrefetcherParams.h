#pragma once

#include <cstdint>
#include "CacheParams.h"

namespace Config {

    // Control unit configuration for prefetching
    struct ControlUnitParams {
        CacheParams trackerParams_{};
    };

} 