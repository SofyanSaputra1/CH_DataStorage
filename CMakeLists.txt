cmake_minimum_required(VERSION 3.9)
project(collective-health-datastorage-contract)

set(CMAKE_CXX_STANDARD 11)

add_executable(datastorage
        datastorage.cpp datastorage.cpp)

include_directories(/Users/johnilquezada/Projects/eos)
include_directories(/Users/johnilquezada/Projects/eos/contracts)
