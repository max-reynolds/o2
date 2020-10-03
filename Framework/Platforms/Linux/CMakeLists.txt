cmake_minimum_required(VERSION 3.17)
project(Framework)

find_package(PNG REQUIRED)

file(GLOB_RECURSE Framework_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/../../Sources/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../Sources/*.h
)

add_library(Framework SHARED
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
        png
)
