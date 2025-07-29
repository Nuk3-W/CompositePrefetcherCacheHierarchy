#pragma once

#include "Core/Types.h"
#include "Utils/BitUtils.h"

namespace Config::Constants {
    constexpr int g_reservedLruBits = 8;
    constexpr int g_validBits       = 1;
    constexpr int g_dirtyBits       = 1;
    constexpr Address g_lruMask   = Utils::makeMask(0, g_reservedLruBits);
    constexpr Address g_validMask = Utils::makeMask(g_reservedLruBits, g_validBits);
    constexpr Address g_dirtyMask = Utils::makeMask(g_reservedLruBits + g_validBits, g_dirtyBits);
}



