#pragma once

#include "Core/Types.h"

class IBlock {
public:
    virtual ~IBlock() = default;

    virtual bool isValid() const = 0;
    virtual bool isDirty() const = 0;
    virtual void setValid() = 0;
    virtual void clearValid() = 0;
    virtual void setDirty() = 0;
    virtual void clearDirty() = 0;

    virtual void setAddress(Address addr) = 0;
    virtual Address getAddress() const = 0;

    virtual uint32_t getLRU() const = 0;
    virtual void incrementLRU() = 0;
    virtual void setLRU(uint8_t lru) = 0;
};


