/**
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "image_helper.h"

#include "mv_private.h"
#include "ImageHelper.h"

#include <opencv2/core/core.hpp>

/**
 * @file   image_helper.cpp
 * @brief  image_helper.h functions implementation.
 */

using namespace MediaVision::Common;

image_data_s convertToCData(ImageHelper::ImageData data)
{
    image_data_s ret;
    ret.image_width = data.imageWidth;
    ret.image_height = data.imageHeight;
    ret.image_colorspace = data.imageColorspace;
    return ret;
}

ImageHelper::ImageData convertToCppData(image_data_s data)
{
    ImageHelper::ImageData ret;
    ret.imageWidth = data.image_width;
    ret.imageHeight = data.image_height;
    ret.imageColorspace = data.image_colorspace;
    return ret;
}


int load_image_to_buffer(
        const char *file_path,
        unsigned char **data_buffer,
        unsigned long *buffer_size,
        image_data_s *image_data)
{
    if (image_data == NULL)
    {
        return MEDIA_VISION_ERROR_INVALID_PARAMETER;
    }

    int err;
    ImageHelper::ImageData imageData;
    err = ImageHelper::loadImageToBuffer(file_path, data_buffer, buffer_size, &imageData);

    if (err == MEDIA_VISION_ERROR_NONE)
    {
        *image_data = convertToCData(imageData);
    }

    return err;
}

int save_image_from_buffer(
        const char *file_path,
        unsigned char *data_buffer,
        const image_data_s *image_data,
        int quality)
{
    if (image_data == NULL)
    {
        return MEDIA_VISION_ERROR_INVALID_PARAMETER;
    }

    ImageHelper::ImageData imageData = convertToCppData(*image_data);
    return ImageHelper::saveImageFromBuffer(file_path, data_buffer, imageData, quality);
}

int destroy_loaded_buffer(unsigned char *data_buffer)
{
	return ImageHelper::destroyLoadedBuffer(data_buffer);
}

int draw_rectangle_on_buffer(
        int tl_vertex_x,
        int tl_vertex_y,
        int br_vertex_x,
        int br_vertex_y,
        int thickness,
        const int rgb_color[3],
        const image_data_s *image_data,
        unsigned char *data_buffer)
{
    if (image_data == NULL)
    {
        return MEDIA_VISION_ERROR_INVALID_PARAMETER;
    }

    ImageHelper::ImageData imageData = convertToCppData(*image_data);

    cv::Scalar color(rgb_color[2], rgb_color[1], rgb_color[0]);

    return ImageHelper::drawRectangleOnBuffer(
                            tl_vertex_x, tl_vertex_y,
                            br_vertex_x, br_vertex_y,
                            thickness,
                            color,
                            imageData, data_buffer);
}

int draw_quadrangle_on_buffer(
        mv_quadrangle_s location,
        int thickness,
        const int rgb_color[3],
        const image_data_s *image_data,
        unsigned char *data_buffer)
{
    if (image_data == NULL)
    {
        return MEDIA_VISION_ERROR_INVALID_PARAMETER;
    }

    ImageHelper::ImageData imageData = convertToCppData(*image_data);

    cv::Scalar color(rgb_color[2], rgb_color[1], rgb_color[0]);

    return ImageHelper::drawQuadrangleOnBuffer(
                            location,
                            thickness,
                            color,
                            imageData,
                            data_buffer);
}

int convert_buffer_to_RGB888(
        const unsigned char *in_buffer,
        const image_data_s *image_data,
        unsigned char **out_buffer)
{
    if (image_data == NULL)
    {
        return MEDIA_VISION_ERROR_INVALID_PARAMETER;
    }

    ImageHelper::ImageData imageData = convertToCppData(*image_data);
    return ImageHelper::convertBufferToRGB888(in_buffer, imageData, out_buffer);
}

int get_number_of_components(
        mv_colorspace_e colorspace,
        int *components_number)
{
    return ImageHelper::getNumberOfComponents(colorspace, components_number);
}
