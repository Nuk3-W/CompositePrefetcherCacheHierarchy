#include "Core/MemoryController.h"

void MemoryController::read(Address addr) {
    AccessBus bus{addr, AccessType::Read};
    Access(bus);
}

void MemoryController::write(Address addr) {
    AccessBus bus{addr, AccessType::Write};
    Access(bus);
}

void MemoryController::Access(AccessBus bus) {
    AccessResult result = managers_.CacheManager_->access(bus);
    if (result.index() == 0) {
        return;
    }
    else {
        AccessResult result = managers_.PrefetchManager_->access(bus);
        if (result.index() == 0) {
            return;
        }
        else {
            AccessResult result = managers_.ThresholdManager_->access(bus);
        }
    }
}   