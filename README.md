# Cache Hierarchy Simulator with Composite Prefetcher

This project is a high-performance, configurable cache hierarchy simulator written in C++. It implements modern memory protocols such as Least Recently Used (LRU), Write Back Write Allocate (WBWA), and victim caches to achieve standard memory performance for single-core CPUs. 

The thing this memory hierarchy does differently is implement a custom composite prefetcher that swaps between two prefetching strategies: a Tagged Sequential Prefetcher and a Markov Prefetcher. This is done through a simple weighted control unit that determines these weights using a superblock, a hashed memory location derived from the idea of memory blocks which are seen within caches to maximize spatial locality. Based on how many hits are received in these superblocks gives a preference towards one of these prefetchers.


To run use this command: //you can change the numbers around to experiment with different cache configurations be careful with certain sized


./sim_cache 32 8192 4 7 262144 8 gcc_trace.txt 

./sim_cache BlockSize, L1Size, L1Assoc, VC#Blocks, L2Size, L2Assoc, ReadFile

## Tagged Sequential Prefetcher

This returns a prefetch buffer of sequential block addresses. However, to maintain sequential prefetches, it will prefetch based on any prefetcher hit which allows for hits on the second or third memory address after the first. One example is if we are getting an image in memory, once we miss, we fetch the next block, but we will eventually need the nextblock+1 which is what the tagged sequential prefetcher does.

## Global History Buffer

The Global History Buffer (GHB) tracks the previous n misses within the cache hierarchy using a doubly linked list. Each index points to any duplicates of itself within the GHB, which can initially be referenced through a shared hashed index table that holds pointers to the first entry of a unique address in the GHB. This is particularly useful when temporal locality extends beyond the scope of the L1 cache, which is typically the case due to its small size.

## Markov Prefetcher

The Markov Prefetcher will determine the most likely candidate based on the number of duplicate entries within the GHB buffer which contains the last n missed memory blocks. This is then flushed into the prefetch buffer to be searched.



An executable version of the file is contained in the files if you are, for some reason, too lazy to compile the main sim_cache.cc file.
