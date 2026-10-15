
cmake_minimum_required(VERSION 3.23)

project(NoGoNoLife)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")


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

target_include_directories(Engine
  PUBLIC ./
)