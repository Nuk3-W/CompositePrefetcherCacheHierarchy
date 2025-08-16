#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <limits>

#include "Config/Params.h"
#include "Core/MemoryController.h"
#include "Core/StatisticsManager.h"

using Config::SystemParams;

constexpr char READ_OPERATION = 'r';
constexpr char WRITE_OPERATION = 'w';

void loadConfigFromFile(SystemParams& params);

int main(int argc, char* argv[]) {
    using std::cerr;
    using std::cout;
    using std::endl;
    using std::string;

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
        std::cout << "Reading: " << std::hex << addr << std::dec << std::endl;
        if (op == READ_OPERATION) {
			memoryController.read(addr);
        }
        else if (op == WRITE_OPERATION) {
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