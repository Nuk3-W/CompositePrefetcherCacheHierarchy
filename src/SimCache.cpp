#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <limits>

#include "Config/Params.h"
#include "Core/MemoryController.h"
#include "Core/StatisticsManager.h"

// Using directives for cleaner code
using Config::SystemParams;

void loadConfigFromFile(SystemParams& params);

int main(int argc, char* argv[]) {
    using std::cerr;
    using std::cout;
    using std::endl;
    using std::string;

    //trace file contained within params
    SystemParams params{};

	loadConfigFromFile(params);

	MemoryController memoryController(params);

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
			memoryController.read(addr);
        }
        else if (op == 'w') {
            memoryController.write(addr);
        }
        else {
            cerr << "Unknown operation: " << op << endl;
        }
    }

    trace.close();
    StatisticsManager::getInstance().printDetailedStats();
    return 0;
}