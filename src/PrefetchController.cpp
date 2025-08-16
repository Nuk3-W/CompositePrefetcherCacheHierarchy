#include "Core/PrefetchController.h"

PrefetchController::PrefetchController(const Config::ControlUnitParams& params)
    : superBlockTracker_(params.trackerParams_),
      prefetchStrategies_(makeAll(std::make_index_sequence<Count>{})) {
}

void PrefetchController::updateTrackerOnAccess(Address addr) {
    auto result = superBlockTracker_.updateOnAccess(addr);
    if (result) {
        currentStrategy_ = (*result > threshold_) ? Noop : Noop;
    }
}

void PrefetchController::updateOnMiss(Address addr) {
    
}