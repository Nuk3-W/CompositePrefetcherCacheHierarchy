#pragma once
#include "Core/SuperBlockTracker.h"
#include "Core/Types.h"
#include "Config/CacheParams.h"
#include "Interface/IPrefetchStrategy.h"
#include "Core/NoopPrefetchStrategy.h"
#include "Core/SequentialPrefetchStrategy.h"
#include "Core/MarkovPrefetchStrategy.h"
#include "Core/PrefetchBuffer.h"
#include "Core/StatisticsManager.h"
#include "Utils/VariantUtils.h"


#include <array>
#include <memory>
#include <tuple>
#include <optional>
#include "Core/EWMA.h"

class PrefetchController {
public:
    // used for strategy creation
    using Strategies = std::tuple<NoopPrefetchStrategy, SequentialPrefetchStrategy, MarkovPrefetchStrategy>;
    enum StrategyType { Noop, Sequential, Markov };
    static constexpr size_t Count = std::tuple_size_v<Strategies>;
    
    PrefetchController(const Config::ControlUnitParams& params);
    ~PrefetchController() = default;
    
    PrefetchController(const PrefetchController&) = delete;
    PrefetchController& operator=(const PrefetchController&) = delete;
    
    PrefetchController(PrefetchController&&) = default;
    PrefetchController& operator=(PrefetchController&&) = default;
    
    void updateTrackerOnAccess(Address addr);
    void updateOnMiss(Address addr);

    std::reference_wrapper<CacheBlock> prefetch (Address missAddr);
    AccessResult probe(Address addr) { 
        AccessResult result = prefetchBuffer_.read(addr);
        if (Utils::isType<Hit>(result)) {
            StatisticsManager::getInstance().recordPrefetchHit(prefetchBuffer_.getCurrentStrategy());
        }
        return result;
    }



private:
    template <typename T>
    static std::unique_ptr<IPrefetchStrategy> make() {
        return std::make_unique<T>();
    }
    
    template<size_t... I>
    static std::array<std::unique_ptr<IPrefetchStrategy>, Count>
    makeAll(std::index_sequence<I...>) {
        return { make<std::tuple_element_t<I, Strategies>>()... };
    }

private:
    SuperBlockTracker superBlockTracker_;

    std::array<std::unique_ptr<IPrefetchStrategy>, Count> prefetchStrategies_;
    StrategyType currentStrategy_{Noop};

    PrefetchBuffer prefetchBuffer_;

    EWMA hitEwma_;
    double enableThresh_;
    double disableThresh_;
};
