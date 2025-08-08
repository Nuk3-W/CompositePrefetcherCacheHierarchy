#pragma once

#include "Core/Types.h"
#include "Utils/BitUtils.h"

class CacheBlock {
private:
    struct CacheBlockData {
        Address address_{};
        uint32_t metaData_{};
    };

    CacheBlockData data_;

    static constexpr int s_reservedLruBits = 8;
    static constexpr int s_validBits       = 1;
    static constexpr int s_dirtyBits       = 1;
    static constexpr Address s_lruMask     = Utils::makeMask(0, s_reservedLruBits);
    static constexpr Address s_validMask   = Utils::makeMask(s_reservedLruBits, s_validBits);
    static constexpr Address s_dirtyMask   = Utils::makeMask(s_reservedLruBits + s_validBits, s_dirtyBits);
    
public:
    CacheBlock(CacheBlockData& data) : data_(data) {}
    CacheBlock() = default;

    void clear() {
        data_.metaData_ = 0;
        data_.address_ = 0;
    }   

    bool isValid() const { return data_.metaData_ & s_validMask; }
    bool isDirty() const { return data_.metaData_ & s_dirtyMask; }

    void setValid() { data_.metaData_ |= s_validMask; }
    void clearValid() { data_.metaData_ &= ~s_validMask; }
    void setDirty() { data_.metaData_ |= s_dirtyMask; }
    void clearDirty() { data_.metaData_ &= ~s_dirtyMask; }

    void setAddress(Address addr) { data_.address_ = addr; }
    Address getAddress() const { return data_.address_; }

    void setMetaData(uint32_t meta) { data_.metaData_ = meta; }
    uint32_t getMetaData() const { return data_.metaData_; }
    
    uint32_t getLRU() const { return data_.metaData_ & s_lruMask; }
    void setLRU(uint8_t lru) { data_.metaData_ = (data_.metaData_ & ~s_lruMask) | (static_cast<uint32_t>(lru) & s_lruMask); }
    void incrementLRU() { setLRU(static_cast<uint8_t>((getLRU() + 1) & 0xFF)); }

    void copy(const CacheBlock& other) {
        data_.address_ = other.data_.address_;
        uint32_t sourceState = other.data_.metaData_ & (s_validMask | s_dirtyMask);
        uint32_t preservedLRU = data_.metaData_ & s_lruMask;
        data_.metaData_ = sourceState | preservedLRU;
    }

    void initialize(Address addr, AccessType accessType) {
        data_.address_ = addr;
        data_.metaData_ |= s_validMask;
        if (accessType == AccessType::Write) {
            data_.metaData_ |= s_dirtyMask;
        } else {
            data_.metaData_ &= ~s_dirtyMask;
        }
    }
};
