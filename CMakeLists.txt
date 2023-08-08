cmake_minimum_required(VERSION 3.11)

project(Final_Project)

include(ExternalProject)

# Set install directory
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/dist CACHE PATH ${CMAKE_SOURCE_DIR}/dist FORCE)
set(CMAKE_BUILD_TYPE "Debug")
if (MSVC OR XCODE)
    set(CMAKE_CONFIGURATION_TYPES "Debug" CACHE STRING "Debug" FORCE)
endif ()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(OpenGL REQUIRED COMPONENTS OpenGL)

include(BuildGLEW)
include(BuildGLFW)
include(BuildGLM)

# project
set(EXEC main)

set(ASSETS assets)

file(GLOB SRC src/*.cpp)

add_executable(${EXEC} ${SRC})

target_include_directories(${EXEC} PRIVATE include)

# Include irrKlang headers and libraries
if (APPLE)
    set(IRRKLANG_LIBRARY "../lib/irrKlang/MacOS/libirrklang.dylib")
endif ()

if (WIN32)
    set(IRRKLANG_LIBRARY "../lib/irrKlang/Windows/irrKlang.lib")
endif ()

if (UNIX AND NOT APPLE)
    set(IRRKLANG_LIBRARY "../lib/irrKlang/Linux/libirrKlang.so")
endif ()

include_directories("../include/irrKlang")

add_library(irrKlang STATIC IMPORTED)
set_target_properties(irrKlang PROPERTIES IMPORTED_LOCATION ${IRRKLANG_LIBRARY})

# Link libraries for the "main" target
target_link_libraries(${EXEC}
        OpenGL::GL glew_s glfw glm irrKlang
)

list(APPEND BIN ${EXEC})
# end project

# install files to install location
install(TARGETS ${BIN} DESTINATION ${CMAKE_INSTALL_PREFIX})
install(DIRECTORY ${ASSETS} DESTINATION ${CMAKE_INSTALL_PREFIX})
