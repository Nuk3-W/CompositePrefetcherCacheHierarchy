#pragma once
#include <cstdint>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <variant>
#include "Core/IFacet.h"
#include "Core/Types.h"

// The travelling tray
class AccessBus {
public:
    Address      addr;
    AccessType   type;
    AccessResult result = Miss{};          // default assumption

    // ----- facet API -----
    template<typename FacetT, typename... Args>
    FacetT& ensureFacet(Args&&... args) {
        static_assert(std::is_base_of_v<IFacet, FacetT>);
        auto key = std::type_index(typeid(FacetT));
        auto& slot = facets_[key];
        if (!slot) slot = std::make_unique<FacetT>(std::forward<Args>(args)...);
        return static_cast<FacetT&>(*slot);
    }

    template<typename FacetT>
    FacetT* tryFacet() {
        auto it = facets_.find(std::type_index(typeid(FacetT)));
        return it == facets_.end() ? nullptr
                                   : static_cast<FacetT*>(it->second.get());
    }

private:
    std::unordered_map<std::type_index, std::unique_ptr<IFacet>> facets_;
};