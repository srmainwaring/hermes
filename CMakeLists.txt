
cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

project(hermes LANGUAGES CXX)


# Cmake scripts
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)


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


# hermes depends on fmt library
include(Add_fmt)

# Creating hermes target
set(hermes_sources
        src/hermes/Messaging.cpp
        )

set(hermes_headers
        src/hermes/Messaging.h
        src/hermes/hermes.h
        )


add_library(hermes SHARED ${hermes_sources})
add_library(hermes::hermes ALIAS hermes)

#find_package(fmt REQUIRED)

target_link_libraries(hermes fmt::fmt) # Buggy ?

target_include_directories(hermes INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>)

set_target_properties(hermes PROPERTIES
        VERSION ${hermes_VERSION})


include(GNUInstallDirs)
#message(${CMAKE_INSTALL_LIBDIR})
install(TARGETS hermes EXPORT fmt-targets
        DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(FILES ${hermes_headers}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/hermes)

