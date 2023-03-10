cmake_minimum_required(VERSION 3.16) # Default version of CMake on Ubuntu 20.04 LTS

########################################################################################################################

project(ines LANGUAGES CXX)

########################################################################################################################

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake) # Add our custom cmake modules to the module path
include(cmake/deps.cmake)  # Load our dependencies
include(cmake/rules.cmake) # Load our custom cmake modules

########################################################################################################################

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ROBOCIN_PROJECT_PATH}/bin) # set the output directory for all executables

########################################################################################################################

add_subdirectory(ines) # add the source directory to the build

########################################################################################################################
