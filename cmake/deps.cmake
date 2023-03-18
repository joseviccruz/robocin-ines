########################################################################################################################

# Set CMAKE_CXX_STANDARD to 20 to use C++20 by default.

if (NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
endif ()

message("-- Using C++: ${CMAKE_CXX_STANDARD}")

########################################################################################################################

# Set CMAKE_EXPORT_COMPILE_COMMANDS to ON to generate a compile_commands.json file in the build directory by default
# This file is used by clang tools such as clangd, clang-tidy and clang-format.

if (NOT CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif ()

########################################################################################################################

# Find Protobuf, absl, gRPC from installed gRPC package
# It enable the following variables:
#   protobuf::libprotobuf                 the protobuf library
#   gRPC::grpc++_reflection               the gRPC++ reflection library
#   $<TARGET_FILE:protobuf::protoc>       the protobuf compiler
#   gRPC::grpc++                          the gRPC++ library
#   $<TARGET_FILE:gRPC::grpc_cpp_plugin>  the gRPC++ plugin executable
#   absl:: ...                            the abseil libraries (https://abseil.io/docs/cpp/guides)
# reference: https://github.com/grpc/grpc/blob/master/examples/cpp/cmake/common.cmake

# Find Threads
find_package(Threads REQUIRED)

# Find Protobuf installation
# Looks for protobuf cmake config files installed by Protobuf's cmake installation.
find_package(Protobuf CONFIG REQUIRED HINTS "/opt/grpc")
message(STATUS "Using Protobuf: ${Protobuf_VERSION}")

# Find gRPC installation
# Looks for gRPC cmake config files installed by gRPC's cmake installation.
find_package(absl CONFIG REQUIRED HINTS "/opt/grpc")
message(STATUS "Using absl: ${absl_VERSION}")

find_package(gRPC CONFIG REQUIRED HINTS "/opt/grpc")
message(STATUS "Using gRPC: ${gRPC_VERSION}")

########################################################################################################################

# Find Google Test package
# It enable the following variables:
#   GTest::gtest                          the gtest library
#   GTest::gtest_main                     the gtest_main library, which is used to link against the main function
#   GTest::gmock                          the gmock library
#   GTest::gmock_main                     the gmock_main library, which is used to link against the main function

# Find GTest installation
# Looks for GTest cmake config files installed by GTest's cmake installation.
find_package(GTest CONFIG REQUIRED HINTS "/opt/googletest")
message(STATUS "Using GTest: ${GTest_VERSION}")

include(GoogleTest) # Provided by CMake

enable_testing() # Enable testing for the entire project

########################################################################################################################

# Find Google Benchmark package
# It enable the following variables:
#   benchmark::benchmark                  the benchmark library
#   benchmark::benchmark_main             the benchmark_main library, which is used to link against the main function

# Find Google Benchmark installation
# Looks for Google Benchmark cmake config files installed by Google Benchmark's cmake installation.
find_package(benchmark CONFIG REQUIRED HINTS "/opt/benchmark")
message(STATUS "Using Google Benchmark: ${benchmark_VERSION}")

########################################################################################################################
