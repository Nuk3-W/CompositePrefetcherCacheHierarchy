#ifndef VICTIM_CACHE_H
#define VICTIM_CACHE_H

#include "LevelCache.h"
#include "CacheData.h"

class VictimCache : public LevelCache {
public:
    VictimCache(const CacheParams& params);

private:
    bool valid{};
};

#endif // VICTIM_CACHE_H