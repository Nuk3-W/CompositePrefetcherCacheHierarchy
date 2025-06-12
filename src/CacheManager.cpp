#include "CacheManager.h"

CacheManager::CacheManager(const SystemCacheParams& params){
    // Initialize normal caches
    for (const CacheParams& cacheParam : params.caches_) {
        caches_.emplace_back(LevelCache(cacheParam));
    }

    // Initialize victim caches
    for (const CacheParams& cacheParam : params.vCaches_) {
        vCaches_.emplace_back(VictimCache(cacheParam));
    }
}

CacheManager::~CacheManager() = default;

/*
Meaning of writeBack after read():
    - g_invalidAddress: Cache hit
    - addr: Cache miss, but no eviction occurred
    - Other address: Cache miss with eviction — write-back required

Read function structure:
    (1) If previous cache had an eviction, write back the evicted block {recursive}
    (2) Try to read from current level cache
            - If hit, return

    (3) If current cache has a victim cache:
        (4)   Write back evicted block if needed
        (5)   Try to read from victim cache
		(6)   If hit, swap the address with the one in the main cache
        (7)   continue to next level cache
*/
void CacheManager::read(Address addr) {
    Address writeBack = g_invalidAddress;

    for (int level = 0; level < caches_.size(); ++level) {
		handleLevelWriteBack(writeBack, level); // (1) 

        writeBack = caches_[level].read(addr); // (2)

        if (isCacheHit(writeBack)) {
            return;
        }

        if (vCaches_[level].isValid()) { // (3)
			handleVictimWriteBack(writeBack, level); // (4)

            writeBack = vCaches_[level].read(addr); // (5)
            if (isCacheHit(writeBack)) { // (6)
				// Swap the address in the main cache with the one in the victim cache [TODO]
                return;
			}
        }
    } // (7)
}

/*
Meaning of writeBack after write():
    - g_invalidAddress: Cache hit
    - addr: Cache miss, but no eviction occurred
    - Other address: Cache miss with eviction — write-back required

Write function structure:
    (1) If previous cache had an eviction, write back the evicted block {recursive}
    (2) Try to write from current level cache
            - If hit, return

    (3) If current cache has a victim cache:
        (4)   Write back evicted block if needed
        (5)   Try to write from victim cache
        (6)   If hit, swap the address with the one in the main cache
        (7)   continue to next level cache
*/
void CacheManager::write(Address addr) {
    Address writeBack = g_invalidAddress;

    for (int level = 0; level < caches_.size(); ++level) {
        handleLevelWriteBack(writeBack, level); // (1) 

        writeBack = caches_[level].write(addr); // (2)

        if (isCacheHit(writeBack)) return; 

        if (vCaches_[level].isValid()) { // (3)
            handleVictimWriteBack(writeBack, level); // (4)

            writeBack = vCaches_[level].write(addr); // (5)
            if (isCacheHit(writeBack)) { // (6)
                // Swap the address in the main cache with the one in the victim cache [TODO]
                return;
            }
        }
    } // (7)
}

bool CacheManager::isCacheHit(Address writeBack) const {
    return writeBack == g_invalidAddress;
}

// these are some coupled recursive functions due to how LevelCaches and VictimCaches are seperated in their own vectors {didnt want to use pointers to the classes themselves}
void CacheManager::handleLevelWriteBack(Address writeBack, int level) {
	if (level >= caches_.size()) return;

	if (writeBack == g_invalidAddress) return; // nothing to write back

	// Write back to this cache level
	Address evicted = caches_[level].writeBack(writeBack);

	// Try victim cache if valid and main cache evicted something
	if (vCaches_[level].isValid() && evicted != g_invalidAddress) {
		handleVictimWriteBack(evicted, level);
	} else {
		handleLevelWriteBack(evicted, level + 1);
	}
}

void CacheManager::handleVictimWriteBack(Address writeBack, int level) {
	if (level >= vCaches_.size() || !vCaches_[level].isValid()) return; // most likely redundant check

	if (writeBack == g_invalidAddress) return; // nothing to write back

    // Write back to victim cache at this level
    Address evicted = vCaches_[level].writeBack(writeBack);

    // Always go down to lower cache level next
    handleLevelWriteBack(evicted, level + 1);
}
// end of coupled recursive functions


