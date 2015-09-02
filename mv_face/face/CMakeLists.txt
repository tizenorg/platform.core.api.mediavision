project(${MV_FACE_LIB_NAME})
cmake_minimum_required(VERSION 2.6)

set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_DEBUG _DEBUG)

if(NOT SKIP_WARNINGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${LIB_INSTALL_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${LIB_INSTALL_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/src")

file(GLOB MV_FACE_INCLUDE_LIST "${PROJECT_SOURCE_DIR}/include/*.h")
file(GLOB MV_FACE_SRC_LIST "${PROJECT_SOURCE_DIR}/src/*.cpp")

find_package(OpenCV REQUIRED core objdetect contrib)
if(NOT OpenCV_FOUND)
    message(SEND_ERROR "Failed to find OpenCV")
    return()
else()
    include_directories(${OpenCV_INCLUDE_DIRS})
endif()

if(FORCED_STATIC_BUILD)
    add_library(${PROJECT_NAME} STATIC ${MV_FACE_INCLUDE_LIST} ${MV_FACE_SRC_LIST})
else()
    add_library(${PROJECT_NAME} SHARED ${MV_FACE_INCLUDE_LIST} ${MV_FACE_SRC_LIST})
endif()

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} ${MV_COMMON_LIB_NAME} dlog)

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${LIB_INSTALL_DIR})
