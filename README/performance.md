## Performance Analysis

This section presents performance analysis results for the composite prefetcher cache hierarchy simulator using various benchmarks and parameter configurations.

### Accuracy vs Alpha Analysis

![Accuracy vs Alpha](../images/Accuracy%20vs%20Alpha.png)


Note: the config for this was 
block_size 16
l1_size 1024
l1_assoc 1
l1vc_num_blocks 16
l2_size 8192
l2_assoc 4
k_tracker 4
superblock_bits 3
enable_threshold .65
disable_threshold .35

This graph shows the relationship between the EWMA (Exponentially Weighted Moving Average) alpha parameter and prefetcher accuracy across different workloads. The alpha parameter controls how quickly the hit rate estimation adapts to recent behavior:

- **Lower alpha values** (closer to 0) make the EWMA more responsive to recent changes
- **Higher alpha values** (closer to 1) give more weight to historical data
- The optimal alpha appears to vary by workload characteristics
- Different traces show varying sensitivity to the alpha parameter

### Combined vs Individual Prefetcher Performance

Note: Changed SuperBlockTracker after analyzing more multiple combinations
trace_file compress_trace.txt
block_size 16
l1_size 1024
l1_assoc 1
l1vc_num_blocks 16
l2_size 8192
l2_assoc 4
k_tracker 6
superblock_bits 1
alpha 0.36
enable_threshold .65
disable_threshold .35

![Combined vs Individual Prefetchers](../images/Combined%20vs%20Individual%20Prefetchers.png)

This comparison demonstrates the effectiveness of the composite prefetcher approach versus using individual prefetching strategies alone:

- **Composite Prefetcher**: Dynamically switches between Sequential and Markov strategies based on hit rate thresholds
- **Sequential Only**: Uses only sequential prefetching (next-block prediction)
- **Markov Only**: Uses only Markov prefetching (pattern-based prediction)

The results show how different workloads benefit from different approaches:
- in almost all cases the combined prefetcher was better by roughly 

The following is percent better compared to worst individual prefetcher

- gcc ~116% improvement compared to markov
- go ~2116% improvement compared to markov
- vortex ~97% improvement compared to markov
- perl ~305% improvement compared to sequential
- compress ~2522 improvement compared to markov

As you can see the greatest benefit from this prefetcher is that it improves on the weak prefeture every time
Obviously those percentages I've just shown are inflated because they take the improvement against the weaker prefetcher but in 4/5 cases the combined prefetcher did better than the components while barly increasing the cost of the system.

This prefetcher across these 5 tests had roughly a 35% accuracy which considering that that these prefetchers are meant for smaller caches, this is highly performant for the cost.

### Key Insights

1. **Workload Dependency**: Prefetcher effectiveness varies significantly across different memory access patterns
2. **Parameter Sensitivity**: The alpha parameter requires tuning for optimal performance
3. **Strategy Selection**: No single prefetching strategy dominates across all workloads
4. **Adaptive Benefits**: The composite approach can outperform individual strategies when properly tuned while maintainig minimal memory bandwidth in the hiearchy