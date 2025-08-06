#pragma once

#include <list>
#include <unordered_map>
#include <vector>
#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <cmath>
#include <optional>
#include <iostream>

#include "Types.h"

class GHB {
public:
    struct GHBEntry {
        Address address;
        GHBEntry(Address addr);
    };
    using ListIt = std::list<GHBEntry>::iterator;

    GHB(size_t bufferSize, uint32_t blockSize);
    void insert(Address addr);
    Address markovPredictor(Address missAddress) const;

private:
    std::vector<ListIt> findDuplicates(Address addr) const;
    std::size_t bufferSize_;
    std::list<GHBEntry> ghbList_;
    std::unordered_multimap<Address, ListIt> indexTable_;
    void addToIndexTable(Address addr, ListIt it);
    void removeFromIndexTable(Address addr, ListIt it);
    Address blockMask_{};
};