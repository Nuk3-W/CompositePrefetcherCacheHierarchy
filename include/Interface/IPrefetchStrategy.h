#pragma once

#include "Core/Types.h"
#include <optional>

class IPrefetchStrategy {
public:
    virtual std::optional<Address> prefetch(Address addr) = 0;
    virtual ~IPrefetchStrategy() = default;
};