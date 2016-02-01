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

#ifndef __IMAGE_HELPER_H__
#define __IMAGE_HELPER_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "mv_common.h"

#ifdef __cplusplus
#include <cstddef>
#include <cstdio>
#else
#include <stddef.h>
#include <stdio.h>
#endif

/**
 * @file   image_helper.h
 * @brief  Helper functions that provides set of useful methods for image management
 *         (c wrapper of ImageHelper class).
 */

/**
 * @brief Structure to keep information about width, height and colorspace of an image.
 *
 * @since_tizen 3.0
 */
typedef struct
{
    unsigned int image_width;                 /**< Image width */
    unsigned int image_height;                /**< Image height */
    mv_colorspace_e image_colorspace;         /**< Image colorspace */
} image_data_s;

/**
 * @brief Loads image from file to the buffer of unsigned chars.
 *
 * @since_tizen 3.0
 * @param [in]  file_path      Path to the image file to be loaded to the
 *                             @a pDataBuffer
 * @param [out] data_buffer    The buffer of unsigned chars where image data
 *                             will be stored
 * @param [out] buffer_size    The size of the @a pDataBuffer
 * @param [out] image_data     The image data (structure that keeps
 *                             information about image width, height,
 *                             and colorspace)
 * @return @c 0 on success, otherwise a negative error value
 *
 * @see save_image_from_buffer()
 * @see destroy_loaded_buffer()
 */
int load_image_to_buffer(
        const char *file_path,
        unsigned char **data_buffer,
        unsigned long *buffer_size,
        image_data_s *image_data);

/**
 * @brief Saves image stored into @a pDataBuffer to the file in jpeg format.
 *
 * @since_tizen 3.0
 * @param [in] file_path      The path to the file where image will be saved
 * @param [in] data_buffer    Data buffer that contains image data
 * @param [in] image_data     The image data (structure that keeps
 *                            information about image width, height,
 *                            and colorspace)
 * @param [in] quality        Quality for the output jpeg file (0..100)
 * @return @c 0 on success, otherwise a negative error value
 *
 * @see load_image_to_buffer()
 */
int save_image_from_buffer(
        const char *file_path,
        unsigned char *data_buffer,
        const image_data_s *image_data,
        int quality);

/**
 * @brief Destroys loaded buffer by load_image_to_buffer().
 *
 * @since_tizen 3.0
 * @param [in] data_buffer    Data buffer to be deallocated
 * @return @c 0 on success, otherwise a negative error value
 *
 * @see load_image_to_buffer()
 */
int destroy_loaded_buffer(unsigned char *data_buffer);

/**
 * @brief Draws the rectangle of specified size on the image data buffer.
 *
 * @since_tizen 3.0
 * @param [in]  tl_vertex_x         The rectangle top left corner x coordinate
 * @param [in]  tl_vertex_y         The rectangle top left corner y coordinate
 * @param [in]  br_vertex_x         The rectangle bottom right corner x coordinate
 * @param [in]  br_vertex_y         The rectangle bottom right corner y coordinate
 * @param [in]  thickness           The thickness of the rectangle border (negative
 *                                  value to draw filled rectangle)
 * @param [in]  rgb_color           The color of the rectangle border
 * @param [in]  image_data          The image data (structure that keeps information
 *                                  about image width, height, and colorspace).
 *                                  Colorspace has to be @c MEDIA_VISION_COLORSPACE_RGB888
 * @param [in, out] data_buffer     The pointer to the image data buffer which will
 *                             be used for rectangle drawing
 * @return @c 0 on success, otherwise a negative error value
 */
int draw_rectangle_on_buffer(
        int tl_vertex_x,
        int tl_vertex_y,
        int br_vertex_x,
        int br_vertex_y,
        int thickness,
        const int rgb_color[3],
        const image_data_s *image_data,
        unsigned char *data_buffer);

/**
 * @brief Draws the quadrangle of specified size on the image data buffer.
 *
 * @since_tizen 3.0
 * @param [in]  location          The quadrangle structure
 * @param [in]  thickness         The thickness of the quadrangle border
 * @param [in]  rgb_color         The color of the quadrangle border
 * @param [in]  image_data        The image data (structure that keeps information
 *                                about image width, height, and colorspace).
 *                                Colorspace has to be @c MEDIA_VISION_COLORSPACE_RGB888
 * @param [in, out] data_buffer   The pointer to the image data buffer which will
 *                                be used for quadrangle drawing
 * @return @c 0 on success, otherwise a negative error value
 */
int draw_quadrangle_on_buffer(
        mv_quadrangle_s location,
        int thickness,
        const int rgb_color[3],
        const image_data_s *image_data,
        unsigned char *data_buffer);

/**
 * @brief Converts image data to the image data of RGB888 colorspace.
 *
 * @since_tizen 3.0
 * @param [in]  in_buffer     Buffer with image data to be converted to
 *                            RGB888 colorspace
 * @param [in]  image_data    The image data (structure that keeps
 *                            information about image width, height,
 *                            and colorspace) for source image
 * @param [out] out_buffer    Buffer with image data to be generated as
 *                            a result of the conversion
 * @return @c 0 on success, otherwise a negative error value
 */
int convert_buffer_to_RGB888(
        const unsigned char *in_buffer,
        const image_data_s *image_data,
        unsigned char **out_buffer);

/**
 * @brief Determines number of channels (components) for the colorspace.
 *
 * @since_tizen 3.0
 * @param [in]  colorspace           Colorspace for which number of
 *                                   components will be determined
 * @param [out] components_number    Number of components to be determined
 * @return @c 0 on success, otherwise a negative error value
 */
int get_number_of_components(
        mv_colorspace_e colorspace,
        int *components_number);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __IMAGE_HELPER_H__*/
