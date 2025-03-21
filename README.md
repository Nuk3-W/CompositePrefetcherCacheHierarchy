# Cache Hierarchy Simulator with Composite Prefetcher

This project is a high-performance, configurable cache hierarchy simulator written in C++. It implements modern memory protocols such as Least Recently Used (LRU), Write Back Write Allocate (WBWA), and victim caches to achieve standard memory performance for single-core CPUs. 

The thing this memory hierarchy does differently is implement a custom composite prefetcher that swaps between two prefetching strategies: a Tagged Sequential Prefetcher and a Markov Prefetcher. This is done through a simple weighted control unit that determines these weights using a superblock, a hashed memory location derived from the idea of memory blocks which are seen within caches to maximize spatial locality. Based on how many hits are received in these superblocks gives a preference towards one of these prefetchers.


To run use this command: //you can change the numbers around to experiment with different cache configurations be careful with certain sized


./sim_cache 32 8192 4 7 262144 8 gcc_trace.txt 

./sim_cache BlockSize, L1Size, L1Assoc, VC#Blocks, L2Size, L2Assoc, ReadFile   #Format

## Tagged Sequential Prefetcher

This returns a prefetch buffer of sequential block addresses. However, to maintain sequential prefetches, it will prefetch based on any prefetcher hit which allows for hits on the second or third memory address after the first. One example is if we are getting an image in memory, once we miss, we fetch the next block, but we will eventually need the nextblock+1 which is what the tagged sequential prefetcher does.

## Control Unit

This is the main unit for determining the bias for either prefetcher based on the superblock access. Superblocks are made up of n blocks. This allows us to see what blocks are being locally accessed based on promixity and choose a specific prefetcher method accordingly.

## Global History Buffer

The Global History Buffer (GHB) tracks the previous n misses within the cache hierarchy using a doubly linked list. Each index points to any duplicates of itself within the GHB, which can initially be referenced through a shared hashed index table that holds pointers to the first entry of a unique address in the GHB. This is particularly useful when temporal locality extends beyond the scope of the L1 cache, which is typically the case due to its small size.

## Markov Prefetcher

The Markov Prefetcher will determine the most likely candidate based on the number of duplicate entries within the GHB buffer which contains the last n missed memory blocks. This is then flushed into the prefetch buffer to be searched.

## Thanks/Extras/Results

This project was initially done as research at North Carolina State University under the mentorship of Rahaf Abdullah. A great thanks to my mentor for helping me with this fun project as it incorporates components of modern cache design and helped me learn as a highschooler which many don't get the opportunity to do.

This project has a lot of future development that can be done; for example, using different prefetchers and minimizing hardware overhead. The big challenge with physically implementing this specific composite prefetcher is that it uses a non-negligible, but somewhat manageable, amount of memory to predict temporaly local memory addresses. With different, prefetchers this can become more practical for commercial use due to its flexibility when tackling different memory access patterns.

Given the said limitations, it was able to achieve 60% increased prefetcher hit rate compared to the individual prefetchers in the given trace benchmarks. This shows promise for these composite prefetcher as they tackle more complex memory access patterns in the future.

Please feel free to use this project by using sim_cache.cc main function or formatting if you want a custom cache hierarchy.

using the source data structures is somewhat annoying and difficult as this was written while learning c++. However, you should mostly focus on the control unit as that is the main determiner between prefetching methods. You can gut any prefetching method by writing a separate prefetcher class

//Format for those who want to have a custom cache hierarchy

Cache L2 = Cache(NULL, "L2", params.l2_size, params.l2_assoc, params.block_size, NULL);
Cache L1 = Cache(NULL, "L1", params.l1_size, params.l1_assoc, params.block_size, &L2);
Victim VL1 = Victim(&L1, params.vc_num_blocks, "VL1", (params.vc_num_blocks*params.block_size), params.vc_num_blocks, params.block_size, NULL);
PrefetchBuffer PF1 = PrefetchBuffer(&L1, "PrefetchBuffer", 16, 16, params.block_size, NULL); //prefetch buffer connected to L1
GHB GHB1 = GHB(512, 2); 
ControlUnit PrefetchController = ControlUnit(2);     //input pushback size
IT IndexTable = IT(512, params.block_size, &GHB1);
GHB1.ConnectToIT(&IndexTable);
L1.Assign(&VL1, &GHB1, &IndexTable, &PF1, &PrefetchController);  
PF1.Assign(NULL, &GHB1, &IndexTable, NULL, &PrefetchController);

## Notes

An executable version of the file is contained in the files if you are, for some reason, too lazy to compile the main sim_cache.cc file.
