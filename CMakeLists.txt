cmake_minimum_required(VERSION 3.25.1)

project(opengl-something)
SET(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SRC src/*.cpp)

add_executable(main ${SRC})

target_link_libraries(main glut glfw GL GLEW)

add_definitions(-DUSE_TEXTURE)