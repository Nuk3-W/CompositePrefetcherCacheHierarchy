#pragma once

#include "Interface/IBlockLookup.h"
#include <vector>
#include <cstddef>

// Generic full-featured block collection interface: lookup, mutation, and observation.
// Clients that only need lookup or editing should depend on the narrower interfaces.

template<typename Block>
class IBlockCollection : public IBlockLookup<Block> {
public:
    using iterator = typename std::vector<Block>::iterator;
    using const_iterator = typename std::vector<Block>::const_iterator;

    virtual ~IBlockCollection() = default;

    virtual std::optional<std::reference_wrapper<Block>> findBlock(Address address) = 0;
    virtual std::optional<std::reference_wrapper<const Block>> findBlock(Address address) const = 0;

    virtual iterator begin() = 0;
    virtual const_iterator begin() const = 0;
    virtual iterator end() = 0;
    virtual const_iterator end() const = 0;
    virtual std::size_t size() const = 0;
}; 