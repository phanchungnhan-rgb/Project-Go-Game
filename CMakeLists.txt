
cmake_minimum_required(VERSION 3.23)

project(NoGoNoLife)

add_executable(NoGoNoLife)

add_library(DebugUtil
    debug/log.hpp
    debug/log.cpp
) 

add_library(Engine
    engine/engine.hpp
    engine/engine.cpp
) 

target_sources(NoGoNoLife
  PRIVATE
    controller/main.hpp
    controller/main.cpp
)

target_link_libraries(NoGoNoLife
  PRIVATE
    DebugUtil
    Engine
)

target_include_directories(NoGoNoLife
        PUBLIC ./
)