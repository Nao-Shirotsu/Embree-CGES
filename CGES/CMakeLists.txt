cmake_minimum_required(VERSION 3.1)

project(cges CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG "-D_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/FindGLFW_Test/cmake)

find_package(GLFW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

include_directories(
    ${OPENGL_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/embree3/include
)

link_directories(
    ${OPENGL_LIBRARIES}
    ${CMAKE_CURRENT_SOURCE_DIR}/embree3/lib
)

file(GLOB srcfiles ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_executable(${PROJECT_NAME}
    ${srcfiles}
)

target_link_libraries(${PROJECT_NAME}
    embree3
    tbb
    tbbmalloc
    ${GLFW_LIBRARIES}
    ${OPENGL_LIBRARY}
)