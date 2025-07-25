#ifndef SYSTEM_CACHE_DATA_H
#define SYSTEM_CACHE_DATA_H

#include <vector>
#include <string>
#include <optional>
#include <cstdint>

#include "Caches/CacheData.h"

struct ControlUnitParams {
    uint32_t kTrackerSize_{};
    uint32_t superBlockBits_{};
};

struct SystemCacheParams {
    unsigned long blockSize_;
    std::string traceFile_; 

    std::vector<CacheParams> caches_;
    std::vector<CacheParams> vCaches_;

    ControlUnitParams controlUnit_;
};

#endif // SYSTEM_CACHE_DATA_H