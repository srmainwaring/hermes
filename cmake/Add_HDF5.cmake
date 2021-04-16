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
    set(HDF5_BUILD_CPP_LIB ON CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE) # TODO: essayer et voir comment ca impacte...
    set(HDF5_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(HDF5_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
    set(HDF5_BUILD_HL_LIB OFF CACHE BOOL "" FORCE)
    set(HDF5_GENERATE_HEADERS OFF CACHE BOOL "" FORCE)
    set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
    #    set(HDF5_ENABLE_Z_LIB_SUPPORT OFF CACHE BOOL "" FORCE)
    #    set(HDF5_ENABLE_SZIP_SUPPORT OFF CACHE BOOL "" FORCE)
    #    set(HDF5_ENABLE_SZIP_ENCODING OFF CACHE BOOL "" FORCE)


    add_subdirectory(${hdf5_SOURCE_DIR} ${hdf5_BINARY_DIR})
endif ()


if (TARGET hdf5-static)
    get_target_property(INC hdf5-static INCLUDE_DIRECTORIES)
    get_target_property(HDF5_C_LIBRARIES hdf5-static INCLUDE_DIRECTORIES)
    set(HDF5_C_LIBRARIES ${HDF5_C_LIBRARIES})
else ()
    message(STATUS "hdf5-static target NOT FOUND")
endif ()

if (TARGET hdf5_cpp-static)
    get_target_property(INC hdf5_cpp-static INCLUDE_DIRECTORIES)
    get_target_property(HDF5_CXX_LIBRARIES hdf5_cpp-static INCLUDE_DIRECTORIES)
    #    target_include_directories(HDF5 PUBLIC ${INC})
    set(HDF5_CXX_LIBRARIES ${HDF5_CXX_LIBRARIES})
else ()
    message(STATUS "hdf5-static target NOT FOUND")
endif ()

include_directories(${hdf5_BINARY_DIR}) # See why this directory is not included into the target...

unset(CMAKE_ARCHIVE_OUTPUT_DIRECTORY CACHE)
unset(CMAKE_LIBRARY_OUTPUT_DIRECTORY CACHE)
unset(CMAKE_RUNTIME_OUTPUT_DIRECTORY CACHE)
