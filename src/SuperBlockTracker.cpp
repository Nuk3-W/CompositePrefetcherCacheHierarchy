#include "Core/SuperBlockTracker.h"

SuperBlockTracker::SuperBlockTracker(const Config::CacheParams& params) :
    superBlockTracker_(params) {}

SuperBlockTracker::SuperBlockTracker(const Config::ControlUnitParams& params) :
    superBlockTracker_(params.trackerParams_) {}

std::optional<uint32_t> SuperBlockTracker::updateOnAccess(Address addr) {
	auto result = superBlockTracker_.read(addr);
	SuperBlock& block = static_cast<SuperBlock&>(Utils::getBlock(result));

	if (Utils::isType<Hit>(result)) {
		block.incrementHits();
		StatisticsManager::getInstance().recordSuperBlockAccess(0, CacheResult::Hit);
		return block.getHits();
	} else {
		block.resetHits();
		block.initialize(addr, AccessType::Read);
		StatisticsManager::getInstance().recordSuperBlockAccess(0, CacheResult::Miss);
		return std::nullopt;
	}
}
