
cmake_minimum_required(VERSION 3.23)

project(NoGoNoLife)

add_executable(NoGoNoLife)

add_library(DebugUtil
    debug/log.hpp
    debug/log.cpp
) 

target_sources(NoGoNoLife
  PRIVATE
    controller/main.hpp
    controller/main.cpp
)

target_link_libraries(NoGoNoLife
  PRIVATE
    DebugUtil
)

target_include_directories(NoGoNoLife
        PUBLIC ./
)