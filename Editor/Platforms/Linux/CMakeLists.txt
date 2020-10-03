cmake_minimum_required(VERSION 3.17)
project(Editor)

file(GLOB_RECURSE Editor_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/../../Sources/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../Sources/*.h
)

list(REMOVE_ITEM Editor_SOURCES ${CMAKE_CURRENT_LIST_DIR}/../../Sources/o2Editor/EditorMain.cpp)

add_library(EditorLib SHARED
    ${Editor_SOURCES}
)

set(EditorRoot ${CMAKE_CURRENT_LIST_DIR}/../..)

target_include_directories(EditorLib
    PUBLIC
        $<BUILD_INTERFACE:${EditorRoot}/Sources>
        $<BUILD_INTERFACE:${EditorRoot}>
)

target_link_libraries(EditorLib
    PUBLIC
        Framework
)


add_executable(Editor ${CMAKE_CURRENT_LIST_DIR}/../../Sources/o2Editor/EditorMain.cpp)

target_link_libraries(Editor
    PUBLIC
        EditorLib
)
