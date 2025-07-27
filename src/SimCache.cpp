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

	CacheManager cacheManager(params);


    std::ifstream trace(params.traceFile_);
    if (!trace.is_open()) {
        cerr << "Error: Could not open file " << params.traceFile_ << endl;
        return EXIT_FAILURE;
    }

    char op;
    Address addr;

    while (trace >> op >> std::hex >> addr) {
        //std::cout << "Processing address: " << std::hex << addr << std::dec << std::endl;
        if (op == 'r') {
			cacheManager.read(addr);
        }
        else if (op == 'w') {
            cacheManager.write(addr);
        }
        else {
            cerr << "Unknown operation: " << op << endl;
        }
    }

	cacheManager.printStats();

    trace.close();
    return 0;
}