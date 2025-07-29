#pragma once

#include <cstdint>

#include "Utils/BitUtils.h"
#include "Config/Constants.h"

using Address = uint32_t;

enum class AccessResult {
    Hit,
    Miss
};

enum class AccessType {
    Read,
    Write
};  

struct CacheBlockData {
    Address tag_{};
    uint32_t metaData_{};
};

struct BitMasks {
    uint32_t tagBits_{};
    uint32_t setBits_{};
};

