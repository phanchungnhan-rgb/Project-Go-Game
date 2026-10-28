
cmake_minimum_required(VERSION 3.23)

project(NoGoNoLife)

# find_package(SFML 3.1 CONFIG REQUIRED COMPONENTS Graphics Window System)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_subdirectory(vendored/SFML EXCLUDE_FROM_ALL)

add_executable(NoGoNoLife)

add_library(DebugUtil
    debug/log.hpp
    debug/log.cpp
) 

add_library(Engine
    engine/engine.hpp
    engine/engine.cpp
    engine/AI.hpp
    engine/AI.cpp
) 

target_sources(NoGoNoLife
  PRIVATE
    controller/main.hpp
    controller/main.cpp
    ui/game_window.hpp
    ui/game_window.cpp
)

target_link_libraries(NoGoNoLife
  PRIVATE
    DebugUtil
    Engine
    SFML::Graphics
    SFML::Window
    SFML::System
)

# add_custom_command(TARGET NoGoNoLife POST_BUILD
#   COMMAND ${CMAKE_COMMAND} -E copy_if_different
#     $<TARGET_RUNTIME_DLLS:NoGoNoLife>
#     $<TARGET_FILE_DIR:NoGoNoLife>
#   COMMAND_EXPAND_LISTS
# )

target_include_directories(NoGoNoLife
  PUBLIC ./
)

target_include_directories(Engine
  PUBLIC ./
)