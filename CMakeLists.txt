
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(fw_name "capi-media-vision")

PROJECT(${fw_name})

SET(CMAKE_INSTALL_PREFIX /usr)
SET(PREFIX ${CMAKE_INSTALL_PREFIX})

# Configure for porting layer:

# Configure for porting layer:

option(MEDIA_VISION_BARCODE_DETECTOR_LICENSE_PORT
       "Turn on building of licensed port of the barcode detecting module (if OFF - open port will be built)." OFF)
option(MEDIA_VISION_BARCODE_GENERATOR_LICENSE_PORT
       "Turn on building of licensed port of the barcode generating module (if OFF - open port will be built)." OFF)
option(MEDIA_VISION_IMAGE_LICENSE_PORT
       "Turn on building of licensed port of the image module (if OFF - open port will be built)." OFF)
option(MEDIA_VISION_FACE_LICENSE_PORT
       "Turn on building of licensed port of the face module (if OFF - open port will be built)." OFF)
option(MEDIA_VISION_SURVEILLANCE_LICENSE_PORT
       "Turn on building of licensed port of the surveillance module (if OFF - open port will be built)." OFF)

set(MV_COMMON_LIB_NAME "mv_common")
set(MV_BARCODE_DETECTOR_LIB_NAME "mv_barcode_detector" CACHE STRING
       "Name of the library will be built for barcode detecting module (without extension).")
set(MV_BARCODE_GENERATOR_LIB_NAME "mv_barcode_generator" CACHE STRING
       "Name of the library will be built for barcode generating module (without extension).")
set(MV_IMAGE_LIB_NAME "mv_image" CACHE STRING
       "Name of the library will be built for image module (without extension).")
set(MV_FACE_LIB_NAME "mv_face" CACHE STRING
       "Name of the library will be built for barcode generating module (without extension).")
set(MV_SURVEILLANCE_LIB_NAME "mv_surveillance" CACHE STRING
       "Name of the library will be built for surveillance module (without extension).")


SET(INC_DIR "${PROJECT_SOURCE_DIR}/include")
SET(INC_COMMON "${PROJECT_SOURCE_DIR}/mv_common/include")

message(STATUS "In_dir ${INC_DIR}")

if(MEDIA_VISION_BARCODE_DETECTOR_LICENSE_PORT)
    add_definitions(-DMEDIA_VISION_BARCODE_DETECTOR_LICENSE_PORT)
    SET(INC_BARCODE_DETECTOR "${PROJECT_SOURCE_DIR}/mv_barcode/barcode_detector_lic/include")
else()
    SET(INC_BARCODE_DETECTOR "${PROJECT_SOURCE_DIR}/mv_barcode/barcode_detector/include")
endif()

if(MEDIA_VISION_BARCODE_GENERATOR_LICENSE_PORT)
    add_definitions(-DMEDIA_VISION_BARCODE_GENERATOR_LICENSE_PORT)
    SET(INC_BARCODE_GENERATOR "${PROJECT_SOURCE_DIR}/mv_barcode/barcode_generator_lic/include")
else()
    SET(INC_BARCODE_GENERATOR "${PROJECT_SOURCE_DIR}/mv_barcode/barcode_generator/include")
endif()

if(MEDIA_VISION_IMAGE_LICENSE_PORT)
    add_definitions(-DMEDIA_VISION_IMAGE_LICENSE_PORT)
    SET(INC_IMAGE "${PROJECT_SOURCE_DIR}/mv_image/image_lic/include")
else()
    SET(INC_IMAGE "${PROJECT_SOURCE_DIR}/mv_image/image/include")
endif()

if(MEDIA_VISION_FACE_LICENSE_PORT)
    add_definitions(-DMEDIA_VISION_FACE_LICENSE_PORT)
    SET(INC_FACE "${PROJECT_SOURCE_DIR}/mv_face/face_lic/include")
else()
    SET(INC_FACE "${PROJECT_SOURCE_DIR}/mv_face/face/include")
endif()

if(MEDIA_VISION_SURVEILLANCE_LICENSE_PORT)
    add_definitions(-DMEDIA_VISION_SURVEILLANCE_LICENSE_PORT)
    SET(INC_SURVEILLANCE "${PROJECT_SOURCE_DIR}/mv_surveillance/surveillance_lic/include")
else()
    SET(INC_SURVEILLANCE "${PROJECT_SOURCE_DIR}/mv_surveillance/surveillance/include")
endif()

INCLUDE_DIRECTORIES(${INC_DIR}
                    ${INC_COMMON}
                    ${INC_BARCODE_DETECTOR}
                    ${INC_BARCODE_GENERATOR}
                    ${INC_FACE}
                    ${INC_IMAGE}
                    ${INC_SURVEILLANCE})

SET(dependents "dlog capi-media-tool capi-system-info capi-appfw-application")
SET(pc_dependents "dlog")

INCLUDE(FindPkgConfig)
pkg_check_modules(${fw_name} REQUIRED ${dependents})
FOREACH(flag ${${fw_name}_CFLAGS})
    SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
    SET(EXTRA_CXXFLAGS "${EXTRA_CXXFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS "-I./include -I./include/headers ${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -fPIC -Wall -w")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g")

SET(CMAKE_CXX_FLAGS "-I./include -I./include/headers ${CMAKE_CXX_FLAGS} ${EXTRA_CXXFLAGS} -fPIC")
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g --w")

IF("${ARCH}" STREQUAL "arm")
    ADD_DEFINITIONS("-DTARGET")
ENDIF("${ARCH}" STREQUAL "arm")

ADD_DEFINITIONS("-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\"")
ADD_DEFINITIONS("-DTIZEN_DEBUG")

SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed -Wl,--rpath=${LIB_INSTALL_DIR}")

ADD_SUBDIRECTORY(mv_common)
ADD_SUBDIRECTORY(mv_barcode)
ADD_SUBDIRECTORY(mv_image)
ADD_SUBDIRECTORY(mv_face)
ADD_SUBDIRECTORY(mv_surveillance)

aux_source_directory(src SOURCES)
ADD_LIBRARY(${fw_name} SHARED ${SOURCES})

TARGET_LINK_LIBRARIES(${fw_name} ${${fw_name}_LDFLAGS}
                                 ${MV_COMMON_LIB_NAME}
                                 ${MV_BARCODE_DETECTOR_LIB_NAME}
                                 ${MV_BARCODE_GENERATOR_LIB_NAME}
                                 ${MV_IMAGE_LIB_NAME}
                                 ${MV_FACE_LIB_NAME}
                                 ${MV_SURVEILLANCE_LIB_NAME})

SET_TARGET_PROPERTIES(${fw_name}
     PROPERTIES
     VERSION ${FULLVER}
     SOVERSION ${MAJORVER}
     CLEAN_DIRECT_OUTPUT 1
)

INSTALL(TARGETS ${fw_name} DESTINATION ${LIB_INSTALL_DIR})
INSTALL(
        DIRECTORY ${INC_DIR}/ DESTINATION include/media
        FILES_MATCHING
        PATTERN "*_private.h" EXCLUDE
        PATTERN "*.h"
        )

SET(PC_NAME ${fw_name})
SET(PC_REQUIRED ${pc_dependents})
SET(PC_LDFLAGS -l${fw_name})
SET(PC_CFLAGS -I\${includedir}/media)

CONFIGURE_FILE(
    ${fw_name}.pc.in
    ${CMAKE_CURRENT_SOURCE_DIR}/${fw_name}.pc
    @ONLY
)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${fw_name}.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)

ADD_SUBDIRECTORY(test)

IF(UNIX)

ADD_CUSTOM_TARGET (distclean @echo cleaning for source distribution)
ADD_CUSTOM_COMMAND(
        DEPENDS clean
        COMMENT "distribution clean"
        COMMAND find
        ARGS    .
        -not -name config.cmake -and \(
        -name tester.c -or
        -name Testing -or
        -name CMakeFiles -or
        -name cmake.depends -or
        -name cmake.check_depends -or
        -name CMakeCache.txt -or
        -name cmake.check_cache -or
        -name *.cmake -or
        -name Makefile -or
        -name core -or
        -name core.* -or
        -name gmon.out -or
        -name install_manifest.txt -or
        -name *.pc -or
        -name *~ \)
        | grep -v TC | xargs rm -rf
        TARGET  distclean
        VERBATIM
)

ENDIF(UNIX)

