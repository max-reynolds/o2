cmake_minimum_required(VERSION 3.17)
project(Framework)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PNG REQUIRED)

add_definitions(-DPLATFORM_LINUX)

file(GLOB_RECURSE Framework_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/../../Sources/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../Sources/*.h
)

add_library(Framework STATIC
    ${Framework_SOURCES}
)

set(FrameworkRoot ${CMAKE_CURRENT_LIST_DIR}/../..)

target_include_directories(Framework
    PUBLIC
        $<BUILD_INTERFACE:${FrameworkRoot}/Sources>
        $<BUILD_INTERFACE:${FrameworkRoot}/3rdPartyLibs>
        $<BUILD_INTERFACE:${FrameworkRoot}/3rdPartyLibs/rapidjson/include>
        $<BUILD_INTERFACE:${FrameworkRoot}/3rdPartyLibs/FreeType/include>
        $<BUILD_INTERFACE:${FrameworkRoot}>
)

target_link_libraries(Framework
    PUBLIC
        libpng
)
