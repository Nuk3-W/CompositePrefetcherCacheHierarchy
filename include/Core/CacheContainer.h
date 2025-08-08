#pragma once

#include <vector>
#include <cmath>
#include "Interface/IBlockCollection.h"
#include "Core/Types.h"
#include "Config/CacheParams.h"
#include "Utils/BitUtils.h"

template<typename Block>    
class CacheContainer : public IBlockCollection<Block> {
public:
    using iterator = typename std::vector<Block>::iterator;

    CacheContainer(const Config::CacheParams& params);
    ~CacheContainer() override = default;

    std::optional<std::reference_wrapper<Block>> findBlock(Address address) override;

    std::size_t size() const override { return blocks_.size(); }

    iterator setBegin(Address address) {
        return blocks_.begin() + (computeSet(address) * params_.assoc_);
    }
    
    iterator setEnd(Address address) {
        return blocks_.begin() + ((computeSet(address) + 1) * params_.assoc_);
    }

private:    
    Address computeSet(Address address) const { return (address & bitMasks_.setBits_) >> blockBits_; }

    int blockBits_{};
    Config::CacheParams params_{};
    BitMasks bitMasks_{};
    std::vector<Block> blocks_{};
};

template<typename Block>
CacheContainer<Block>::CacheContainer(const Config::CacheParams& params)
    : params_(params), blocks_(params.sets_ * params.assoc_) {
    constexpr int addressBits = sizeof(Address) * 8;
    blockBits_ = static_cast<int>(std::log2(params_.blockSize_));
    int setBits = static_cast<int>(std::log2(params_.sets_));
    int tagBits = addressBits - (blockBits_ + setBits);
    bitMasks_.setBits_ = Utils::makeMask(blockBits_, setBits);
    bitMasks_.tagBits_ = Utils::makeMask(blockBits_ + setBits, tagBits);
}

template<typename Block>
std::optional<std::reference_wrapper<Block>> CacheContainer<Block>::findBlock(Address address) {
    Address incomingTag = address & bitMasks_.tagBits_;
    for (auto itBegin = setBegin(address); itBegin != setEnd(address); ++itBegin) {
        if (itBegin->isValid()) {
            Address storedTag = itBegin->getAddress() & bitMasks_.tagBits_;
            if (storedTag == incomingTag) {
                return *itBegin;
            }
        }
    }
    return std::nullopt;
}
