## Architecture Overview

### Components
- Memory controller (`Core/MemoryController`): orchestrates reads/writes across cache levels, manages misses/evictions, and integrates prefetching.
- Level cache (`Core/LevelCache`): set-associative cache with LRU replacement and write-back/write-allocate policy.
- Victim cache: optional per-level spill buffer.
- Prefetch controller (`Core/PrefetchController`): selects and manages prefetching strategies; stages predicted lines in a `PrefetchBuffer`.
- Prefetch strategies (`Core/*PrefetchStrategy`): `Noop`, `Sequential`, and `Markov`.
- Superblock tracker (`Core/SuperBlockTracker`): tracks locality via superblocks; feeds an EWMA hit-rate metric.
- Global History Buffer (`Core/GHB`): used by the Markov strategy to predict next likely block.

### Flow
1) An access enters at L1; hits are returned immediately.
2) On a miss, lower levels are probed and the block is installed.
3) Prefetch controller updates on access/miss and may stage a prefetch address.
4) Subsequent accesses probe the prefetch buffer first; on hit, the staged line is installed into lower levels.


