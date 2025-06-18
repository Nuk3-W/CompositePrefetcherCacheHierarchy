#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <limits>

#include "SystemCacheData.h"
#include "CacheManager.h"

void loadConfigFromFile(SystemCacheParams& params);

int main(int argc, char* argv[]) {
    using std::cerr;
    using std::cout;
    using std::endl;
    using std::string;

    //trace file contained within params
    SystemCacheParams params{};

	loadConfigFromFile(params);

    //std::cout << "debug check 1" << std::endl;

	CacheManager cacheManager(params);

    //std::cout << "debug check 2" << std::endl;

    std::ifstream trace(params.traceFile_);
    if (!trace.is_open()) {
        cerr << "Error: Could not open file " << params.traceFile_ << endl;
        return EXIT_FAILURE;
    }

    //std::cout << "debug check 3" << std::endl;

    char op;
    Address addr;

    while (trace >> op >> std::hex >> addr) {
        if (op == 'r') {
			cacheManager.read(addr);
			std::cout << "Read from address: " << std::hex << addr << std::dec << endl;
        }
        else if (op == 'w') {
            cacheManager.write(addr);
            std::cout << "Write from address: " << std::hex << addr << std::dec << endl;
        }
        else {
            cerr << "Unknown operation: " << op << endl;
        }
    }

    trace.close();
    return 0;
}