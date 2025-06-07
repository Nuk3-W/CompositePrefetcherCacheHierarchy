cmake_minimum_required(VERSION 3.10)
project(CacheSim)

set(CMAKE_CXX_STANDARD 17)

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_executable(CacheSim ${SOURCES} "include/Caches/LevelCache.h" "include/Caches/VictimCache.h" "include/Prefetcher/ControlUnit.h" "include/Prefetcher/GHB.h" "include/CacheManager.h" "src/CacheManager.cpp" "include/Caches/CacheData.h" "include/Prefetcher/PrefetcherData.h" "src/ConfigParser.cpp")

file(COPY ${PROJECT_SOURCE_DIR}/tests DESTINATION ${CMAKE_BINARY_DIR})