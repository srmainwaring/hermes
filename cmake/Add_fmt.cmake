find_package(fmt QUIET)
if (NOT fmt_FOUND)
    include(FetchContent)

    set(fmt_URL https://github.com/fmtlib/fmt.git)
    IF(MINGW OR MSVC)
        #add_definitions(-DFMT_DLL)
        message(STATUS "FetchContent_Declare 'Fmt' dependency for MS Windows")
        FetchContent_Declare(fmt
            GIT_REPOSITORY ${fmt_URL}
            GIT_TAG 4.1.0
            PATCH_COMMAND git apply "${CMAKE_CURRENT_SOURCE_DIR}/cmake/patches/fmt-windows.patch"
            )
    ELSE(MINGW OR MSVC)
        FetchContent_Declare(fmt
            GIT_REPOSITORY ${fmt_URL}
            GIT_TAG 4.1.0
            )
    ENDIF(MINGW OR MSVC)

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
