#ifndef CacheManager_H
#define CacheManager_H

#include <memory>
#include <array>

#include "Caches/LevelCache.h"
#include "Caches/CacheData.h"
#include "Caches/VictimCache.h"
//#include "Prefetcher/GHB.h"
//#include "Prefetcher/ControlUnit.h"
#include "SystemCacheData.h"

class CacheManager {
public:
    CacheManager(const SystemCacheParams& systemCacheParams);

    void read(Address addr);
	void write(Address addr);

    void printStats() const;
private:
    bool isCacheHit(Address result) const;

    void handleLevelWriteBack(Address writeBack, std::size_t level);
    void handleVictimWriteBack(Address writeBack, std::size_t level);

    std::vector<LevelCache> caches_;
    std::vector<VictimCache> vCaches_; //this is configureable to allow for more than just L1 victim cache 
};

#endif