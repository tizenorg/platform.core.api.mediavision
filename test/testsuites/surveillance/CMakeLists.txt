project(mv_surveillance_test_suite)
cmake_minimum_required(VERSION 2.6)

set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_DEBUG    _DEBUG)

if(NOT SKIP_WARNINGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${LIB_INSTALL_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${LIB_INSTALL_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${MV_CAPI_MEDIA_VISION_INC_DIR})
include_directories(${INC_TS_COMMON})
include_directories(${INC_VIDEO_HELPER})
include_directories(${INC_IMAGE_HELPER})

file(GLOB MV_SURVEILLANCE_TS_INC_LIST "${PROJECT_SOURCE_DIR}/*.h")
file(GLOB MV_SURVEILLANCE_TS_SRC_LIST "${PROJECT_SOURCE_DIR}/*.c")

add_executable(${PROJECT_NAME} ${MV_SURVEILLANCE_TS_SRC_LIST}
                               ${MV_SURVEILLANCE_TS_INC_LIST}
                               ${MV_CAPI_MEDIA_VISION_INC_LIST})

target_link_libraries(${PROJECT_NAME} capi-media-vision
                                      mv_testsuite_common
                                      mv_image_helper)

install(TARGETS ${PROJECT_NAME} DESTINATION ${testbin_dir})
