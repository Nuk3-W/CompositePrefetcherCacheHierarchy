#include "Core/GHB.h"

GHB::GHBEntry::GHBEntry(Address addr) : address(addr) {}


GHB::GHB(size_t bufferSize, uint32_t blockSize) : bufferSize_(bufferSize) {
    blockMask_ = ~0UL << static_cast<uint32_t>(std::log2(blockSize));
}

void GHB::insert(Address addr) {
    // Convert to block address before storing
    Address blockAddr = addr & blockMask_;
    
    if (ghbList_.size() == bufferSize_) {
        removeFromIndexTable(ghbList_.front().address, ghbList_.begin());
        ghbList_.pop_front();
    }
    ghbList_.emplace_back(blockAddr);
    auto it = std::prev(ghbList_.end());
    addToIndexTable(blockAddr, it);
}

std::vector<GHB::ListIt> GHB::findDuplicates(Address addr) const {
    std::vector<ListIt> result;
    auto range = indexTable_.equal_range(addr);
    for (auto it = range.first; it != range.second; ++it) {
        result.push_back(it->second);
    }
    return result;
}

void GHB::addToIndexTable(Address addr, ListIt it) {
    indexTable_.emplace(addr, it);
}

void GHB::removeFromIndexTable(Address addr, ListIt it) {
    auto range = indexTable_.equal_range(addr);
    for (auto itr = range.first; itr != range.second; ) {
        if (itr->second == it) {
            itr = indexTable_.erase(itr);
        } else {
            ++itr;
        }
    }
}

Address GHB::markovPredictor(Address missAddress) const {
    // Convert miss address to block address for lookup
    Address missBlockAddr = missAddress & blockMask_;
    
    std::unordered_map<Address, int> markovTable;
    std::vector<ListIt> duplicates = findDuplicates(missBlockAddr);

    for (ListIt it : duplicates) {
        // newer address at back of list
        auto nextIt = std::next(it);
        if (nextIt != ghbList_.end()) {
            markovTable[nextIt->address]++;
        }
    }
    // unholy magic number below just a placeholder for a non existent value because its basically never called
    if (markovTable.empty()) return ~0UL;
    auto maxElement = std::max_element(markovTable.begin(), markovTable.end(),
        [](const auto& a, const auto& b) {
            return a.second < b.second;
        });

    return maxElement->first;
}
