cmake_minimum_required(VERSION 3.17)
project(Editor)

file(GLOB_RECURSE Editor_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/../../Sources/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../Sources/*.h
)

add_executable(Editor
    ${Editor_SOURCES}
)

set(EditorRoot ${CMAKE_CURRENT_LIST_DIR}/../..)

target_include_directories(Editor
    PUBLIC
        $<BUILD_INTERFACE:${EditorRoot}/Sources>
        $<BUILD_INTERFACE:${EditorRoot}>
)

target_link_libraries(Editor
    PUBLIC
        Framework
)
