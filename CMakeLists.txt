cmake_minimum_required(VERSION 3.10)
project(test)

set(OpenCV_DIR C:/opencv/build)

find_package(OpenCV REQUIRED)

add_executable(${PROJECT_NAME} src/test.cpp)


target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})