cmake_minimum_required(VERSION 3.17)
project(o2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_definitions(-DPLATFORM_LINUX)

set(BOX2D_BUILD_STATIC ON)
set(BOX2D_VERSION 2.3.0)
add_subdirectory(Framework/3rdPartyLibs/Box2D)

add_subdirectory(Framework/Platforms/Linux)
add_subdirectory(Editor/Platforms/Linux)