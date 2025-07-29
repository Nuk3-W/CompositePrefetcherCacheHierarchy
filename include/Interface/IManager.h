#pragma once

#include "Core/AccessBus.h"
#include "Core/Types.h"

class IManager {
public:
    virtual ~IManager() = default;
    virtual AccessResult access(AccessBus bus) = 0;
}