#pragma once

#include "Core/Types.h"
#include "Core/CacheBlock.h"

namespace Utils {
    template<typename T>
    inline bool isType(const AccessResult& variant) {
        return std::holds_alternative<T>(variant);
    }

    inline CacheBlock& getBlock(AccessResult& variant) {
        return std::visit([](auto& value) -> CacheBlock& {
            return value.block.get();
        }, variant);
    }

    inline Address getAddress(const AccessResult& variant) {
        return std::visit([](const auto& value) -> Address {
            return value.block.get().getAddress();
        }, variant);
    }
}
