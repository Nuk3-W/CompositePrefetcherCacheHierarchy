#ifndef VICTIM_CACHE_H
#define VICTIM_CACHE_H

class VictimCache : public BaseCache {
public:
	VictimCache(const CacheParams& params);
	~VictimCache() = default;

	Address read(Address addr);
	Address write(Address addr);
	void printStats() const;
};

#endif // VICTIM_CACHE_H