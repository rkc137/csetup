cmake_minimum_required(VERSION 3.10)
project(csetup LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)

FetchContent_Declare(
    string_literal
    GIT_REPOSITORY https://github.com/rkc137/string_literal.git
    GIT_TAG main
)
FetchContent_MakeAvailable(string_literal)

add_executable(csetup src/main.cpp)
target_link_libraries(csetup PRIVATE rkc::string_literal)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --static -fno-rtti") # modern cpp experience lol