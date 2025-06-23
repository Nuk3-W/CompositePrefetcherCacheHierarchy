#ifndef BaseCache_H
#define BaseCache_H

#include <vector>
#include <cmath>
#include <iostream>
#include <bitset>
#include <iomanip>

#include "CacheData.h"

class CacheManager;

//abstract base class for all caches (L1, L2, Victim, etc.)
class BaseCache {
public:
	// can be defined because all caches must have these params even if victim
	BaseCache(const CacheParams& params);
	~BaseCache() = default;

	// I dont want virtual functions here to avoid vtable overhead so I leave undefined for linker errors to act as pure virtuals
	// and yes the performance loss isn't much but I decided to be strict about it anyways fight me
	Address read(Address addr);
	Address write(Address addr);
	void printStats() const;
protected:
	struct DecodedAddress;
	DecodedAddress decodeAddress(Address addr) const;

	int getVictimLRU(Address set) const;
	void updateLRU(int set, int way);

	bool isValidBlock(int cacheIndex) const;
	void setValid(int set, int way);

	bool isDirtyBlock(int cacheIndex) const;
	void setDirty(int set, int way);
	void clearDirty(int set, int way);

	Address handleCacheEviction(const DecodedAddress& dAddr, int victimIndex, Address newAddr);
	int findInSet(const DecodedAddress& dAddr) const;

	void updateReadStats(bool hit);
	void updateWriteStats(bool hit);
	void checkHit(bool hit);

	Address makeMask(int start, int length) const;
	void printMask(const std::string& label, int start, int length, unsigned long mask) const;
protected:
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

	BitMasks bitMasks_{};

	CacheParams params_{};
	CacheStats stats_{};

	// address and metadata storage
	std::vector<Address> cache_{};
	std::vector<uint32_t> extraBits_{};
};
#endif // BaseCache_H