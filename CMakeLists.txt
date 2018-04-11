cmake_minimum_required(VERSION 3.8)
project(messaging_polymorphism)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(src)
add_subdirectory(fmt)

add_library(hermes SHARED ${FILES})
target_link_libraries(hermes PRIVATE fmt::fmt-header-only)



add_executable(test_hermes main.cpp ${FILES})
target_link_libraries(test_hermes PRIVATE fmt::fmt-header-only)
