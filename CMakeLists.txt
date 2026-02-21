cmake_minimum_required(VERSION 3.23)
project(csv_median_calculator LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# 1. Скачиваем Boost
FetchContent_Declare(
    boost
    GIT_REPOSITORY https://github.com/boostorg/boost.git
    GIT_TAG boost-1.86.0
)
FetchContent_MakeAvailable(boost)


FetchContent_Declare(tomlplusplus GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git GIT_TAG v3.4.0)
FetchContent_MakeAvailable(tomlplusplus)
FetchContent_Declare(spdlog GIT_REPOSITORY https://github.com/gabime/spdlog.git GIT_TAG v1.14.1)
FetchContent_MakeAvailable(spdlog)
FetchContent_Declare(fmt GIT_REPOSITORY https://github.com/fmtlib/fmt.git GIT_TAG 11.0.2)
FetchContent_MakeAvailable(fmt)



# Приложение
add_executable(
    csv_median_calculator 
    src/main.cpp src/csv_reader.hpp 
    src/csv_reader.cpp
    src/median_calculator.hpp
    src/median_calculator.cpp
    src/config_parser.hpp
    src/config_parser.cpp
    )
target_link_libraries(csv_median_calculator PRIVATE
    boost_headers
    tomlplusplus::tomlplusplus
    spdlog::spdlog
    fmt::fmt
    Boost::program_options
)

message(STATUS "boost_SOURCE_DIR = ${boost_SOURCE_DIR}")
message(STATUS "Boost_INCLUDE_DIR = ${Boost_INCLUDE_DIR}")

file(GLOB BOOST_INCLUDE_DIRS "${boost_SOURCE_DIR}/libs/*/include")

target_include_directories(csv_median_calculator PRIVATE 
    ${BOOST_INCLUDE_DIRS}
)



    
    
