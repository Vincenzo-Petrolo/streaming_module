cmake_minimum_required(VERSION 3.10)
project(test_cameraServiceLaunc CXX)

set(CMAKE_CXX_STANDARD 14)

add_executable(test_cameraServiceLaunc main.cpp lib/Stream/stream.h)
