## Architecture

This simulator models a multi-level cache hierarchy with an optional victim cache per level and a composite prefetcher. This document summarizes each component, its responsibilities, and how requests flow through the system.

### Key data types
- `Address`: 32-bit address alias for `uint32_t`.
- `AccessResult`: `std::variant<Hit, Miss, Evict>`; each alternative holds a reference to the affected `CacheBlock`.
- `AccessType`: `Read` or `Write`.

### Components
- **Memory controller** (`Core/MemoryController`)
  - Orchestrates `read(addr)`/`write(addr)` across levels.
  - Coordinates with the prefetch controller; probes lower levels of memory and the prefetch buffer on misses.
- **Level cache** (`Core/LevelCache`)
  - Wraps the main set-associative cache and an optional victim cache.
  - Applies write-back/write-allocate policy and returns `Hit | Miss | Evict`.
  - Handles victim-cache swap when enabled.
- **Container manager** (`Core/CacheContainerManager`)
  - Fronts a `CacheContainer` plus replacement policy.
  - On hit: updates LRU and returns `Hit{block}`.
  - On miss: selects a line via replacement policy and returns `Miss{evicted}` or `Evict{evicted}` if dirty.
- **Cache container** (`Core/CacheContainer`)
  - Physical storage of `CacheBlock`s.
  - Computes set boundaries and performs tag lookup within a set.
- **Replacement policy** (`Core/CacheLRUReplacement`)
  - Maintains per-set LRU state and selects victims.
- **Victim cache**
  - Optional small buffer per level managed by `LevelCache` for swaps on misses.
- **Prefetch controller** (`Core/PrefetchController`)
  - Chooses between strategies; probes the `PrefetchBuffer` before accessing caches.
  - On misses, asks the active strategy for the next candidate and stages it.
- **Prefetch buffer** (`Core/PrefetchBuffer`)
  - Stream buffer that holds the most recently prefetched block (line-aligned match on probe).
- **Prefetch strategies** (`Core/*PrefetchStrategy`)
  - `Noop`, `Sequential`, and `Markov` implement `IPrefetchStrategy`.
- **Superblock tracker** (`Core/SuperBlockTracker`)
  - Tracks locality using superblocks to bias strategy selection.
- **Global History Buffer** (`Core/GHB`)
  - Records recent miss sequences used by the Markov strategy.

### Read flow
1. Probe `PrefetchController::probe(addr)` to check the `PrefetchBuffer`.
2. If the probe hits, use the offered block and continue.
3. Otherwise access L1 `LevelCache::read(addr)`.
4. If `Hit`, return. If not, proceed to the next level.
5. On a final-level miss, allocate on the way back according to write-allocate.
6. Notify the prefetch controller of the miss to stage the next predicted block.

### Write flow
1. Same as read, but on `Hit` mark the returned block as dirty.
2. On miss, a victim may be evicted; dirty victims are surfaced as `Evict` to the caller for write-back handling at a higher level.

### Level cache internals
- Main path: `CacheContainerManager::read/write` → `CacheContainer::findBlock`.
- Hit: update LRU and return `Hit{block}`.
- Miss: `CacheLRUReplacement::evict` selects a line and returns `Miss{evicted}` or `Evict{evicted}` if dirty.
- Victim path (when present): `LevelCache` may swap the requested line from the victim cache into the main cache set.

### Replacement and metadata
- LRU, valid, and dirty are tracked in `CacheBlock` metadata.
- LRU is incremented and masked per access; writes set the dirty bit.

### Prefetching
Modern prefetchers often adopt monolithic strategies, which perform well on specific access patterns but degrade significantly on others. More advanced prefetchers broaden coverage at the cost of increased hardware complexity and bandwidth demands.

My approach starts from the observation that caches already capture spatial and temporal locality effectively. To build on this, I partition memory into superblocks—groups of n consecutive cache lines. A small superblock tracker, weighted by an exponentially weighted moving average, monitors recent access patterns. If accesses remain within superblocks, spatial locality dominates; otherwise, temporal locality is inferred. Based on a threshold, the prefetcher dynamically switches between strategies.

Since this design targets the L1 cache, strict timing and bandwidth constraints apply: prefetches must complete within 1–5 CPU cycles, ruling out aggressive multi-line requests. To address this, I adopt a pipelined stream-buffer model, inspired by Jouppi. The prefetcher enqueues predicted lines but issues only one request per cycle from the queue head, thus reducing bandwidth demands while retaining responsiveness. This allows for a pipelined 1-cycle fetch, meeting the demands of the L1 cache without polluting the L1 cache, thanks to the dedicated buffer.

While prefetching can significantly reduce miss penalties in small or latency-critical caches, its benefits diminish as caches grow larger and more sophisticated. Larger caches naturally increase hit rates, especially for workloads with strong spatial or temporal locality, leaving fewer misses for the prefetcher to exploit. Moreover, advanced cache hierarchies already implement techniques such as larger line sizes, victim buffers, or inclusive/exclusive policies that reduce compulsory and conflict misses.

In such contexts, prefetching not only offers diminishing returns but may also compete with demand requests for bandwidth, potentially harming overall performance. This makes prefetching strategies most valuable in the context of low-cost caches, where hardware cannot simply scale capacity or associativity to improve performance. By targeting the L1 cache in particular, this design leverages lightweight tracking and pipelined prefetching to enhance effective hit rates without incurring the area, power, or complexity penalties of larger hierarchies.

Moreover, because this cache hierarchy uses a Global History Buffer, different prefetching methods can be implemented at low cost, since miss data is already recorded.



