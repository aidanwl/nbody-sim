cmake_minimum_required(VERSION 3.20)
project(NBody C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(FetchContent)

# Pick a released version instead of tracking master
set(RAYLIB_VERSION 6.0)

# Download thing to make it easier, still have to install stuff
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
)

# Remove Bloat
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(raylib)

add_executable(nbody
    src/main.c
    src/core/vec2.c
    src/core/gravity.c
    src/core/simulation.c
    src/core/renderer.c
    src/core/widget.c
    src/app/app.c
    src/core/body_creator.c
)

target_include_directories(nbody PRIVATE include)

target_link_libraries(nbody PRIVATE raylib)