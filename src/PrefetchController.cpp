#include "Core/PrefetchController.h"

PrefetchController::PrefetchController(const Config::ControlUnitParams& params)
    : superBlockTracker_(params.trackerParams_),
      prefetchStrategies_(makeAll(std::make_index_sequence<Count>{})) {
}

void PrefetchController::updateTrackerOnAccess(Address addr) {
    auto result = superBlockTracker_.updateOnAccess(addr);
    bool isHit = result.has_value();
    hitEwma_.record(isHit);
    double r = hitEwma_.getRate();
    std::cout << "Hit rate: " << r << std::endl;
    if (r >= enableThresh_) {
        currentStrategy_ = Noop;
    } else if (r < disableThresh_) {
        currentStrategy_ = Noop;
    }
}


void PrefetchController::prefetch(Address missAddr) {
    auto prefetchAddr = prefetchStrategies_[currentStrategy_]->prefetch(missAddr);
    if (prefetchAddr) {
        prefetchBuffer_->initialize(*prefetchAddr, AccessType::Read);
    }
}