#=============================================================================
#
# CMake configuration file for the hermes project
#
#=============================================================================


cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

project(hermes LANGUAGES CXX)


# Prevent in-source build
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message( FATAL_ERROR  "In-source build is not possible and not recommended. Choose an empty directory for build output.")
endif(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})

# Languages requirements
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)


# Version informations
set(hermes_VERSION_MAJOR   "0")
set(hermes_VERSION_MINOR   "1")
set(hermes_VERSION_PATCH   "0")
set(hermes_VERSION
  "${hermes_VERSION_MAJOR}.${hermes_VERSION_MINOR}.${hermes_VERSION_PATCH}")

# Cmake scripts
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Hermes options
option(HERMES_BUILD_TESTS "Tests building." ON)
option(BUILD_SHARED_LIBS "" ON)


# hermes depends on fmt library
include(Add_fmt)
include(Add_Eigen3)


# For Windows (exposing public API)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Creating the library
add_library(hermes)
add_subdirectory(src)

target_link_libraries(hermes fmt Eigen3::Eigen)


# Adding tests
if (HERMES_BUILD_TESTS)
    add_subdirectory(tests)
endif()
