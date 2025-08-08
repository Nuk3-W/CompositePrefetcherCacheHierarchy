#pragma once

#include <vector>
#include "Interface/IBlockCollection.h"
#include "Core/CacheBlock.h"
#include "Core/Types.h"
#include "Config/CacheParams.h"

class CacheContainer : public IBlockCollection<CacheBlock> {
public:
    using iterator = typename std::vector<CacheBlock>::iterator;

    CacheContainer(const Config::CacheParams& params);
    ~CacheContainer() override = default;

    std::optional<std::reference_wrapper<CacheBlock>> findBlock(Address address) override;

    std::size_t size() const override { return blocks_.size(); }

    iterator CacheContainer::setBegin(Address address) {
        return blocks_.begin() + (computeSet(address) * params_.assoc_);
    }
    
    iterator CacheContainer::setEnd(Address address) {
        return blocks_.begin() + ((computeSet(address) + 1) * params_.assoc_);
    }

private:    
    Address computeSet(Address address) const { return (address & bitMasks_.setBits_) >> blockBits_; }

    int blockBits_{};
    Config::CacheParams params_{};
    BitMasks bitMasks_{};
    std::vector<CacheBlock> blocks_{};
};
