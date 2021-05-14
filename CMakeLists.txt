cmake_minimum_required(VERSION 3.16)
project(Assignment)

set(CMAKE_CXX_STANDARD 14)

FILE(COPY datasets/areas.csv DESTINATION "${CMAKE_BINARY_DIR}")

add_executable(Assignment main.cpp bethyw.cpp area.cpp areas.cpp measure.cpp input.cpp)