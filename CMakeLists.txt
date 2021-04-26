#=============================================================================
#
# CMake configuration file for the hermes project
#
#=============================================================================


cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

project(hermes LANGUAGES C CXX)


# Prevent in-source build
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message( FATAL_ERROR  "In-source build is not possible and not recommended. Choose an empty directory for build output.")
endif(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})

# Languages requirements
set(CMAKE_CXX_STANDARD 17)
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

#=============================================================================
# Retrieving the current Git revision
#=============================================================================
set(PRE_CONFIGURE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/hermes/version.h.in CACHE FILEPATH "" FORCE)
set(POST_CONFIGURE_FILE ${CMAKE_CURRENT_BINARY_DIR}/src/hermes/version.h CACHE FILEPATH "" FORCE)
set(GIT_WORKING_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE FILEPATH "" FORCE)
set(GIT_STATE_FILE ${CMAKE_CURRENT_BINARY_DIR}/git-state-hash CACHE FILEPATH "" FORCE)
include(cmake/git_watcher.cmake)

if (NOT EXISTS ${POST_CONFIGURE_FILE})
    configure_file(${PRE_CONFIGURE_FILE} ${POST_CONFIGURE_FILE})
endif()

#=============================================================================
# Hermes options
#=============================================================================
cmake_policy(SET CMP0077 NEW)

#=============================================================================
# Hermes options
#=============================================================================
option(HERMES_BUILD_TESTS "Tests building." ON)
option(BUILD_SHARED_LIBS "" ON)

option(BUILD_SHARED_LIBS "Shared lib version" ON)
option(HERMES_USE_HDF5 "Activate HDF5 as serialization tool" ON)
option(HERMES_BUILD_TESTS "Tests building." ON)

#=============================================================================
# Hermes dependencies
#=============================================================================
include(cmake/URL.conf.cmake)
include(cmake/Add_fmt.cmake)
include(cmake/Add_Eigen3.cmake)

if (HERMES_USE_HDF5)
    message(STATUS "hermes can use HDF5 as a serialization target")
    include(cmake/Add_HDF5.cmake)
#    include(cmake/Add_HighFive.cmake)
endif ()

# For Windows (exposing public API)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

#=============================================================================
# Hermes header only library
#=============================================================================
add_library(hermes INTERFACE)
add_dependencies(hermes check_git_hermes) # For git_watcher to fetch git informations before effective build

target_include_directories(hermes INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(hermes INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/src)

target_link_libraries(hermes INTERFACE fmt Eigen3::Eigen)

if (HERMES_USE_HDF5)
    target_compile_definitions(hermes INTERFACE USE_HDF5)
#    target_link_libraries(hermes INTERFACE HighFive)
    if (BUILD_SHARED_LIBS)
        message(STATUS "Using HDF5 SHARED lib into hermes")
        target_link_libraries(hermes INTERFACE hdf5_cpp-shared)
    else ()
        message(STATUS "Using HDF5 STATIC lib into hermes")
        target_link_libraries(hermes INTERFACE hdf5_cpp-static)
        set_property(TARGET hermes PROPERTY POSITION_INDEPENDENT_CODE ON)
    endif ()
endif()


# Adding tests
if (HERMES_BUILD_TESTS)
    add_subdirectory(tests)
endif()
