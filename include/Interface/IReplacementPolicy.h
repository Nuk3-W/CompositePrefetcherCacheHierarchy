#pragma once

#include <cstdint>

template<typename IndexType>
class IReplacementPolicy {
public:
    virtual ~IReplacementPolicy() = default;

    virtual IndexType getVictim() = 0;
}; 