#pragma once

#include <vector>
#include <cmath>
#include <bitset>
#include "Interface/IBlockCollection.h"
#include "Core/CacheBlock.h"
#include "Core/Types.h"
#include "Config/CacheParams.h"

class CacheContainer : public IBlockCollection<CacheBlock> {
public:
    CacheContainer(const Config::CacheParams& params);
    ~CacheContainer() override = default;

    std::optional<std::reference_wrapper<CacheBlock>> findBlock(Address address) override;

    std::size_t size() const override { return blocks_.size(); }

    iterator setBegin(Address address);
    iterator setEnd(Address address);

private:    
    int blockBits_{};
    Config::CacheParams params_{};
    BitMasks bitMasks_{};
    std::vector<CacheBlock> blocks_{};
};
