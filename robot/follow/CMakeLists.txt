cmake_minimum_required (VERSION 2.8) 
project (follow)

find_package(OpenCV REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "/usr/local/lib/cmake")
find_package(raspicam REQUIRED)


add_executable (follow follow.cpp)  
target_link_libraries (follow ${raspicam_CV_LIBS} ${OpenCV_LIBS})
