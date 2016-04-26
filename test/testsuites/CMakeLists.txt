project(mv_testsuites)
cmake_minimum_required(VERSION 2.6)

SET(INC_IMAGE_HELPER "${PROJECT_SOURCE_DIR}/common/image_helper/include")
SET(INC_VIDEO_HELPER "${PROJECT_SOURCE_DIR}/common/video_helper")
SET(INC_TS_COMMON "${PROJECT_SOURCE_DIR}/common/testsuite_common")

add_subdirectory(${PROJECT_SOURCE_DIR}/common)
add_subdirectory(${PROJECT_SOURCE_DIR}/barcode)
add_subdirectory(${PROJECT_SOURCE_DIR}/face)
add_subdirectory(${PROJECT_SOURCE_DIR}/image)
add_subdirectory(${PROJECT_SOURCE_DIR}/surveillance)
