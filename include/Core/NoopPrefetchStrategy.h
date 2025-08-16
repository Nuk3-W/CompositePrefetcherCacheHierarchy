#pragma once

#include "Interface/IPrefetchStrategy.h"

class NoopPrefetchStrategy : public IPrefetchStrategy {
public:
    std::optional<AccessResult> prefetch(Address addr) override {
        return std::nullopt;
    }
};
