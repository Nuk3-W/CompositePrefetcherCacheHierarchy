## Configuration

Edit `config.txt` to configure the simulator.

### Keys
- `block_size`: base cache block size (bytes). Also used for prefetch alignment.
- `trace_file`: filename under `tests/` to run.
- `l1_size`, `l1_assoc`, `l2_size`, `l2_assoc`, ...: cache sizing per level.
- `v1_blocks`, `v2_blocks`, ...: victim cache blocks per level.
- `k_tracker`: superblock tracker associativity (if set, enables tracker and prefetching).
- `superblock_bits`: number of block-size shifts composing a superblock.


