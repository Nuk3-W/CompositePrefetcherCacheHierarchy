#pragma once
#include "Core/SuperBlockTracker.h"
#include "Core/Types.h"
#include "Config/CacheParams.h"
#include "Interface/IPrefetchStrategy.h"
#include "Core/NoopPrefetchStrategy.h"
#include "Core/SequentialPrefetchStrategy.h"
#include "Core/MarkovPrefetchStrategy.h"
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

    void prefetch (Address missAddr);
    std::optional<std::reference_wrapper<CacheBlock>> getPrefetchBuffer() {
        if (prefetchBuffer_) return std::ref(*prefetchBuffer_);
        return std::nullopt;
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

    // it might be strange to use a single block as a stream buffer
    // but it's a simple way to implement a prefetch buffer with pipelined memory access
    std::optional<CacheBlock> prefetchBuffer_;

    EWMA hitEwma_{0.5};
    double enableThresh_{0.65};
    double disableThresh_{0.35};
};
