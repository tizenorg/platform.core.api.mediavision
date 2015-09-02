SET (pkgname mv_video_helper)
PROJECT(${pkgname})

cmake_minimum_required(VERSION 2.6)

SET(INC_DIR include)
INCLUDE_DIRECTORIES(${INC_DIR})
include_directories(${INC_IMAGE_HELPER})

SET(dependents "gstreamer-1.0 gstreamer-app-1.0 dlog")

INCLUDE(FindPkgConfig)
pkg_check_modules(${pkgname} REQUIRED ${dependents})
FOREACH(flag ${${pkgname}_CFLAGS})
  SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS "-I./include -I./include/headers ${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -fPIC -Wall")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g")

ADD_LIBRARY(${pkgname} SHARED "mv_video_helper.c" "mv_video_helper.h")

TARGET_LINK_LIBRARIES(${pkgname} ${${pkgname}_LDFLAGS} gstvideo-1.0)

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${LIB_INSTALL_DIR})
