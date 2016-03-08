cmake_minimum_required(VERSION 2.8)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src)
LINK_LIBRARIES(${PROJECT_NAME} )

ADD_EXECUTABLE(raspicam_cv_still_test raspicam_cv_still_test.cpp )
target_link_libraries(raspicam_cv_still_test ${PROJECT_NAME}_cv)
INSTALL(TARGETS raspicam_cv_still_test RUNTIME DESTINATION bin)
