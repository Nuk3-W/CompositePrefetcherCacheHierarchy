#include "Core/CacheContainer.h"


CacheContainer::CacheContainer(const Config::CacheParams& params) : params_(params), blocks_(params.sets_ * params.assoc_) {
    constexpr int addressBits = sizeof(Address) * 8;
    
    blockBits_ = static_cast<int>(std::log2(params_.blockSize_));
    int setBits = static_cast<int>(std::log2(params_.sets_));
    int tagBits = addressBits - (blockBits_ + setBits);

    bitMasks_.setBits_ = Utils::makeMask(blockBits_, setBits);
    bitMasks_.tagBits_ = Utils::makeMask(blockBits_ + setBits, tagBits);
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




