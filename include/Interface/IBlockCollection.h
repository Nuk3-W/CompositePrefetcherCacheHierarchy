#pragma once

#include <vector>
#include <cstddef>
#include <optional>
#include <functional>
#include "Core/Types.h"

// Generic block collection interface: lookup only.
// Clients that need iteration should access the container directly.

template<typename Block>
class IBlockCollection {
public:
    using iterator = typename std::vector<Block>::iterator;
    using const_iterator = typename std::vector<Block>::const_iterator;

    virtual ~IBlockCollection() = default;

    virtual std::optional<std::reference_wrapper<Block>> findBlock(Address address) = 0;
    virtual std::optional<std::reference_wrapper<const Block>> findBlock(Address address) const = 0;

    virtual std::size_t size() const = 0;
}; 