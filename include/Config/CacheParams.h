#pragma once

#include <cstdint>

namespace Config {

    // Cache configuration parameters
    struct CacheParams {
        uint32_t blockSize_{};
        uint32_t size_{};
        uint32_t sets_{};
        uint32_t assoc_{};
    };
    
    // Cache performance statistics
    struct CacheStats {
        uint32_t hits_{};
        uint32_t misses_{};
        uint32_t writeMiss_{};
        uint32_t readMiss_{};
        uint32_t writes_{};
        uint32_t reads_{};
        uint32_t writeBacks_{};
        uint32_t swapRequests_{};
        uint32_t swapHits_{};
    };

} 