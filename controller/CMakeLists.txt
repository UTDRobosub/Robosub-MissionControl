cmake_minimum_required(VERSION 3.2)
project(robosub-controller CXX)
find_package(SDL REQUIRED)
set(CMAKE_CXX_STANDARD 11)

file(GLOB SOURCES src/*.cpp)

add_executable(robosub-controller ${SOURCES})
target_link_libraries(robosub-controller 
${SDL_LIBRARY})
target_compile_features(robosub-controller PRIVATE cxx_range_for)
