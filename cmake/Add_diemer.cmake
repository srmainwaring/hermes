
find_package(diemer QUIET)

if (NOT diemer_FOUND)
    include(FetchContent)


    FetchContent_Declare(diemer
            GIT_REPOSITORY ${diemer_URL}
            GIT_TAG ${diemer_TAG}
            )

    FetchContent_GetProperties(diemer)
    if(NOT diemer_POPULATED)
        message(STATUS "Downloading, Configuring and Generating 'diemer' dependency")
        FetchContent_Populate(diemer)

        # diemer BUILD OPTIONS
#        set(VFIT_BUILD_TESTS OFF CACHE BOOL "" FORCE)
#        set(VFIT_USE_MATPLOTLIB OFF CACHE BOOL "" FORCE)

        add_subdirectory(${diemer_SOURCE_DIR} ${diemer_BINARY_DIR})
    else()
        message(STATUS "diemer already populated")
    endif()
endif()

#if (TARGET vfit)
#    get_target_property(INC vfit INTERFACE_INCLUDE_DIRECTORIES)
#    message(STATUS "Found vfit : ${INC}")
#else()
#    message(STATUS "vfit target NOT FOUND")
#endif()
