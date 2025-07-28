#pragma once

#include <variant>

namespace Utils {
    template <typename T, typename Variant>
    inline bool isType(const Variant& v) {
        return std::holds_alternative<T>(v);
    }

    template <typename T, typename Variant>
    inline Address getAddress(const Variant& v) {
        return std::get<T>(v).addr;
    }
}
