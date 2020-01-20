find_package(fmt QUIET)
if (NOT fmt_FOUND)
    include(FetchContent)

    set(fmt_URL https://github.com/fmtlib/fmt.git)
    FetchContent_Declare(fmt
            GIT_REPOSITORY ${fmt_URL}
            GIT_TAG 6.1.2
            )

    FetchContent_GetProperties(fmt)
    if(NOT fmt_POPULATED)
        message(STATUS "Downloading, Configuring and Generating 'fmt' dependency")
        FetchContent_Populate(fmt)

        # FMT BUILD OPTIONS
        set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/fmt)
        set(BUILD_SHARED_LIBS TRUE)
        
        set(FMT_TEST OFF CACHE BOOL "Generate the test target." FORCE)
        set(FMT_DOC OFF CACHE BOOL "Generate the doc target." FORCE)
        set(FMT_INSTALL OFF CACHE BOOL "Generate the install target." FORCE)

        add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR})
    endif()
endif()
