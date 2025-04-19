cmake_minimum_required(VERSION 3.10)
project(csetup LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

add_executable(csetup
                        src/main.cpp
              )

target_compile_features(csetup PRIVATE cxx_std_17)