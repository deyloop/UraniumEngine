cmake_minimum_required (VERSION 3.6)

project(uranium-core)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD 11)

find_package(X11 REQUIRED)
link_libraries(${X11_LIBRARIES})

include_directories(${X11_INCLUDE_DIR} ${PROJECT_SOURCE_DIR}/headers)

file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

set(SOURCE_FILES src/main.cpp)
add_executable(uranium-core ${SOURCE_FILES})
