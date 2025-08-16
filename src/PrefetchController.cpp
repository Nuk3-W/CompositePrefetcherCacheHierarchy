#include "Core/PrefetchController.h"

PrefetchController::PrefetchController(const Config::ControlUnitParams& params)
    : superBlockTracker_(params.trackerParams_),
      prefetchStrategies_([&]{
          std::array<std::unique_ptr<IPrefetchStrategy>, Count> a{};
          a[Noop] = std::make_unique<NoopPrefetchStrategy>();
          a[Sequential] = std::make_unique<SequentialPrefetchStrategy>(params.prefetchBlockSize_);
          a[Markov] = std::make_unique<MarkovPrefetchStrategy>(params.prefetchBlockSize_);
          return a;
      }()) {
}

void PrefetchController::updateTrackerOnAccess(Address addr) {
    auto result = superBlockTracker_.updateOnAccess(addr);
    bool isHit = result.has_value();
    hitEwma_.record(isHit);
    double r = hitEwma_.getRate();
    //std::cout << "Hit rate: " << r << std::endl;
    if (r >= enableThresh_) currentStrategy_ = Sequential;
    else if (r < disableThresh_) currentStrategy_ = Markov;
    prefetchStrategies_[Markov]->onAccess(addr);
}


void PrefetchController::prefetch(Address missAddr) {
    auto prefetchAddr = prefetchStrategies_[currentStrategy_]->prefetch(missAddr);
    if (prefetchAddr) {
        std::cout << "Prefetching: " << std::hex << *prefetchAddr << std::dec << std::endl;
        std::cout << "Prefetching type: " << currentStrategy_ << std::endl;
        prefetchBuffer_.initialize(*prefetchAddr, AccessType::Read);
    }
}

void PrefetchController::updateOnMiss(Address addr) {
    prefetchStrategies_[Markov]->onAccess(addr);
}