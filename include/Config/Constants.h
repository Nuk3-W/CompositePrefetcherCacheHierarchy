#pragma once

#include "Core/Types.h"
#include "Utils/BitUtils.h"

namespace Config::Constants {
    inline constexpr int g_reservedLruBits = 8;
    inline constexpr int g_validBits       = 1;
    inline constexpr int g_dirtyBits       = 1;
    inline constexpr Address g_lruMask   = Utils::makeMask(0, g_reservedLruBits);
    inline constexpr Address g_validMask = Utils::makeMask(g_reservedLruBits, g_validBits);
    inline constexpr Address g_dirtyMask = Utils::makeMask(g_reservedLruBits + g_validBits, g_dirtyBits);
}



