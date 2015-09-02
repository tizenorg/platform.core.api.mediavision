project(mv_image_port)
cmake_minimum_required(VERSION 2.6)

if(MEDIA_VISION_IMAGE_PORT)
    add_subdirectory(${PROJECT_SOURCE_DIR}/image_lic) # Licensed port
else()
    add_subdirectory(${PROJECT_SOURCE_DIR}/image) # Open port
endif()
