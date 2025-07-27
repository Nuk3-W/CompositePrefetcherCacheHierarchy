#include "Prefetcher/Prefetcher.h"

Prefetcher::Prefetcher(const unsigned long blockSize) {
    const uint32_t blockSizeMask = static_cast<uint32_t>(log2(blockSize));
    blockMask_ = ~0UL << blockSizeMask;
    //std::cout << "Block mask: " << std::hex << blockMask_ << std::dec << std::endl;
    
    // Initialize GHB with the same block size
    ghb_ = GHB(512, blockSize);
}

// need to return address so cache hierarchy pulls from lower layers
Address Prefetcher::prefetch(Address addr, PrefetchType type) {
    return type == PrefetchType::Sequential ? seqPrefetch(addr) : markovPrefetch(addr);
}

Address Prefetcher::seqPrefetch(Address addr) {
    Address blockAddr = addr & blockMask_;
    //std::cout << "Sequential prefetching block address: " << std::hex << blockAddr << std::dec << std::endl;
    Address nextBlockAddr = blockAddr + ~blockMask_ + 1;
    //std::cout << "Next block address: " << std::hex << nextBlockAddr << std::dec << std::endl;
    currentPrefetchCandidate_ = nextBlockAddr;
    return currentPrefetchCandidate_;
}

Address Prefetcher::markovPrefetch(Address addr) {
    Address predictedAddr = ghb_.markovPredictor(addr);
    if (predictedAddr != ~0UL) {
        // Convert to block address like sequential prefetcher
        currentPrefetchCandidate_ = predictedAddr & blockMask_;
    } else {
        currentPrefetchCandidate_ = ~0UL;
    }
    return currentPrefetchCandidate_;
}

void Prefetcher::updateGHB(Address addr) {
    ghb_.insert(addr);
}





