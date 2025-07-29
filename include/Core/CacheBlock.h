#pragma once

#include "Core/Types.h"
#include "Utils/BitUtils.h"

class CacheBlock {
    public:
        CacheBlock(CacheBlockData& data) : data_(data) {}
        ~CacheBlock() = default;
    
        bool isValid() const { return data_.metaData_ & Constants::g_validMask; }
        bool isDirty() const { return data_.metaData_ & Constants::g_dirtyMask; }
    
        void setValid() {
            data_.metaData_ |= Constants::g_validMask;
        }
    
        void clearValid() {
            data_.metaData_ &= ~Constants::g_validMask;
        }
    
        void setDirty() {
            data_.metaData_ |= Constants::g_dirtyMask;
        }
    
        void clearDirty() {
            data_.metaData_ &= ~Constants::g_dirtyMask;
        }
    
        void setTag(Address tag) { data_.tag_ = tag; }
        Address getTag() const { return data_.tag_; }
    
        void setMetaData(uint32_t meta) { data_.metaData_ = meta; }
        uint32_t getMetaData() const { return data_.metaData_; }
        
        uint8_t getLRU() const { return (data_.metaData_ & Constants::g_lruMask) >> 2; }
        void setLRU(uint8_t lru) {
            data_.metaData_ = (data_.metaData_ & ~Constants::g_lruMask) | (static_cast<uint32_t>(lru) << 2);
        }
        void incrementLRU() {
            data_.metaData_ += 1;
        }
        
    private:
        CacheBlockData& data_{};
        static constexpr int s_reservedLruBits = 8;
        static constexpr int s_validBits       = 1;
        static constexpr int s_dirtyBits       = 1;
        static constexpr Address s_lruMask   = Utils::makeMask(0, s_reservedLruBits);
        static constexpr Address s_validMask = Utils::makeMask(s_reservedLruBits, s_validBits);
        static constexpr Address s_dirtyMask = Utils::makeMask(s_reservedLruBits + s_validBits, s_dirtyBits);
    };