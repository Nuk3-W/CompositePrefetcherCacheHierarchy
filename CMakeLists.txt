cmake_minimum_required(VERSION 3.10)
project(CacheSim)

set(CMAKE_CXX_STANDARD 17)


file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/*.cpp)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(CacheSim ${SRC_FILES})

add_executable(test_config
    tests/test_config.cpp
    src/ConfigParser.cpp
)

add_custom_command(TARGET test_config POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${PROJECT_SOURCE_DIR}/config.txt
    $<TARGET_FILE_DIR:test_config>
)