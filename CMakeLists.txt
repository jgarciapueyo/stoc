cmake_minimum_required(VERSION 3.13)

# Project Name
project(stoc)

set(CMAKE_CXX_STANDARD 17)

# include
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Third-Party Libraries
add_subdirectory(libs/cxxopts)

# Header files
include_directories(include)

# Executable code is here
add_subdirectory(src)

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs support core irreader transformutils bitwriter)

# Link against LLVM libraries
target_link_libraries(stoc ${llvm_libs})
target_link_libraries(stoc cxxopts)
