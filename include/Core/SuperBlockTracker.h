#pragma once

class SuperBlockTracker {
public:
    SuperBlockTracker(const Config::ControlUnitParams& params, uint32_t blockSize);

    void updateOnAccess(Address addr);
private:
    CacheContainerManager superBlockTracker_;
};