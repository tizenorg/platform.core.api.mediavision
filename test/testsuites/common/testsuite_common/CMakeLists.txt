SET (pkgname mv_testsuite_common)
PROJECT(${pkgname})

cmake_minimum_required(VERSION 2.6)

SET(INC_DIR include)
INCLUDE_DIRECTORIES(${INC_DIR})
include_directories(${INC_IMAGE_HELPER})
include_directories(${INC_VIDEO_HELPER})

SET(CMAKE_C_FLAGS "-I./include -I./include/headers ${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -fPIC -Wall")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g")

ADD_LIBRARY(${pkgname} SHARED "mv_testsuite_common.c" "mv_testsuite_common.h")

TARGET_LINK_LIBRARIES(${pkgname} ${MV_COMMON_LIB_NAME} mv_image_helper)

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${LIB_INSTALL_DIR})
