
set(Eigen3_URL http://bitbucket.org/eigen/eigen/get/3.3.4.tar.gz)

# Hack to make possible to include Eigen3Targets.cmake from the Eigen3 build dir as called by Eigen3Config.cmake
# that is used while running find_package(Eigen3)
if (POLICY CMP0024)
    cmake_policy(SET CMP0024 OLD)
endif()

find_package(Eigen3 QUIET)
if (NOT Eigen3_FOUND)
    include(FetchContent)

    FetchContent_Declare(Eigen3
            URL ${Eigen3_URL}
            )

    FetchContent_GetProperties(Eigen3)
    if(NOT eigen3_POPULATED)
        message(STATUS "Downloading, Configuring and Generating 'Eigen3' dependency")
        FetchContent_Populate(Eigen3)

        add_subdirectory(${eigen3_SOURCE_DIR} ${eigen3_BINARY_DIR})

        find_package(Eigen3) # To make the Eigen3::Eigen target readily available at first Add
    else()
        message(STATUS "Eigen3 already populated")
    endif()
endif()

if (NOT MPFR_LIBRARIES)  # Hack for GeographicLib library as Eigen set MPFR_LIBRARIES to NOT_FOUND and is not accepted by GeographicLib build chain
    set(MPFR_LIBRARIES "")
endif()


if (TARGET Eigen3::Eigen)

    get_target_property(INC Eigen3::Eigen INTERFACE_INCLUDE_DIRECTORIES)
    message(STATUS "Eigen target FOUND : ${INC}")

else()
    message(STATUS "Eigen target NOT FOUND")
endif()
