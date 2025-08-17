## Prefetching

### Strategies
- Noop: Disabled prefetching.
- Sequential: Predicts the next aligned block from the current access.
- Markov: Uses a Global History Buffer (GHB) to predict the most frequent successor block.

### Controller
- Maintains multiple strategies and a current strategy selection.
- Uses an EWMA of superblock hits to switch between strategies.
- Stages predictions in a `PrefetchBuffer` that aligns addresses to block size and exposes a `read(addr)` probe.

### Stats
- Per-strategy prefetch hits are recorded when a probe finds the staged line.


