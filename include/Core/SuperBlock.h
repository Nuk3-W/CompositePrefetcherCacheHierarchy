#pragma once

#include "Core/CacheBlock.h"
#include "Utils/BitUtils.h"

class SuperBlock : public CacheBlock {
public:
    SuperBlock() = default;

    uint32_t getHits() const { return (getMetaData() & s_hitsMask) >> s_hitsOffset; }

    void incrementHits() {
        auto meta = getMetaData();
        uint32_t value = (meta & s_hitsMask) >> s_hitsOffset;
        if (value < s_hitsMask) {
            ++value;
        }
        meta = (meta & ~s_hitsMask) | ((value << s_hitsOffset) & s_hitsMask);
        setMetaData(meta);
    }

    void resetHits() {
        auto meta = getMetaData();
        meta &= ~s_hitsMask;
        setMetaData(meta);
    }

private:
    static constexpr int s_hitsOffset = s_reservedLruBits + s_validBits + s_dirtyBits;
    static constexpr int s_hitsBits = 32 - s_hitsOffset;
    static constexpr uint32_t s_hitsMask = Utils::makeMask(s_hitsOffset, s_hitsBits);
};