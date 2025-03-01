cmake_minimum_required(VERSION 3.10.0)
project(Lab2 VERSION 0.1.0 LANGUAGES C CXX)

add_executable(Lab2 main.cpp Vector3D.cpp)

include(CTest)
enable_testing()

