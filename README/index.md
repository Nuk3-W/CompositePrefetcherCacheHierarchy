## Cache Hierarchy Simulator with Composite Prefetcher

This project is a configurable, single-core cache hierarchy simulator focused on evaluating a composite prefetcher. It includes LRU replacement, WBWA policy, optional victim caches, a superblock tracker, and multiple prefetching strategies.

### Key features
- Multi-level caches with LRU replacement and write-back/write-allocate
- Victim caches per level (optional)
- Composite prefetcher that switches between Sequential and Markov strategies which can be changed / swapped out
- Superblock tracker(tracks n amount of cachelines to identify spatial locality) and EWMA-based strategy selection
- Detailed statistics with per-strategy prefetch hit counts

### Quick start
1) Configure
- Edit `config.txt` in the repo root. See `README/configuration.md` for all options.

2) Build
- From a terminal:
  - `mkdir build`
  - `cd build`
  - `cmake ..`
  - `cmake --build .`

3) Run
- From the `build/` directory:
  - `./CacheSim` (or `CacheSim.exe` on Windows)

Traces are located under `tests/`. The simulator reads the trace file path from `config.txt`.

### Documentation map
- Architecture: `README/architecture.md`
- Prefetching design: `README/prefetching.md`
- Configuration: `README/configuration.md`
- Statistics: `README/stats.md`
- Development notes: `README/development.md`


