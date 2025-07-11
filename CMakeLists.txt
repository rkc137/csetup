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
FetchContent_Declare(
  battery-embed
  GIT_REPOSITORY https://github.com/batterycenter/embed.git
  GIT_TAG main
)
FetchContent_MakeAvailable(battery-embed)

add_executable(${PROJECT_NAME} src/main.cpp)

b_embed(${PROJECT_NAME} res/templates/CMakeLists.txt)
b_embed(${PROJECT_NAME} res/templates/.gitignore)
b_embed(${PROJECT_NAME} res/templates/main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE rkc::string_literal)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --static -fno-rtti") # modern cpp experience lol