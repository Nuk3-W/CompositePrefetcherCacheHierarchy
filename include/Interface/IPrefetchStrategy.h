#pragma once

#include "Core/Types.h"
#include <optional>

class IPrefetchStrategy {
public:
    virtual std::optional<AccessResult> prefetch(Address addr) = 0;
    virtual ~IPrefetchStrategy() = default;
};