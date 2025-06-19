#ifndef LevelCache_H
#define LevelCache_H

#include <vector>
#include <cmath>
#include <iostream>

#include "CacheData.h"

class LevelCache {
public:
	LevelCache(const CacheParams& params);

	virtual Address read(Address addr); 
	virtual Address write(Address addr);
private:
	struct DecodedAddress;
	int getVictimLRU(Address set) const;

	int findInSet(const DecodedAddress& dAddr) const;

	bool isValidBlock(int cacheIndex) const;
	void setValid(int set, int way);

	bool isDirtyBlock(int cacheIndex) const;
	void setDirty(int set, int way);
	void clearDirty(int set, int way);

	Address handleCacheEviction(const DecodedAddress& dAddr, int victimIndex, Address newAddr);

	void updateLRU(int set, int way);


	DecodedAddress decodeAddress(Address addr) const;

	void updateReadStats(bool hit);
	void updateWriteStats(bool hit);
	void updateWriteBackStats(bool hit);

	Address makeMask(int start, int length) const;
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