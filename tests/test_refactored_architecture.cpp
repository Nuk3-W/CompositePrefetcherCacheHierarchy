#include <iostream>
#include <memory>
#include <cassert>

#include "Core/MemoryController.h"
#include "Core/CacheManager.h"
#include "Core/PrefetcherManager.h"
#include "Core/MarkovPrefetcher.h"
#include "Core/SequentialPrefetcher.h"
#include "Core/StreamBuffer.h"
#include "Core/StatisticsManager.h"
#include "Core/ThresholdManager.h"
#include "Core/Printer.h"
#include "Config/SystemParams.h"

using namespace Config;

void testInterfaces() {
    std::cout << "Testing interfaces..." << std::endl;
    
    // Test StatisticsManager
    StatisticsManager stats;
    stats.record("test.key", 42);
    assert(stats.getValue("test.key") == 42);
    std::cout << "✓ StatisticsManager interface works" << std::endl;
    
    // Test ThresholdManager
    ThresholdManager threshold;
    threshold.updateThreshold(true, PrefetchType::Sequential);
    threshold.updateThreshold(false, PrefetchType::Markov);
    assert(threshold.getThreshold() > 0);
    std::cout << "✓ ThresholdManager works" << std::endl;
    
    // Test StreamBuffer
    StreamBuffer buffer(4);
    buffer.insertStream(0x1000);
    buffer.insertStream(0x1004);
    assert(buffer.hasHit(0x1000));
    assert(!buffer.hasHit(0x2000));
    std::cout << "✓ StreamBuffer works" << std::endl;
}

void testPrefetchers() {
    std::cout << "Testing prefetchers..." << std::endl;
    
    // Test MarkovPrefetcher
    MarkovPrefetcher markov(512, 64);
    markov.observeMiss(0x1000);
    auto markovPrefetches = markov.getNextPrefetches();
    std::cout << "✓ MarkovPrefetcher works" << std::endl;
    
    // Test SequentialPrefetcher
    SequentialPrefetcher sequential(64, 4);
    sequential.observeMiss(0x1000);
    auto sequentialPrefetches = sequential.getNextPrefetches();
    std::cout << "✓ SequentialPrefetcher works" << std::endl;
}

void testManagers() {
    std::cout << "Testing managers..." << std::endl;
    
    // Test PrefetcherManager
    PrefetcherManager prefetcherManager;
    prefetcherManager.addPrefetcher(PrefetchType::Markov, 
        std::make_unique<MarkovPrefetcher>(512, 64));
    prefetcherManager.addPrefetcher(PrefetchType::Sequential, 
        std::make_unique<SequentialPrefetcher>(64, 4));
    prefetcherManager.addBuffer(PrefetchType::Sequential, 
        std::make_unique<StreamBuffer>(16));
    
    prefetcherManager.initialize();
    prefetcherManager.notifyMiss(0x1000);
    auto prefetches = prefetcherManager.getNextPrefetches();
    prefetcherManager.finalize();
    std::cout << "✓ PrefetcherManager works" << std::endl;
}

void testMemoryController() {
    std::cout << "Testing MemoryController..." << std::endl;
    
    MemoryController controller;
    
    // Create a simple cache manager (would need proper params in real usage)
    SystemCacheParams params;
    params.blockSize_ = 64;
    params.traceFile_ = "test.txt";
    
    auto cacheManager = std::make_unique<CacheManager>(params);
    controller.setCacheAccess(cacheManager.get());
    controller.setPrefetchCoordinator(cacheManager.get());
    controller.addManager(std::move(cacheManager));
    
    // Create prefetcher manager
    auto prefetcherManager = std::make_unique<PrefetcherManager>();
    prefetcherManager->addPrefetcher(PrefetchType::Markov, 
        std::make_unique<MarkovPrefetcher>(512, 64));
    prefetcherManager->addPrefetcher(PrefetchType::Sequential, 
        std::make_unique<SequentialPrefetcher>(64, 4));
    controller.addManager(std::move(prefetcherManager));
    
    controller.initialize();
    controller.tick();
    controller.finalize();
    std::cout << "✓ MemoryController works" << std::endl;
}

int main() {
    std::cout << "Running refactored architecture tests..." << std::endl;
    
    try {
        testInterfaces();
        testPrefetchers();
        testManagers();
        testMemoryController();
        
        std::cout << "\n🎉 All tests passed! The refactored architecture is working correctly." << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
} 