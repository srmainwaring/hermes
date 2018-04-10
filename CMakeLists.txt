
# stackoverflow.com/questions/17511496/how-to-create-a-shared-library-with-cmake


cmake_minimum_required(VERSION 3.8)
project(hermes)

set(CMAKE_CXX_STANDARD 14)

# Guard against in-source builds
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt. ")
endif()

# VERSION INFORMATIONS
# TODO: aller chercher dans un config.h !
set(HERMES_MAJOR_VERSION 0)
set(HERMES_MINOR_VERSION 1)
set(HERMES_PATCH_VERSION 0)
set(HERMES_VERSION ${HERMES_MAJOR_VERSION}.${HERMES_MINOR_VERSION}.${HERMES_PATCH_VERSION})


#add_subdirectory(fmt)  # TODO: voir si on fait cet import...

add_subdirectory(src)

list(APPEND HERMES_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/fmt)

message(STATUS ------->${CMAKE_CURRENT_SOURCE_DIR})
set(FMT_LIB ${CMAKE_CURRENT_SOURCE_DIR}/fmt/build/fmt/libfmt.so)

#list(APPEND HERMES_THIRDPARTY_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/build/libhermes.so)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/fmt)

add_library(hermes SHARED ${HERMES_SRC_FILES})
target_link_libraries(hermes ${FMT_LIB})


#list(APPEND HERMES_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/fmt/fmt)
list(APPEND HERMES_THIRDPARTY_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/build/libhermes.so)  # TODO: ajouter fmt !!!

#set_target_properties(hermes PROPERTIES PUBLIC_HEADER hermes.h)


add_executable(test_hermes main.cpp ${HERMES_SRC_FILES})
target_link_libraries(test_hermes ${FMT_LIB})

list(APPEND HERMES_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/build/libhermes.so ${FMT_LIB})


# Generating configuration files to make MathUtils easily usable in other projects
include (CMakePackageConfigHelpers)
configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/HermesConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/HermesConfig.cmake
        PATH_VARS HERMES_INCLUDE_DIRS HERMES_LIBS# Here we expose these variables as replacement into the MathUtilsConfig.cmake.in
        INSTALL_DESTINATION ${CMAKE_BINARY_DIR}
        #    NO_CHECK_REQUIRED_COMPONENTS_MACRO # Eigen does not provide components
)

# Generating the version file
write_basic_package_version_file(
        HermesConfigVersion.cmake
        VERSION ${HERMES_VERSION}
        COMPATIBILITY SameMajorVersion
)