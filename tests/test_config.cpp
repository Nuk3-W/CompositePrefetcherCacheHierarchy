#include <iostream>
#include <cassert>
#include "SystemCacheData.h"

// Forward declaration of the function you want to test
void loadConfigFromFile(SystemCacheParams& params);

int main() {
    SystemCacheParams params{};
    loadConfigFromFile(params);

    // Test the system-wide bitmask utilities
    std::cout << "Testing system-wide bitmask utilities...\n";
    
    // Test block offset mask
    Address blockMask = createBlockOffsetMask(16);
    std::cout << "Block mask for 16-byte blocks: 0x" << std::hex << blockMask << std::dec << "\n";
    assert(blockMask == 0xFFFFFFFFFFFFFFF0); // Should mask out 4 bits (log2(16) = 4)
    
    // Test super block mask
    Address superBlockMask = createSuperBlockMask(16, 2);
    std::cout << "Super block mask for 16-byte blocks + 2 bits: 0x" << std::hex << superBlockMask << std::dec << "\n";
    assert(superBlockMask == 0xFFFFFFFFFFFFFFC0); // Should mask out 6 bits (4 + 2)
    
    std::cout << "All bitmask tests passed!\n";

    return 0;
}