#include <iostream>
#include "SystemCacheData.h"

// Forward declaration of the function you want to test
void loadConfigFromFile(SystemCacheParams& params);

int main() {
    SystemCacheParams params{};
    loadConfigFromFile(params);

    // Do your assertions or printing here

    return 0;
}