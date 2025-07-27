#ifndef CACHE_DATA_H
#define CACHE_DATA_H

//I aint namespacing this shit
using Address = unsigned long;

struct CacheParams {
    unsigned long blockSize_;
    unsigned long size_;
    unsigned long sets_;
    unsigned long assoc_;
};

struct CacheStats {
    unsigned long hits_;
    unsigned long misses_;
    unsigned long writeMiss_;
    unsigned long readMiss_;
    unsigned long writes_;
    unsigned long reads_;
    unsigned long writeBacks_;
    unsigned long swapRequests_;
    unsigned long swapHits_;
};

#endif // CACHE_DATA_H
