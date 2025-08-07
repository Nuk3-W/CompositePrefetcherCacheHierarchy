#pragma once

#include <vector>
#include <cstddef>
#include <optional>
#include <functional>
#include "Core/Types.h"

template<typename Block>
class IBlockCollection {
public:
    using iterator = typename std::vector<Block>::iterator;

    virtual ~IBlockCollection() = default;

    virtual std::optional<std::reference_wrapper<Block>> findBlock(Address address) = 0;

    virtual std::size_t size() const = 0;
}; 