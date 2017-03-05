cmake_minimum_required (VERSION 2.8) 
project (presskeys)

find_package(OpenCV REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "/usr/local/lib/cmake")

add_executable (training training.cpp)  
target_link_libraries (training ${OpenCV_LIBS})

add_executable (trainNetwork trainNetwork.cpp)  
target_link_libraries (trainNetwork ${OpenCV_LIBS})
