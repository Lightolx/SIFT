cmake_minimum_required(VERSION 3.14)
project(SIFT)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(GFTT GFTT.cpp)
target_link_libraries(GFTT ${OpenCV_LIBS})

add_executable(SIFT SIFT.cpp)
target_link_libraries(SIFT ${OpenCV_LIBS})

add_executable(Harris Harris.cpp)
target_link_libraries(Harris ${OpenCV_LIBS})

add_executable(Faster Faster.cpp)
target_link_libraries(Faster ${OpenCV_LIBS})

add_executable(Image2Vedio Image2Vedio.cpp)
target_link_libraries(Image2Vedio ${OpenCV_LIBS})

add_executable(hough hough.cpp)
target_link_libraries(hough ${OpenCV_LIBS})