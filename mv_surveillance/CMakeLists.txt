project(mv_surveillance_port)
cmake_minimum_required(VERSION 2.6)

if(MEDIA_VISION_SURVEILLANCE_LICENSE_PORT)
    add_subdirectory(${PROJECT_SOURCE_DIR}/surveillance_lic) # Licensed port
else()
    add_subdirectory(${PROJECT_SOURCE_DIR}/surveillance) # Open port
endif()
