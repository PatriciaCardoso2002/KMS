cmake_minimum_required(VERSION 3.16)
project(etsi004)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Set the build type (Release or Debug)
set(CMAKE_BUILD_TYPE Release)
# Uncomment the line below for Debug build
# set(CMAKE_BUILD_TYPE Debug)

# Set compiler flags for Release build
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Set compiler flags for Debug build
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -fsanitize=address")

# Set the output directory for the binary
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Add executable for etsi_client
add_executable(etsi_client etsi_client.cpp etsi_qkd_004.cpp)

# Add executable for etsi_server
add_executable(etsi_server etsi_server.cpp etsi_qkd_004.cpp)
