cmake_minimum_required(VERSION 3.10)
project(CacheSim)
set(CMAKE_CXX_STANDARD 17)

include_directories(${PROJECT_SOURCE_DIR}/include)

# Main CacheSim executable (excluding testCheck.cpp)
file(GLOB_RECURSE MAIN_SRC_FILES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/*.cpp)
list(REMOVE_ITEM MAIN_SRC_FILES ${PROJECT_SOURCE_DIR}/src/testCheck.cpp)

add_executable(CacheSim ${MAIN_SRC_FILES})

# Copy config.txt to build directory for CacheSim
add_custom_command(TARGET CacheSim POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${PROJECT_SOURCE_DIR}/config.txt
    $<TARGET_FILE_DIR:CacheSim>/config.txt
)

# Copy tests directory to build directory for CacheSim
add_custom_command(TARGET CacheSim POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/tests
    $<TARGET_FILE_DIR:CacheSim>/tests
)