#pragma once

#include "Interface/IPrefetchStrategy.h"
#include "Utils/BitUtils.h"
#include <cmath>

class SequentialPrefetchStrategy : public IPrefetchStrategy {
public:
    explicit SequentialPrefetchStrategy(uint32_t blockSize) {
        int blockBits = static_cast<int>(std::log2(blockSize));
        constexpr int addressBits = sizeof(Address) * 8;
        blockMask_ = Utils::makeMask(blockBits, addressBits - blockBits);
    }

    SequentialPrefetchStrategy() = default;

    std::optional<Address> prefetch(Address addr) override {
        Address blockAddr = addr & blockMask_;
        Address nextBlockAddr = blockAddr + ~blockMask_ + 1;
        return nextBlockAddr;
    }

private:
    Address blockMask_{};
};


