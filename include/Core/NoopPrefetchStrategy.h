#pragma once

#include "Interface/IPrefetchStrategy.h"

class NoopPrefetchStrategy : public IPrefetchStrategy {
public:
    std::optional<Address> prefetch(Address addr) override {
        return std::nullopt;
    }
};
