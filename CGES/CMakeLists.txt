cmake_minimum_required(VERSION 3.9)

project(CGES CXX)
find_package(PkgConfig REQUIRED)

pkg_search_module(GLFW REQUIRED glfw3)

find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/
)

add_executable(cges
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gl_engine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/render_buffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scene.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/util_objfile.cpp
)

target_link_libraries(cges
    ${OPENGL_LIBRARY}
    ${GLFW_LIBRARY}
    Threads::Threads
    embree3
    tbb
    tbbmalloc
)

include_directories(
    ${OPENGL_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)