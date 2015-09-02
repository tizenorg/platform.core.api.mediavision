project(mv_barcode)
cmake_minimum_required(VERSION 2.6)

if(MEDIA_VISION_BARCODE_DETECTOR_LICENSE_PORT)
    add_subdirectory(${PROJECT_SOURCE_DIR}/barcode_detector_lic) # Licensed port
else()
    add_subdirectory(${PROJECT_SOURCE_DIR}/barcode_detector) # Open port
endif()

if(MEDIA_VISION_BARCODE_GENERATOR_LICENSE_PORT)
    add_subdirectory(${PROJECT_SOURCE_DIR}/barcode_generator_lic) # Licened port
else()
    add_subdirectory(${PROJECT_SOURCE_DIR}/barcode_generator) # Open port
endif()
