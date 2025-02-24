cmake_minimum_required(VERSION 3.16)
project(vrob)

set(CMAKE_CXX_STANDARD 17)

set(OpenCV_DIR "C:/Libraries/opencv/build")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV include dirs: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")

add_executable(${PROJECT_NAME} main.cpp
        includes/Calibrator.h
)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

