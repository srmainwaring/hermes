cmake_minimum_required(VERSION 3.8)
project(messaging_polymorphism)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(src)

add_executable(messaging_polymorphism main.cpp ${FILES})