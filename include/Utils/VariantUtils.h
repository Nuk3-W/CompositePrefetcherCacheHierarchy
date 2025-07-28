#pragma once

template <typename T, typename Variant>
inline bool isType(const Variant& v) {
    return std::holds_alternative<T>(v);
}