cmake_minimum_required(VERSION 3.14)
project(options_pricing_engine LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(tomlplusplus)

include_directories(${CMAKE_SOURCE_DIR}/include)
file(GLOB SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/app/*.cpp
)
configure_file(${CMAKE_SOURCE_DIR}/option.toml ${CMAKE_BINARY_DIR}/option.toml COPYONLY)
add_executable(options_pricing_engine ${SOURCES})
target_link_libraries(options_pricing_engine PRIVATE tomlplusplus::tomlplusplus)
