#include <fstream>
#include <sstream>
#include <unordered_map>
#include "SystemCacheData.h"

bool loadConfigFromFile(const std::string& filename, SystemCacheParams& params, std::string& traceFile) {
    std::ifstream config(filename);
    if (!config.is_open()) {
        return false;
    }

    std::string line;
    std::unordered_map<std::string, std::string> kv;
    while (std::getline(config, line)) {
        std::istringstream iss(line);
        std::string key, eq, value;
        if (iss >> key >> eq >> value && eq == "=") {
            kv[key] = value;
        }
    }

    try {
        unsigned long blockSize = std::stoul(kv.at("block_size"));
        params.l1_.blockSize_ = params.l2_.blockSize_ = params.l3_.blockSize_ = params.vCache_.blockSize_ = blockSize;

        params.l1_size = std::stoul(kv.at("l1_size"));
        params.l1_assoc = std::stoul(kv.at("l1_assoc"));
        params.vc_num_blocks = std::stoul(kv.at("vc_num_blocks"));
        params.l2_size = std::stoul(kv.at("l2_size"));
        params.l2_assoc = std::stoul(kv.at("l2_assoc"));
        traceFile = kv.at("trace_file");
    }
    catch (...) {
        return false;
    }

    return true;
}