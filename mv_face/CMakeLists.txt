project(mv_face_port)
cmake_minimum_required(VERSION 2.6)

if(MEDIA_VISION_FACE_LICENSE_PORT)
    add_subdirectory(${PROJECT_SOURCE_DIR}/face_lic) # Licensed port
else()
    add_subdirectory(${PROJECT_SOURCE_DIR}/face) # Open port
endif()
