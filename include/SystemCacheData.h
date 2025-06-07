#ifndef SYSTEM_CACHE_DATA_H
#define SYSTEM_CACHE_DATA_H

#include <vector>

#include "Caches/CacheData.h"
//#include "Prefetcher/PrefetcherData.h"

struct SystemCacheParams {
    std::vector<CacheParams> caches_;
    std::vector<CacheParams> vCaches_;
};

#endif // SYSTEM_CACHE_DATA_H