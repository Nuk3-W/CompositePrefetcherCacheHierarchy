#include "Core/CacheCollection.h"

CacheCollection::CacheCollection(const CacheParams& params) : params_(params), blocks_(params.sets_ * params.assoc_) {
    blockBits_ = static_cast<int>(std::log2(params_.blockSize_));
    int setBits   = static_cast<int>(std::log2(params_.sets_));
    int tagBits   = static_cast<int>(sizeof(Address) * 8 - (blockBits_ + setBits));

    bitMasks_.setBits_ = Utils::makeMask(blockBits_, setBits);
    bitMasks_.tagBits_ = Utils::makeMask(blockBits_ + setBits, tagBits);
}

std::optional<std::reference_wrapper<CacheBlock>> CacheCollection::findBlock(Address address) {
    for (auto itBegin = setBegin(address); itBegin != setEnd(address); ++itBegin) {
        if (itBegin->getTag() == address & bitMasks_.tagBits_ && itBegin->isValid()) {
            return *itBegin;
        }
    }
    return std::nullopt;
}

std::optional<std::reference_wrapper<const CacheBlock>> CacheCollection::findBlock(Address address) const { 
    for (auto itBegin = setBegin(address); itBegin != setEnd(address); ++itBegin) {
        if (itBegin->getTag() == address & bitMasks_.tagBits_ && itBegin->isValid()) {
            return *itBegin;
        }
    }
    return std::nullopt;
}

iterator CacheCollection::setBegin(Address address) {
    Address setIndex = (address & bitMasks_.setBits_) >> blockBits_;
    return blocks_.begin() + (setIndex * params_.assoc_);
}

const_iterator CacheCollection::setBegin(Address address) const {
    Address setIndex = (address & bitMasks_.setBits_) >> blockBits_;
    return blocks_.cbegin() + (setIndex * params_.assoc_);
}

iterator CacheCollection::setEnd(Address address) {
    Address setIndex = (address & bitMasks_.setBits_) >> blockBits_;
    return blocks_.begin() + ((setIndex + 1) * params_.assoc_);
}

const_iterator CacheCollection::setEnd(Address address) const {
    Address setIndex = (address & bitMasks_.setBits_) >> blockBits_;
    return blocks_.cbegin() + ((setIndex + 1) * params_.assoc_);
}