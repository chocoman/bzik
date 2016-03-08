cmake_minimum_required (VERSION 2.8) 
project (presskeys)

find_package(OpenCV REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "/usr/local/lib/cmake")
find_package(raspicam REQUIRED)


add_executable (presskeys presskeys.cpp)  
target_link_libraries (presskeys ${raspicam_CV_LIBS} ${OpenCV_LIBS})
