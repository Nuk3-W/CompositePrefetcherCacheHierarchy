#ifndef LevelCache_H
#define LevelCache_H

#include <vector>
#include <cmath>

#include "CacheData.h"

class LevelCache {
public:
	LevelCache(CacheParams params);
	virtual ~LevelCache();

	//returns writeback address. 0 if none as 0x00000000 is not a valid address
	virtual Address read(Address addr); 
	virtual Address write(Address addr);

	virtual Address writeBack(Address addr);
private:
	// my morals stop me from using a struct def here
	Address makeMask(int start, int length) const;
	
	Address getVictimLRU(Address set) const;

	void updateLRU(int set, int way);
	void updateDirty(int set, int way);

	struct DecodedAddress;
	DecodedAddress decodeAddress(Address addr) const;

	void updateReadStats(bool hit);
	void updateWriteStats(bool hit);
	void updateWriteBackStats(bool hit);
private:
	struct BitMasks {
		Address tagBits_;
		Address setBits_;
		Address offsetBits_;

		Address lruBits_;
		Address validBits_;
		Address dirtyBits_;
	};

	struct DecodedAddress {
		Address tag;
		Address set;
	};

	BitMasks bitMasks_;

	CacheParams params_;
	CacheStats stats_;

	// address and metadata storage
	std::vector<Address> cache_;
	std::vector<uint32_t> extraBits_; 
};
#endif // LevelCache_H