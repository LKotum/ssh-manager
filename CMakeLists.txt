cmake_minimum_required(VERSION 3.8.2)
project (sshmgr VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(MAIN_SRC sshmgr.cpp)
set(HOSTSPSR_SRC libs/hostspsr/hostspsr.cpp)

include_directories(libs/hostspsr/include)

add_library(hostspsr STATIC ${HOSTSPSR_SRC})

add_executable(sshmgr ${MAIN_SRC})

target_link_libraries(sshmgr hostspsr)