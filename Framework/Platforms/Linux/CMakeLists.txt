cmake_minimum_required(VERSION 3.17)
project(Framework)

find_package(PNG REQUIRED)
find_package(Freetype REQUIRED)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../3rdPartyLibs/gtest ${CMAKE_CURRENT_BINARY_DIR}/../../3rdPartyLibs/gtest)

set(BOX2D_BUILD_STATIC ON)
set(BOX2D_VERSION 2.3.0)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../3rdPartyLibs/Box2D ${CMAKE_CURRENT_BINARY_DIR}/../../3rdPartyLibs/Box2D)

file(GLOB_RECURSE PUGI_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/../../3rdPartyLibs/pugixml/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../3rdPartyLibs/pugixml/*.h
)

add_library(pugi STATIC
    ${PUGI_SOURCES}
)

set_property(TARGET pugi PROPERTY POSITION_INDEPENDENT_CODE ON)



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
        $<BUILD_INTERFACE:${FrameworkRoot}>
        $<BUILD_INTERFACE:${FREETYPE_INCLUDE_DIRS}>
)

target_link_libraries(Framework
    PUBLIC
        png ${FREETYPE_LIBRARIES}
        Box2D pugi
)

file(GLOB_RECURSE FrameworkTests_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/../../Tests/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../Tests/*.h
)

add_executable(FrameworkTests
    ${FrameworkTests_SOURCES}
)

target_link_libraries(FrameworkTests
    PUBLIC
        Framework
        gtest gtest_main
)
