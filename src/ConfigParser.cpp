#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cmath>

#include "SystemCacheData.h"


void buildMaps(std::unordered_map<std::string, unsigned long>& sizeMap,
               std::unordered_map<std::string, unsigned long>& assocMap,
               std::unordered_map<std::string, unsigned long>& victimMap,
               SystemCacheParams& params) {

    std::ifstream file("config.txt");

    std::string line;

    if ( !file.is_open() ) {
        std::cerr << "Error opening config file.\n";
        return;
	}

    while ( std::getline(file, line) ) {
		std::istringstream iss(line);
        std::string key;
        iss >> key;

        if ( key == "block_size" ) {
            iss >> params.blockSize_;
        } else if ( key == "trace_file" ) {
            iss >> params.traceFile_;
        } else if ( key.find("_blocks") != std::string::npos ) {
            std::string level = key.substr(0, key.find("_"));
            unsigned long val;
            iss >> val;
            victimMap[level] = val;
        } else if ( key.find("_size") != std::string::npos ) {
            std::string level = key.substr(0, key.find("_"));
            unsigned long val;
            iss >> val;
            sizeMap[level] = val;
        } else if ( key.find("_assoc") != std::string::npos ) {
            std::string level = key.substr(0, key.find("_"));
            unsigned long val;
            iss >> val;
            assocMap[level] = val;
        }
    }
}

void buildCacheParams(const std::unordered_map<std::string, unsigned long>& sizeMap,
                      const std::unordered_map<std::string, unsigned long>& assocMap,
                      const std::unordered_map<std::string, unsigned long>& victimMap,
                      SystemCacheParams& sysParams) {

    for ( const auto& [level, size] : sizeMap ) {
        if ( assocMap.find(level) == assocMap.end() ) {
            std::cerr << "Missing assoc for " << level << "\n";
            continue;
        }

        unsigned long assoc = assocMap.at(level);
        unsigned long sets = size / ( sysParams.blockSize_ * assoc );

        CacheParams cache;

        cache.blockSize_ = sysParams.blockSize_;
        cache.size_ = size;
        cache.assoc_ = assoc;
        cache.sets_ = sets;

        sysParams.caches_.push_back(cache);
    }

    for ( const auto& [level, numOfBlocks] : victimMap ) {
        unsigned long assoc = numOfBlocks;
		unsigned long size = numOfBlocks * sysParams.blockSize_;
        unsigned long sets = size / ( sysParams.blockSize_ * assoc );

        CacheParams vcache;

        vcache.blockSize_ = sysParams.blockSize_;
        vcache.size_ = size;
        vcache.assoc_ = assoc;
        vcache.sets_ = sets;
        sysParams.vCaches_.push_back(vcache);
	}
}

void loadConfigFromFile(SystemCacheParams& params) {
    std::unordered_map<std::string, unsigned long> sizeMap;
    std::unordered_map<std::string, unsigned long> assocMap;
    std::unordered_map<std::string, unsigned long> victimMap;

	//uses out parameters for maps
	buildMaps(sizeMap, assocMap, victimMap, params);

    // Step 2: Build CacheParams for L1, L2...
	buildCacheParams(sizeMap, assocMap, victimMap, params);

    // Debug print
    std::cout << "Caches:\n";
    for ( const auto& c : params.caches_ ) {
        std::cout << "Size: " << c.size_ << ", Assoc: " << c.assoc_ << ", Sets: " << c.sets_
            << "\n";
    }

    std::cout << "Victim Caches:\n";
    for ( const auto& v : params.vCaches_ ) {
        std::cout << "Blocks: " << v.assoc_ << ", Size: " << v.size_ << "\n";
    }
}