cmake_minimum_required(VERSION 3.2)
project(robosub-controller CXX)
find_package(SDL REQUIRED)
find_package(OpenCV REQUIRED)
find_package(robosub REQUIRED)
set(CMAKE_CXX_STANDARD 11)

file(GLOB SOURCES src/*.cpp src/**/*.cpp src/*.cc src/**/*.cc)
file(GLOB HEADERS include/*.h include/**/*.h)
include_directories(include)

add_executable(robosub-controller ${SOURCES})
target_link_libraries(robosub-controller robosub ${robosub_LIBS} ${SDL_LIBRARY} ${OpenCV_LIBS})
target_compile_features(robosub-controller PRIVATE cxx_range_for)
