#ifndef LevelCache_H
#define LevelCache_H

#include <vector>

#include "CacheData.h"

class LevelCache {
public:
	LevelCache(CacheParams params);
	~LevelCache();

	Address read(Address addr); //returns writeback address. 0 if none as 0x00000000 is not a valid address
	Address write(Address addr); 
	Address writeBack(Address addr);
private:
	CacheParams params_;
	CacheStats stats_;

	std::vector<Address> cache_;
	std::vector<uint8_t> extraBits_;
};
#endif // LevelCache_H