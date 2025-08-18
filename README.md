## Composite Prefetcher Cache Hierarchy Simulator

Simple, configurable cache hierarchy simulator in C++17. Models multilayer caches with LRU replacement, write-back/write-allocate, optional victim cache, and a composite prefetcher blending sequential and Markov strategies that has a dedicated stream buffer.

### Quick Start

**Prerequisites**
- CMake 3.10+
- C++17 compiler (MSVC, Clang, GCC)

**Build**
1. Create a build directory and configure:

```bash
# Single-config (Make/Ninja)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Multi-config (Visual Studio/Xcode)
cmake -S . -B build
cmake --build build --config Release

# Linux/macOS
./build/CacheSim

# Windows (multi-config)
build\Release\CacheSim.exe
```
Notes:
- `config.txt` and the `tests/` directory are copied next to the executable after build, if you change them you must rebuild or change directly in the build directory.
- Edit `config.txt` to adjust hierarchy sizes, associativity, victim cache, and trace path.

### Configuration

Edit `config.txt` to configure the simulator.

- `block_size`: base cache block size (bytes). Also used for prefetch alignment.
- `trace_file`: filename under `tests/` to run (do not include the path).
- `l1_size`, `l1_assoc`, `l2_size`, `l2_assoc`, ...: cache sizing per level.
- `l1vc_num_blocks`, `l2vc_num_blocks`, ...: victim cache blocks per level.
- `k_tracker`: superblock capacity of tracker (number of superblocks tracked).
- `superblock_bits`: number of block-size shifts composing a superblock (e.g., 3 → 8 cache lines).

### Learn more
- Architecture overview: [README/architecture.md](README/architecture.md)

### Papers

See the curated list in [README/papers.md](README/papers.md).
