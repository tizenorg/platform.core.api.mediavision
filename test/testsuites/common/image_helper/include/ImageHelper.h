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

#ifndef __IMAGEHELPER_H__
#define __IMAGEHELPER_H__

#include "mv_common.h"

#include <stddef.h>
#include <stdio.h>

/**
 * @file   ImageHelper.h
 * @brief  ImageHelper class definition.
 */

namespace cv
{
    template<typename _Tp> class Scalar_;
    typedef Scalar_<double> Scalar;

    class VideoCapture;
    class VideoWriter;
}

namespace MediaVision
{
namespace Common
{

/**
 * @class ImageHelper
 * @brief Helper class that provides set of useful methods
 *        for image management.
 */
class EXPORT_API ImageHelper
{
public:
    /**
     * @brief Structure to keep information about width, height and colorspace of an image.
     */
    struct ImageData
    {
        unsigned int imageWidth;                 /**< Image width */
        unsigned int imageHeight;                /**< Image height */
        mv_colorspace_e imageColorspace;         /**< Image colorspace */
    };

   /**
     * @brief Loads image from file to the buffer of unsigned chars.
     *
     * @since_tizen 3.0
     * @param [in]  filePath       Path to the image file to be loaded to the
     *                             @a pDataBuffer
     * @param [out] pDataBuffer    The buffer of unsigned chars where image data
     *                             will be stored
     * @param [out] pBufferSize    The size of the @a pDataBuffer
     * @param [out] pImageData     The image data (structure that keeps
     *                             information about image width, height,
     *                             and colorspace)
     * @return @c 0 on success, otherwise a negative error value
     *
     * @see ImageHelper::saveImageFromBuffer()
	 * @see ImageHelper::destroyLoadedBuffer()
     */
    static int loadImageToBuffer(
            const char *filePath,
            unsigned char **pDataBuffer,
            unsigned long *pBufferSize,
            ImageData *pImageData);

    /**
     * @brief Saves image stored into @a pDataBuffer to the file in jpeg format.
     *
     * @since_tizen 3.0
     * @param [in] filePath       The path to the file where image will be saved
     * @param [in] pDataBuffer    Data buffer that contains image data
     * @param [in] imageData      The image data (structure that keeps
     *                            information about image width, height,
     *                            and colorspace)
     * @param [in] quality        Quality for the output jpeg file (0..100)
     * @return @c 0 on success, otherwise a negative error value
     *
     * @see ImageHelper::loadImageToBuffer()
     */
    static int saveImageFromBuffer(
            const char *filePath,
            unsigned char *pDataBuffer,
            const ImageData& imageData,
            int quality = 100);

   /**
     * @brief Destroys loaded buffer by loadImageToBuffer().
     *
     * @since_tizen 3.0
     * @param [out] pDataBuffer    The buffer of unsigned chars where image data
     *                             will be stored
     * @return @c 0 on success, otherwise a negative error value
     *
     * @see ImageHelper::loadImageToBuffer()
     */
    static int destroyLoadedBuffer(
            unsigned char *pDataBuffer);

    /**
     * @brief Draws the rectangle of specified size on the image data buffer.
     *
     * @since_tizen 3.0
     * @param [in]  topLeftVertexX        The rectangle top left corner
     *                                    x coordinate
     * @param [in]  topLeftVertexY        The rectangle top left corner
     *                                    y coordinate
     * @param [in]  bottomRightVertexX    The rectangle bottom right corner
     *                                    x coordinate
     * @param [in]  bottomRightVertexY    The rectangle bottom right corner
     *                                    y coordinate
     * @param [in]  thickness             The thickness of the rectangle border
     *                                    (negative value to draw filled rectangle)
     * @param [in]  color                 The color of the rectangle border
     * @param [in]  imageData             The image data (structure that keeps
     *                                    information about image width, height,
     *                                    and colorspace). Colorspace has to be
     *                                    @c MEDIA_VISION_COLORSPACE_RGB888
     * @param [in, out] pDataBuffer       The pointer to the image data buffer
     *                                    which will be used for rectangle drawing
     * @return @c 0 on success, otherwise a negative error value
     */
    static int drawRectangleOnBuffer(
            int topLeftVertexX,
            int topLeftVertexY,
            int bottomRightVertexX,
            int bottomRightVertexY,
            int thickness,
            const cv::Scalar& color,
            const ImageData& imageData,
            unsigned char *pDataBuffer);

    /**
     * @brief Draws the quadrangle of specified size on the image data buffer.
     *
     * @since_tizen 3.0
     * @param [in]  location              The quadrangle structure
     * @param [in]  thickness             The thickness of the quadrangle border
     * @param [in]  color                 The color of the quadrangle border
     * @param [in]  imageData             The image data (structure that keeps
     *                                    information about image width, height,
     *                                    and colorspace). Colorspace has to be
     *                                    @c MEDIA_VISION_COLORSPACE_RGB888
     * @param [in, out] pDataBuffer       The pointer to the image data buffer
     *                                    which will be used for quadrangle drawing
     * @return @c 0 on success, otherwise a negative error value
     */
    static int drawQuadrangleOnBuffer(
            mv_quadrangle_s location,
            int thickness,
            const cv::Scalar& color,
            const ImageData& imageData,
            unsigned char *pDataBuffer);

    /**
     * @brief Converts image data to the image data of RGB888 colorspace.
     *
     * @since_tizen 3.0
     * @param [in]  pInBuffer     Buffer with image data to be converted to
     *                            RGB888 colorspace
     * @param [in]  imageData     The image data (structure that keeps
     *                            information about image width, height,
     *                            and colorspace) for source image
     * @param [out] pOutBuffer    Buffer with image data to be generated as
     *                            a result of the conversion
     * @return @c 0 on success, otherwise a negative error value
     */
    static int convertBufferToRGB888(
            const unsigned char *pInBuffer,
            const ImageData& imageData,
            unsigned char **pOutBuffer);

    /**
     * @brief Determines number of channels (components) for the colorspace.
     *
     * @since_tizen 3.0
     * @param [in]  colorspace           Colorspace for which number of
     *                                   components will be determined
     * @param [out] pComponentsNumber    Number of components to be determined
     * @return @c 0 on success, otherwise a negative error value
     */
    static int getNumberOfComponents(
            mv_colorspace_e colorspace,
            int *pComponentsNumber);

private:
    /**
     * @brief Converts buffer with image data in Y800 colorspace format
     *        to the buffer with image data in RGB888 colorspace format.
     *
     * @since_tizen 3.0
     * @param [in]  pInBuffer     The buffer with data in Y800 colorspace format
     * @param [in]  imageData     The image data (structure that keeps
     *                            information about image width, height, and
     *                            colorspace) for source buffer
     * @param [out] pOutBuffer    The buffer that will contain converted image
     *                            data in RGB888 colorspace format
     * @return @c 0 on success, otherwise a negative error value
     */
    static int convertY800ToRGB(
            const unsigned char *pInBuffer,
            const ImageData& imageData,
            unsigned char **pOutBuffer);
};

} /* namespace Common */
} /* namespace MediaVision */

#endif /* __IMAGEHELPER_H__ */
