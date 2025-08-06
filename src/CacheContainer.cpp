#include "Core/CacheContainer.h"


CacheContainer::CacheContainer(const Config::CacheParams& params) : params_(params), blocks_(params.sets_ * params.assoc_) {
    blockBits_ = static_cast<int>(std::log2(params_.blockSize_));
    int setBits   = static_cast<int>(std::log2(params_.sets_));
    int tagBits   = static_cast<int>(sizeof(Address) * 8 - (blockBits_ + setBits));

    bitMasks_.setBits_ = Utils::makeMask(blockBits_, setBits);
    bitMasks_.tagBits_ = Utils::makeMask(blockBits_ + setBits, tagBits);

    std::cout << "setBits:   " << std::bitset<32>(bitMasks_.setBits_) << std::endl;
    std::cout << "tagBits:   " << std::bitset<32>(bitMasks_.tagBits_) << std::endl;
}

std::optional<std::reference_wrapper<CacheBlock>> CacheContainer::findBlock(Address address) {
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

std::optional<std::reference_wrapper<const CacheBlock>> CacheContainer::findBlock(Address address) const { 
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

CacheContainer::iterator CacheContainer::setBegin(Address address) {
    Address setIndex = (address & bitMasks_.setBits_) >> blockBits_;
    return blocks_.begin() + (setIndex * params_.assoc_);
}

CacheContainer::const_iterator CacheContainer::setBegin(Address address) const {
    Address setIndex = (address & bitMasks_.setBits_) >> blockBits_;
    return blocks_.cbegin() + (setIndex * params_.assoc_);
}

CacheContainer::iterator CacheContainer::setEnd(Address address) {
    Address setIndex = (address & bitMasks_.setBits_) >> blockBits_;
    return blocks_.begin() + ((setIndex + 1) * params_.assoc_);
}

CacheContainer::const_iterator CacheContainer::setEnd(Address address) const {
    Address setIndex = (address & bitMasks_.setBits_) >> blockBits_;
    return blocks_.cbegin() + ((setIndex + 1) * params_.assoc_);
}


