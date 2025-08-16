#pragma once

#include "Interface/IPrefetchStrategy.h"
#include "Core/GHB.h"
#include "Utils/BitUtils.h"

class MarkovPrefetchStrategy : public IPrefetchStrategy {
public:
    explicit MarkovPrefetchStrategy(uint32_t blockSize)
        : ghb_(512, blockSize) {
        int blockBits = static_cast<int>(std::log2(blockSize));
        constexpr int addressBits = sizeof(Address) * 8;
        blockMask_ = Utils::makeMask(blockBits, addressBits - blockBits);
    }

    std::optional<Address> prefetch(Address addr) override {
        Address predictedAddr = ghb_.markovPredictor(addr);
        if (predictedAddr == std::numeric_limits<Address>::max()) return std::nullopt;
        return predictedAddr & blockMask_;
    }

    void onAccess(Address addr) override {
        ghb_.insert(addr);
    }

private:
    GHB ghb_;
    Address blockMask_{};
};


