# Refactored Cache Hierarchy Simulator Architecture

## Overview

This document describes the refactored architecture of the cache hierarchy simulator, which has been redesigned to follow SOLID principles and improve maintainability, extensibility, and testability.

## SOLID Principles Implementation

### 1. Single Responsibility Principle (SRP)

Each class now has a single, well-defined responsibility:

- **MemoryController**: Central orchestration and lifecycle management
- **CacheManager**: Cache access operations and hierarchy management
- **PrefetcherManager**: Prefetcher coordination and selection
- **ThresholdManager**: Threshold tracking and decision logic
- **StatisticsManager**: Statistics collection and management
- **Printer**: Output rendering
- **MarkovPrefetcher**: Markov-based prefetching algorithm
- **SequentialPrefetcher**: Sequential prefetching algorithm
- **StreamBuffer**: Stream queue management

### 2. Open/Closed Principle (OCP)

The system is now open for extension but closed for modification:

- New prefetcher types can be added by implementing `IPrefetcher`
- New buffer types can be added by implementing `IBuffer`
- New cache types can be added by implementing `ICacheAccess`
- New statistics providers can be added by implementing `IStatisticsProvider`

### 3. Interface Segregation Principle (ISP)

Interfaces are segregated by responsibility:

- `IManager`: Lifecycle management for all managers
- `ICacheAccess`: Cache access operations
- `IPrefetcher`: Prefetcher algorithm interface
- `IBuffer`: Buffer management interface
- `IStatisticsProvider`: Statistics reporting interface
- `IPrefetchCoordinator`: Prefetch coordination interface

### 4. Dependency Inversion Principle (DIP)

High-level modules depend on abstractions, not concrete implementations:

- `MemoryController` depends on `IManager`, `ICacheAccess`, and `IPrefetchCoordinator`
- `PrefetcherManager` depends on `IPrefetcher` and `IBuffer`
- All components depend on `IStatisticsProvider` for statistics

## Architecture Components

### Core Interfaces

#### IManager
```cpp
class IManager {
public:
    virtual void initialize() = 0;
    virtual void tick() = 0;
    virtual void finalize() = 0;
    virtual ~IManager() = default;
};
```

#### ICacheAccess
```cpp
class ICacheAccess {
public:
    virtual AccessResult access(Address addr, AccessType type) = 0;
    virtual ~ICacheAccess() = default;
};
```

#### IPrefetcher
```cpp
class IPrefetcher {
public:
    virtual void observeMiss(Address addr) = 0;
    virtual std::vector<Address> getNextPrefetches() = 0;
    virtual ~IPrefetcher() = default;
};
```

#### IBuffer
```cpp
class IBuffer {
public:
    virtual bool hasHit(Address addr) const = 0;
    virtual void insertStream(Address startAddr) = 0;
    virtual std::optional<Address> getNext() = 0;
    virtual ~IBuffer() = default;
};
```

### Core Components

#### MemoryController
- **Responsibility**: Central orchestrator for the entire system
- **Dependencies**: All managers via `IManager` interface
- **Key Methods**:
  - `initialize()`: Initialize all managers
  - `processAccess()`: Handle memory access requests
  - `finalize()`: Finalize all managers and collect statistics

#### CacheManager
- **Responsibility**: Cache hierarchy management and access operations
- **Implements**: `IManager`, `ICacheAccess`, `IPrefetchCoordinator`, `IStatisticsProvider`
- **Key Features**:
  - Multi-level cache hierarchy management
  - Victim cache support
  - Statistics collection

#### PrefetcherManager
- **Responsibility**: Prefetcher coordination and selection
- **Implements**: `IManager`, `IStatisticsProvider`
- **Key Features**:
  - Multiple prefetcher management
  - Dynamic prefetcher selection
  - Threshold-based decision making

#### ThresholdManager
- **Responsibility**: Threshold tracking and decision logic
- **Key Features**:
  - Performance tracking for different prefetcher types
  - Adaptive threshold adjustment
  - Hit rate calculation

#### StatisticsManager
- **Responsibility**: Centralized statistics collection
- **Key Features**:
  - Statistics registration and collection
  - Scoped statistics (dot notation)
  - Provider-based statistics gathering

## Usage Example

```cpp
// Create memory controller
MemoryController controller;

// Create and configure cache manager
auto cacheManager = std::make_unique<CacheManager>(params);
controller.setCacheAccess(cacheManager.get());
controller.setPrefetchCoordinator(cacheManager.get());
controller.addManager(std::move(cacheManager));

// Create and configure prefetcher manager
auto prefetcherManager = std::make_unique<PrefetcherManager>();

// Add prefetchers
auto markovPrefetcher = std::make_unique<MarkovPrefetcher>(512, blockSize);
prefetcherManager->addPrefetcher(PrefetchType::Markov, std::move(markovPrefetcher));

auto sequentialPrefetcher = std::make_unique<SequentialPrefetcher>(blockSize, 4);
prefetcherManager->addPrefetcher(PrefetchType::Sequential, std::move(sequentialPrefetcher));

controller.addManager(std::move(prefetcherManager));

// Initialize and run simulation
controller.initialize();
// ... process trace file ...
controller.finalize();
controller.printResults();
```

## Benefits of Refactored Architecture

### 1. Maintainability
- Clear separation of concerns
- Single responsibility for each component
- Reduced coupling between components

### 2. Extensibility
- Easy to add new prefetcher types
- Easy to add new cache types
- Easy to add new statistics providers

### 3. Testability
- Components can be tested in isolation
- Mock implementations can be easily created
- Interface-based testing

### 4. Flexibility
- Dynamic component configuration
- Runtime prefetcher selection
- Configurable statistics collection

### 5. Performance
- Efficient interface-based communication
- Minimal overhead from abstraction layers
- Optimized data structures

## Migration Guide

### From Old Architecture to New

1. **Replace ControlUnit with PrefetcherManager**:
   - Old: `ControlUnit` managed both threshold and prefetchers
   - New: `PrefetcherManager` coordinates prefetchers, `ThresholdManager` handles thresholds

2. **Replace direct cache access with ICacheAccess**:
   - Old: Direct cache manipulation
   - New: Interface-based cache access

3. **Replace manual statistics with StatisticsManager**:
   - Old: Manual statistics tracking
   - New: Centralized statistics collection via `IStatisticsProvider`

4. **Replace hardcoded prefetchers with IPrefetcher**:
   - Old: Hardcoded Markov and Sequential prefetchers
   - New: Pluggable prefetcher implementations

## Future Enhancements

### 1. Additional Prefetcher Types
- Stride prefetcher
- Pattern-based prefetcher
- Machine learning-based prefetcher

### 2. Additional Cache Types
- Set-associative cache
- Fully associative cache
- Non-uniform cache architecture (NUCA)

### 3. Additional Statistics
- Power consumption statistics
- Bandwidth utilization
- Cache coherence statistics

### 4. Configuration Management
- JSON-based configuration
- Runtime configuration updates
- Configuration validation

## Testing Strategy

### Unit Tests
- Test each component in isolation
- Mock dependencies using interfaces
- Test edge cases and error conditions

### Integration Tests
- Test component interactions
- Test end-to-end workflows
- Test performance characteristics

### Performance Tests
- Benchmark different configurations
- Compare with original implementation
- Validate performance improvements

## Conclusion

The refactored architecture successfully addresses the SOLID principle violations in the original design while maintaining the same functionality and improving the overall code quality. The new design is more maintainable, extensible, and testable, making it easier to add new features and optimize performance in the future. 