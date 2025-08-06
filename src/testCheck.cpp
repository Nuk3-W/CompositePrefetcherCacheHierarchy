#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <limits>
#include <bitset>
#include <cassert>

#include "Core/CacheBlock.h"
#include "Core/CacheContainer.h"
#include "Core/CacheContainerManager.h"
#include "Core/LevelCache.h"
#include "Config/CacheParams.h"
#include "Utils/VariantUtils.h"

void testFullyAssociativeMainCache() {
    using std::cout;
    using std::endl;
    
    cout << "\n=== Testing Fully Associative Main Cache (No Victim Cache) ===" << endl;
    
    Config::CacheParams mainParams{};
    mainParams.sets_ = 1;  // Fully associative = 1 set
    mainParams.assoc_ = 4; // 4-way fully associative
    mainParams.blockSize_ = 64;
    mainParams.size_ = mainParams.sets_ * mainParams.assoc_ * mainParams.blockSize_;
    
    LevelCache levelCache(mainParams, Config::CacheParams{}, 0);
    
    cout << "Cache configuration: " << mainParams.sets_ << " set, " 
         << mainParams.assoc_ << "-way fully associative, " << mainParams.blockSize_ 
         << " byte blocks (no victim cache)" << endl;
    cout << "Cache size: " << mainParams.size_ << " bytes" << endl << endl;
    
    cout << "Test 1: Fill the fully associative cache" << endl;
    Address addresses[] = {0x1000, 0x2000, 0x3000, 0x4000};
    
    for (int i = 0; i < 4; ++i) {
        auto result = levelCache.write(addresses[i]);
        if (Utils::isType<Miss>(result)) {
            cout << "✓ Address 0x" << std::hex << addresses[i] << std::dec << " - Miss" << endl;
            auto& block = Utils::getBlock(result);
            block.setAddress(addresses[i]);
            block.setValid();
        } else {
            cout << "✗ Address 0x" << std::hex << addresses[i] << std::dec << " - Unexpected result" << endl;
        }
    }
    
    cout << "\nTest 2: Verify all blocks are in cache (should be hits)" << endl;
    for (int i = 0; i < 4; ++i) {
        auto result = levelCache.write(addresses[i]);
        if (Utils::isType<Hit>(result)) {
            cout << "✓ Address 0x" << std::hex << addresses[i] << std::dec << " - Hit" << endl;
        } else {
            cout << "✗ Address 0x" << std::hex << addresses[i] << std::dec << " - Expected Hit" << endl;
        }
    }
    
    cout << "\nTest 3: Trigger eviction (5th address should evict LRU)" << endl;
    Address newAddr = 0x5000;
    auto result = levelCache.read(newAddr);
    if (Utils::isType<Evict>(result)) {
        cout << "✓ Address 0x" << std::hex << newAddr << std::dec << " - Evict (LRU block evicted)" << endl;
        auto& block = Utils::getBlock(result);
        block.setAddress(newAddr);
        block.setValid();
    } else {
        cout << "✗ Address 0x" << std::hex << newAddr << std::dec << " - Expected Evict" << endl;
    }
    
    cout << "\nTest 4: Test LRU behavior - access oldest block should trigger eviction" << endl;
    Address oldestAddr = 0x1000; // First address accessed
    auto result2 = levelCache.read(oldestAddr);
    if (Utils::isType<Evict>(result2)) {
        cout << "✓ Address 0x" << std::hex << oldestAddr << std::dec << " - Evict (LRU updated)" << endl;
        auto& block = Utils::getBlock(result2);
        block.setAddress(oldestAddr);
        block.setValid();
        block.clearDirty();
    } else {
        cout << "✗ Address 0x" << std::hex << oldestAddr << std::dec << " - Expected Hit" << endl;
    }
    
    cout << "\nTest 5: Add another address to trigger eviction of new LRU" << endl;
    Address anotherAddr = 0x2000;
    auto result3 = levelCache.read(anotherAddr);
    if (Utils::isType<Evict>(result3)) {
        cout << "✓ Address 0x" << std::hex << anotherAddr << std::dec << " - Evict" << endl;
        auto& block = Utils::getBlock(result3);
        block.setAddress(anotherAddr);
        block.setValid();
    } else {
        cout << "✗ Address 0x" << std::hex << anotherAddr << std::dec << " - Expected Evict" << endl;
    }
    
    cout << "\nTest 6: Test write operations with dirty bits" << endl;
    auto writeResult = levelCache.write(addresses[1]); // Write to 0x2000
    if (Utils::isType<Hit>(writeResult)) {
        cout << "✓ Write to 0x" << std::hex << addresses[1] << std::dec << " - Hit" << endl;
        auto& block = Utils::getBlock(writeResult);
        cout << "  Block is dirty: " << (block.isDirty() ? "Yes" : "No") << endl;
    } else {
        cout << "✗ Write to 0x" << std::hex << addresses[1] << std::dec << " - Expected Hit" << endl;
    }
    
    cout << "\nTest 7: Test dirty block eviction" << endl;
    Address dirtyEvictAddr = 0x7000;
    auto dirtyEvictResult = levelCache.read(dirtyEvictAddr);
    if (Utils::isType<Evict>(dirtyEvictResult)) {
        cout << "✓ Address 0x" << std::hex << dirtyEvictAddr << std::dec << " - Evict (dirty block evicted)" << endl;
        auto& block = Utils::getBlock(dirtyEvictResult);
        block.setAddress(dirtyEvictAddr);
        block.setValid();
        cout << "  Evicted block is dirty: " << (block.isDirty() ? "Yes" : "No") << endl;
    } else {
        cout << "✗ Address 0x" << std::hex << dirtyEvictAddr << std::dec << " - Expected Evict" << endl;
    }
    
    cout << "\n=== Fully Associative Main Cache Test Complete ===" << endl;
}

int main() {
    testFullyAssociativeMainCache();
    return 0;
}







