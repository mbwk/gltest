cmake_minimum_required(VERSION 2.8)

project( CMAKETUT_PROJECT ) # project name, this is case sensitive

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g") # compilation flags

######################################
# finds OpenGL, GLU and X11
find_package(OpenGL REQUIRED) # searches for specified library/package on system
if(NOT OPENGL_FOUND)
    message("ERROR: OpenGL not found")
endif(NOT OPENGL_FOUND)
set(GL_LIBRARY GL GLU X11 GLEW)

file(GLOB SOURCES src/*.cpp) # globs source files every time you run cmake (but not when actually building)
include_directories(include) # adds specified include directory to include path
file(GLOB HEADERS include/*.hpp) # globs include files, similar to previous usage

add_executable(cmaketut ${SOURCES} ${HEADERS}) # adds compilation target: a standalone executable

# linking "glfw" and not "glfw3"
# assumes that glfw was built with BUILD_SHARED_LIBS to ON
target_link_libraries(cmaketut glfw ${GL_LIBRARY} m GLEW) # link to given libraries

