#ifndef PREFETCHER_H
#define PREFETCHER_H

#include "Caches/CacheData.h"
#include "GHB.h"
#include "SystemCacheData.h"
#include <cstdint>
#include <iostream>

enum class PrefetchType {
    Sequential,
    Markov
};

class Prefetcher {
public:
    Prefetcher(const unsigned long blockSize);
    Prefetcher() = default;

    Address prefetch(Address addr, PrefetchType type);
    Address getPrefetchedAddress() const { return currentPrefetchCandidate_; }
    Address getBlockMask() const { return blockMask_; }

    Address seqPrefetch(Address addr);
    Address markovPrefetch(Address addr);
    void updateGHB(Address addr); 

private:
    Address currentPrefetchCandidate_ { ~g_cacheHitAddress };
    uint32_t blockMask_{};
    GHB ghb_{512, 0};
};

#endif // PREFETCHER_H