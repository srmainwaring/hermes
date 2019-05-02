set(Eigen3_URL http://bitbucket.org/eigen/eigen/get/3.3.4.tar.gz)

# Hack to make possible to include Eigen3Targets.cmake from the Eigen3 build dir
# as called by Eigen3Config.cmake that is used while running find_package(Eigen3)
if (POLICY CMP0024)
    cmake_policy(SET CMP0024 OLD)
endif()

if (NOT TARGET Eigen3::Eigen)  # if we haven't found the target yet
  find_package(Eigen3 QUIET)
  # In some case, the EIGEN3_INCLUDE_DIR does not exist (maybe a bug in the
  # FindEigen3 script at a given version)
  # We need to build from source if we can't find it or if the include dir isn't
  # right.
  if (NOT Eigen3_FOUND OR NOT EXISTS ${EIGEN3_INCLUDE_DIR})
      include(FetchContent)

      FetchContent_Declare(Eigen3 URL ${Eigen3_URL})
      FetchContent_GetProperties(Eigen3)
      if(NOT Eigen3_POPULATED)
          message(STATUS "Downloading, Configuring and Generating 'Eigen3' dependency")
          FetchContent_Populate(Eigen3)
          add_subdirectory(${eigen3_SOURCE_DIR} ${eigen3_BINARY_DIR})

          # To make the Eigen3::Eigen target readily available at first Add
          get_target_property(INC eigen INTERFACE_INCLUDE_DIRECTORIES)
          if (TARGET Eigen3::Eigen)
            set_target_properties(Eigen3::Eigen
              PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${INC}"
            )
          endif()
          add_library(Eigen3::Eigen ALIAS eigen)
      else()
          message(STATUS "Eigen3 already populated")
      endif()
  endif()
endif()

# Hack for GeographicLib library as Eigen set MPFR_LIBRARIES to NOT_FOUND and is
# not accepted by GeographicLib build chain
if (NOT MPFR_LIBRARIES)
    set(MPFR_LIBRARIES "")
endif()

if (TARGET Eigen3::Eigen)
    get_target_property(INC Eigen3::Eigen INTERFACE_INCLUDE_DIRECTORIES)
    message(STATUS "Eigen target FOUND : ${INC}")
else()
    message(STATUS "Eigen target NOT FOUND")
endif()
