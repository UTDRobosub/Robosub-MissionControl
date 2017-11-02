cmake_minimum_required(VERSION 3.2)
project(robosub-controller CXX)
find_package(SDL REQUIRED)
find_package(OpenCV REQUIRED)
set(CMAKE_CXX_STANDARD 11)

file(GLOB SOURCES controller/src/*.cpp)

add_executable(robosub-controller ${SOURCES})
target_link_libraries(robosub-controller 
${SDL_LIBRARY} ${OpenCV_LIBS})
target_compile_features(robosub-controller PRIVATE cxx_range_for)
