#pragma once

#include <cstdint>

namespace Config {

    // Control unit configuration for prefetching
    struct ControlUnitParams {
        uint32_t kTrackerSize_{};
        uint32_t superBlockBits_{};
    };

} 