#pragma once

#include "Types.h"
#include "GHB.h"
#include <cstdint>
#include <iostream>

enum class PrefetchType {
    Sequential,
    Markov
};

class Prefetcher {
public:
    Prefetcher(const uint32_t blockSize);
    Prefetcher() = default;

    AccessResult prefetch(Address addr, PrefetchType type);
    Address getPrefetchedAddress() const { return currentPrefetchCandidate_; }
    Address getBlockMask() const { return blockMask_; }

    AccessResult seqPrefetch(Address addr);
    AccessResult markovPrefetch(Address addr);
    void updateGHB(Address addr); 

private:
    Address currentPrefetchCandidate_ {};
    uint32_t blockMask_{};
    GHB ghb_{512, 0};
};