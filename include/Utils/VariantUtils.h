#pragma once

namespace Utils {
    template<typename T>
    inline bool isType(const std::variant<T>& variant) const {
        return std::holds_alternative<T>(variant);
    }

    template<typename T>
    inline T& get(std::variant<T>& variant) {
        return std::get<T>(variant);
    }
}
