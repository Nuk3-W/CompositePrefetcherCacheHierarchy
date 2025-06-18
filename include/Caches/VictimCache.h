#ifndef VICTIM_CACHE_H
#define VICTIM_CACHE_H

#include "LevelCache.h"
#include "CacheData.h"

class VictimCache : public LevelCache {
public:
    VictimCache(const CacheParams& params);

    Address read(Address addr) override;
    Address write(Address addr) override;
	Address writeBack(Address addr) override;

    bool isValid() const;
private:
    bool valid{};
};

#endif // VICTIM_CACHE_H