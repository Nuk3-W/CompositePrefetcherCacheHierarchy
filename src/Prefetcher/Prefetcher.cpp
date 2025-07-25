#include "Prefetcher/Prefetcher.h"

Prefetcher::Prefetcher(const unsigned long blockSize) {}

Address Prefetcher::prefetch(Address addr, PrefetchType type) {
    if (type == PrefetchType::Sequential) {
        return seqPrefetch(addr, type);
    }
    return markovPrefetch(addr, type);
}

Address Prefetcher::seqPrefetch(Address addr, PrefetchType type) {
    currentPrefetchCandidate_ = addr + 1;
    return currentPrefetchCandidate_;
}

Address Prefetcher::markovPrefetch(Address addr, PrefetchType type) {
    // Markov prefetching logic goes here
    return g_cacheHitAddress;
}





