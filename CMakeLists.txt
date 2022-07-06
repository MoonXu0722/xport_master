cmake_minimum_required(VERSION 2.8)

project(xport-root)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall")
#set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_subdirectory(./src)

add_subdirectory(./test)

add_subdirectory(./example)
