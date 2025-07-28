#pragma once

#include "Core/Types.h"
#include <cstdint>
#include <iostream>
#include <string>

namespace Utils {
    inline constexpr Address makeMask(int start, int length) {
        if (length == 0) return 0;
        if (start + length >= sizeof(Address) * 8) return ~0UL << start;
        return ( ( 1UL << length ) - 1 ) << start;
    }

    inline void printMask(const std::string& label, Address mask) {
        std::cout << label << ": " << std::hex << mask << std::dec << std::endl;
    }
}