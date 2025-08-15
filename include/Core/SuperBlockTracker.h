#pragma once

#include "Config/PrefetcherParams.h"
#include "Core/StatisticsManager.h"
#include "Core/Types.h"
#include "Core/SuperBlock.h"
#include "Core/CacheContainerManager.h"
#include "Config/CacheParams.h"
#include "Utils/VariantUtils.h"
#include <optional>

class SuperBlockTracker {
public:
	SuperBlockTracker(const Config::CacheParams& params);
	SuperBlockTracker(const Config::ControlUnitParams& params);

	std::optional<uint32_t> updateOnAccess(Address addr);

private:
	CacheContainerManager<SuperBlock> superBlockTracker_;
};

