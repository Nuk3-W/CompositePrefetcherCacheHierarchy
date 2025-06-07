#ifndef PREFETCHER_DATA_H
#define PREFETCHER_DATA_H

#include <string>

typedef struct GHBEntry {
    std::string Address;
    GHBEntry* Next = nullptr;
    GHBEntry* Previous = nullptr;
    GHBEntry* Duplicate = nullptr;
    int HashStep = 0;
    GHBEntry(std::string addr) : Address(addr), Next(nullptr) {}
} GHBEntry;

typedef struct GHBParams {

}GHBParams;





#endif // PREFETCHER_DATA_H