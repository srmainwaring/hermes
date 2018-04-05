cmake_minimum_required(VERSION 3.8)
project(messaging_polymorphism)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(src)
add_subdirectory(fmt)

add_library(hermes SHARED ${FILES})
target_link_libraries(hermes PRIVATE fmt::fmt-header-only)



add_executable(messaging_polymorphism main.cpp ${FILES})
target_link_libraries(messaging_polymorphism PRIVATE fmt::fmt-header-only)
