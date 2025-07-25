#ifndef PREFETCHER_H
#define PREFETCHER_H

#include "Caches/CacheData.h"

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

    Address seqPrefetch(Address addr, PrefetchType type);
    Address markovPrefetch(Address addr, PrefetchType type);

private:
    Address currentPrefetchCandidate_ { ~g_cacheHitAddress };
};

#endif // PREFETCHER_H