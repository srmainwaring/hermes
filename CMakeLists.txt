
# stackoverflow.com/questions/17511496/how-to-create-a-shared-library-with-cmake


cmake_minimum_required(VERSION 3.8)
project(messaging_polymorphism)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(src)
add_subdirectory(fmt)

add_library(hermes SHARED ${FILES})
target_link_libraries(hermes PRIVATE fmt::fmt-header-only)

#message(STATUS fmt::fmt-header-only)

list(APPEND HERMES_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/)


set_target_properties(hermes PROPERTIES PUBLIC_HEADER hermes.h)


#add_executable(messaging_polymorphism main.cpp ${FILES})
#target_link_libraries(messaging_polymorphism PRIVATE fmt::fmt-header-only)
