#ifndef LevelCache_H
#define LevelCache_H

#include <vector>
#include <cmath>

#include "CacheData.h"

constexpr Address g_invalidAddress;

class LevelCache {
public:
	LevelCache(CacheParams params);
	~LevelCache();

	virtual Address read(Address addr); //returns writeback address. 0 if none as 0x00000000 is not a valid address
	virtual Address write(Address addr);

	virtual Address writeBack(Address addr);
private:
	Address makeMask(int start, int length);

	void updateReadStats(bool hit);
	void updateWriteStats(bool hit);

	CacheParams params_;
	CacheStats stats_;

	// address and metadata storage
	std::vector<Address> cache_;
	std::vector<uint32_t> extraBits_; //if you put some insane paramters like a 2^2^100000 size cache thats fully associative this will break due to not having enough bits

	// bit masks for address decomposition
	Address tagBits_;
	Address setBits_;
	Address offsetBits_;
};
#endif // LevelCache_H