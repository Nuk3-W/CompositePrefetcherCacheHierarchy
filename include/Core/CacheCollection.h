#pragma once

#include <vector>
#include "Interface/IBlockCollection.h"
#include "Core/CacheBlock.h"
#include "Config/CacheParams.h"

class CacheCollection : public IBlockCollection<CacheBlock> {
public:
    CacheCollection(const CacheParams& params);
    ~CacheCollection() override = default;

    std::optional<std::reference_wrapper<CacheBlock>> findBlock(Address address) override;
    std::optional<std::reference_wrapper<const CacheBlock>> findBlock(Address address) const override;

    iterator begin() override { return blocks_.begin(); }
    const_iterator begin() const override { return blocks_.cbegin(); }
    iterator end() override { return blocks_.end(); }
    const_iterator end() const override { return blocks_.cend(); }
    std::size_t size() const override { return blocks_.size(); }

    // Cache Specific Functions -------------------------------------------
    iterator setBegin(Address address) override;
    const_iterator setBegin(Address address) const override;
    iterator setEnd(Address address) override;
    const_iterator setEnd(Address address) const override;

private:
    int blockBits_{};
    Config::CacheParams params_{};
    Types::BitMasks bitMasks_{};
    std::vector<CacheBlock> blocks_{};
};

