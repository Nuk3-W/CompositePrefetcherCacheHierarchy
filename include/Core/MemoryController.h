#pragma once

#include "Core/Types.h"

class MemoryController {
public:
    MemoryController(const SystemParams& params);

    void read(Address addr);
    void write(Address addr);
private:
    void Access(AccessBus bus);
private:
    struct Mangers {
        std::Unique_ptr<IManager> CacheManager_;
        std::Unique_ptr<IManager> PrefetchManager_;
        std::Unique_ptr<IManager> ThresholdManager_;
    };

    Mangers managers_;
}