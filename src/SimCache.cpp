#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <limits>

#include "SystemCacheData.h"
#include "CacheManager.h"

void loadConfigFromFile(std::string_view filename, std::string_view traceFile, SystemCacheParams& params);

int main(int argc, char* argv[]) {
    using std::cerr;
    using std::cout;
    using std::endl;
    using std::string;

    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <config_file>\n";
        return EXIT_FAILURE;
    }

    SystemCacheParams params{};
    string trace_file;

    try { //fix params input
        params.l1_.blockSize_ = params.l2_.blockSize_ = params.l3_.blockSize_ = params.vCache_.blockSize_ = std::stoul(argv[1]); //best as a global but Id rather it all be encasulated in the class
        params.l1_size = std::stoul(argv[2]);
        params.l1_assoc = std::stoul(argv[3]);
        params.vc_num_blocks = std::stoul(argv[4]);
        params.l2_size = std::stoul(argv[5]);
        params.l2_assoc = std::stoul(argv[6]);
    }
    catch (const std::exception& e) {
        cerr << "Invalid numeric input: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    string trace_file = argv[7];
    
    // Print parameters
    cout << "  ===== Simulator configuration =====\n"
        << "  L1_BLOCKSIZE:                     " << params.block_size << "\n"
        << "  L1_SIZE:                          " << params.l1_size << "\n"
        << "  L1_ASSOC:                         " << params.l1_assoc << "\n"
        << "  VC_NUM_BLOCKS:                    " << params.vc_num_blocks << "\n"
        << "  L2_SIZE:                          " << params.l2_size << "\n"
        << "  L2_ASSOC:                         " << params.l2_assoc << "\n"
        << "  trace_file:                       " << trace_file << "\n"
        << "  ===================================\n\n";

	CacheManager cacheManager(params);

    std::ifstream trace(trace_file);
    if (!trace.is_open()) {
        cerr << "Error: Could not open file " << trace_file << endl;
        return EXIT_FAILURE;
    }

    char op;
    Address addr;

    while (trace >> op >> std::hex >> addr) {
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

    trace.close();
    return 0;
}