if (NOT BUILD_SHARED_LIBS)
    message(STATUS "Using static libraries for HDF5")
    set(HDF5_USE_STATIC_LIBRARIES ON CACHE BOOL "Use static library for HDF5")
else ()
    message(STATUS "Using shared libraries for HDF5")
endif ()

message(STATUS "******* FINDING HDF5 dependency from ${PROJECT_NAME} (requested minimum version: ${HDF5_FIND_TAG}) *******")
find_package(HDF5 ${HDF5_FIND_TAG} COMPONENTS CXX)

if (HDF5_FOUND)
    message(STATUS "HDF5 ${HDF5_VERSION} found here: ${HDF5_INCLUDE_DIRS}")

    if (BUILD_SHARED_LIBS)
        message(STATUS "Creating target hdf5_cpp-shared based on ${HDF5_LIBRARIES}")
        add_library(hdf5_cpp-shared INTERFACE IMPORTED)
        target_include_directories(hdf5_cpp-shared INTERFACE ${HDF5_CXX_INCLUDE_DIRS})
        target_link_libraries(hdf5_cpp-shared INTERFACE ${HDF5_LIBRARIES})
    else ()
        message(STATUS "Creating target hdf5_cpp-static based on ${HDF5_LIBRARIES}")
        add_library(hdf5_cpp-static INTERFACE IMPORTED)
        target_include_directories(hdf5_cpp-static INTERFACE ${HDF5_CXX_INCLUDE_DIRS})
        target_link_libraries(hdf5_cpp-static INTERFACE ${HDF5_LIBRARIES})
    endif ()

else ()
    message(STATUS "HDF5 not FOUND on the system, switching to a build from scratch")

    include(FetchContent)

    FetchContent_Declare(HDF5
            GIT_REPOSITORY ${HDF5_URL}
            GIT_TAG ${HDF5_TAG}
            PATCH_COMMAND patch < "${PROJECT_SOURCE_DIR}/cmake/patches/${HDF5_PATCH}"
            )

    FetchContent_GetProperties(HDF5)

    if (NOT hdf5_POPULATED)
        message(STATUS "******* FETCHING HDF5 dependency from ${PROJECT_NAME} (requested version: ${HDF5_TAG}) *******")
        FetchContent_Populate(HDF5)

        # HDF5 BUILD OPTIONS
        set(HDF5_BUILD_CPP_LIB ON CACHE BOOL "")
        set(HDF5_BUILD_EXAMPLES OFF CACHE BOOL "")
        set(HDF5_BUILD_TOOLS OFF CACHE BOOL "")
        set(HDF5_BUILD_HL_LIB OFF CACHE BOOL "")
        set(HDF5_GENERATE_HEADERS OFF CACHE BOOL "")
        set(BUILD_TESTING OFF CACHE BOOL "")

        add_subdirectory(${hdf5_SOURCE_DIR} ${hdf5_BINARY_DIR})
    endif ()

    unset(CMAKE_ARCHIVE_OUTPUT_DIRECTORY CACHE)
    unset(CMAKE_LIBRARY_OUTPUT_DIRECTORY CACHE)
    unset(CMAKE_RUNTIME_OUTPUT_DIRECTORY CACHE)

endif ()