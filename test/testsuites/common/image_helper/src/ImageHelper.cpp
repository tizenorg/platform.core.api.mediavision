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

#include "ImageHelper.h"
#include "mv_private.h"

#include <vector>

#include <cstring>

#include <setjmp.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

/**
 * @file   ImageHelper.cpp
 * @brief  The ImageHelper class methods implementation.
 */

namespace MediaVision
{
namespace Common
{

namespace
{

static const int OpenCVChannels = 3;
static const mv_colorspace_e OpenCVColor = MEDIA_VISION_COLORSPACE_RGB888;
static const int QuadrangleVertices = 4;

std::vector<std::string> getJPGExtensions()
{
    std::vector<std::string> extensions;
    extensions.push_back(".jpg");
    extensions.push_back(".jpe");
    extensions.push_back(".jpeg");
    return extensions;
}

} /* anonymous namespace */

int ImageHelper::loadImageToBuffer(
        const char *filePath,
        unsigned char **pDataBuffer,
        unsigned long *pBufferSize,
        ImageData *pImageData)
{
    if (filePath == NULL || pDataBuffer == NULL ||
        pBufferSize == NULL || pImageData == NULL)
    {
        return MEDIA_VISION_ERROR_INVALID_PARAMETER;
    }

    cv::Mat image;
    image = cv::imread(filePath);

    if (!image.data)
    {
        return MEDIA_VISION_ERROR_INVALID_PARAMETER;
    }

    cv::cvtColor(image, image, CV_BGR2RGB);

    *pBufferSize = image.total() * image.elemSize();
    (*pDataBuffer) = new unsigned char[*pBufferSize];
    std::memcpy(*pDataBuffer, image.data, *pBufferSize);

    pImageData->imageWidth = image.cols;
    pImageData->imageHeight = image.rows;
    pImageData->imageColorspace = OpenCVColor;

    return MEDIA_VISION_ERROR_NONE;
}

int ImageHelper::saveImageFromBuffer(
        const char *filePath,
        unsigned char *pDataBuffer,
        const ImageData& imageData,
        int quality)
{
    if (filePath == NULL || pDataBuffer == NULL)
    {
        return MEDIA_VISION_ERROR_INVALID_PARAMETER;
    }

    static const std::string defaultFilePath = "out";
    static const std::vector<std::string> jpgExtensions = getJPGExtensions();

    bool rightExtensionFlag = false;

    std::string resultFilePath(filePath);
    if (resultFilePath.empty())
    {
        resultFilePath = defaultFilePath;
    } else {
        for (size_t extNum = 0; extNum < jpgExtensions.size(); ++extNum)
        {
            if (resultFilePath.size() >= jpgExtensions[extNum].size())
            {
                std::string givenExtension = resultFilePath.substr(
                        resultFilePath.length() - jpgExtensions[extNum].size(),
                        jpgExtensions[extNum].size());

                std::transform(
                        givenExtension.begin(), givenExtension.end(),
                        givenExtension.begin(), ::tolower);

                if (givenExtension == jpgExtensions[extNum])
                {
                    rightExtensionFlag = true;
                    break;
                }
            }
        }
    }
    if (!rightExtensionFlag)
    {
        resultFilePath += jpgExtensions[0];
    }

    if (quality <= 0 || quality > 100)
    {
        quality = 100;
    }

    unsigned int width = imageData.imageWidth;
    unsigned int height = imageData.imageHeight;

    int conversionType = -1; // Type of conversion from given colorspace to BGR
    unsigned int channelsNumber = 0;
    switch (imageData.imageColorspace)
    {
        case MEDIA_VISION_COLORSPACE_INVALID:
            return MEDIA_VISION_ERROR_INVALID_PARAMETER;
        case MEDIA_VISION_COLORSPACE_Y800:
            channelsNumber = 1;
            conversionType = CV_GRAY2BGR;
            break;
        case MEDIA_VISION_COLORSPACE_I420:
            channelsNumber = 1;
            height *= 1.5;
            conversionType = CV_YUV2BGR_I420;
            break;
        case MEDIA_VISION_COLORSPACE_NV12:
            channelsNumber = 1;
            height *= 1.5;
            conversionType = CV_YUV2BGR_NV12;
            break;
        case MEDIA_VISION_COLORSPACE_YV12:
            channelsNumber = 1;
            height *= 1.5;
            conversionType = CV_YUV2BGR_YV12;
            break;
        case MEDIA_VISION_COLORSPACE_NV21:
            channelsNumber = 1;
            height *= 1.5;
            conversionType = CV_YUV2BGR_NV21;
            break;
        case MEDIA_VISION_COLORSPACE_YUYV:
            channelsNumber = 2;
            conversionType = CV_YUV2BGR_YUYV;
            break;
        case MEDIA_VISION_COLORSPACE_UYVY:
            channelsNumber = 2;
            conversionType = CV_YUV2BGR_UYVY;
            break;
        case MEDIA_VISION_COLORSPACE_422P:
            channelsNumber = 2;
            conversionType = CV_YUV2BGR_Y422;
            break;
        case MEDIA_VISION_COLORSPACE_RGB565:
            channelsNumber = 2;
            conversionType = CV_BGR5652BGR;
            break;
        case MEDIA_VISION_COLORSPACE_RGB888:
            channelsNumber = 3;
            conversionType = CV_RGB2BGR;
            break;
        case MEDIA_VISION_COLORSPACE_RGBA:
            channelsNumber = 4;
            conversionType = CV_RGBA2BGR;
            break;
        default:
            return MEDIA_VISION_ERROR_NOT_SUPPORTED_FORMAT;
    }

    const int depth = CV_8U;
    cv::Mat cvImage(cv::Size(width, height),
                    CV_MAKETYPE(depth, channelsNumber), pDataBuffer);
    cv::Mat cvBGRImage;
    cv::cvtColor(cvImage, cvBGRImage, conversionType);

    std::vector<int> compression_params(2);
    compression_params[0] = CV_IMWRITE_JPEG_QUALITY;
    compression_params[1] = quality;

    if (!cv::imwrite(resultFilePath, cvBGRImage, compression_params))
    {
        return MEDIA_VISION_ERROR_INVALID_OPERATION;
    }

    return MEDIA_VISION_ERROR_NONE;
}

int ImageHelper::destroyLoadedBuffer(unsigned char *pDataBuffer)
{
	if (!pDataBuffer) {
		return MEDIA_VISION_ERROR_NONE;
	}

	delete [] pDataBuffer;
	pDataBuffer = NULL;

	return MEDIA_VISION_ERROR_NONE;
}

int ImageHelper::drawRectangleOnBuffer(
        int topLeftVertexX,
        int topLeftVertexY,
        int bottomRightVertexX,
        int bottomRightVertexY,
        int thickness,
        const cv::Scalar& color,
        const ImageData& imageData,
        unsigned char *pDataBuffer)
{
    if (NULL == pDataBuffer)
    {
        return MEDIA_VISION_ERROR_INVALID_PARAMETER;
    }

    cv::Mat cvImage(imageData.imageHeight, imageData.imageWidth, CV_8UC(OpenCVChannels), pDataBuffer);
    cv::rectangle(
            cvImage,
            cv::Point(topLeftVertexX, topLeftVertexY),
            cv::Point(bottomRightVertexX, bottomRightVertexY),
            color,
            thickness);

    return MEDIA_VISION_ERROR_NONE;
}

int ImageHelper::drawQuadrangleOnBuffer(
            mv_quadrangle_s location,
            int thickness,
            const cv::Scalar& color,
            const ImageData& imageData,
            unsigned char *pDataBuffer)
{
    if (NULL == pDataBuffer)
    {
        return MEDIA_VISION_ERROR_INVALID_PARAMETER;
    }

    cv::Mat cvImage(imageData.imageHeight, imageData.imageWidth, CV_8UC(OpenCVChannels), pDataBuffer);
    for (int i = 0; i < QuadrangleVertices; ++i)
    {
        cv::line(
                cvImage,
                cv::Point(location.points[i].x, location.points[i].y),
                cv::Point(location.points[(i + 1) % QuadrangleVertices].x,
                        location.points[(i + 1) % QuadrangleVertices].y),
                color,
                thickness);
    }
    return MEDIA_VISION_ERROR_NONE;
}

int ImageHelper::convertBufferToRGB888(
        const unsigned char *pInBuffer,
        const ImageData& imageData,
        unsigned char **pOutBuffer)
{
    // todo: support more colorspaces:
    switch (imageData.imageColorspace)
    {
        case MEDIA_VISION_COLORSPACE_Y800:
            return convertY800ToRGB(pInBuffer, imageData, pOutBuffer);
        case MEDIA_VISION_COLORSPACE_RGB888:
        {
            int numberOfComponents = 0;
            getNumberOfComponents(
                    MEDIA_VISION_COLORSPACE_RGB888, &numberOfComponents);
            const int dataSize =
                    imageData.imageHeight * imageData.imageWidth * numberOfComponents;
            (*pOutBuffer) = new unsigned char[dataSize];
            std::memcpy(*pOutBuffer, pInBuffer, dataSize);
            return MEDIA_VISION_ERROR_NONE;
        }
        default:
            return MEDIA_VISION_ERROR_NOT_SUPPORTED;
    }

    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

int ImageHelper::getNumberOfComponents(
        mv_colorspace_e colorspace,
        int *pComponentsNumber)
{
    // todo: support more colorspaces:
    switch (colorspace)
    {
        case MEDIA_VISION_COLORSPACE_Y800:
            (*pComponentsNumber) = 1;
            return MEDIA_VISION_ERROR_NONE;
        case MEDIA_VISION_COLORSPACE_RGB888:
            (*pComponentsNumber) = 3;
            return MEDIA_VISION_ERROR_NONE;
        default:
            return MEDIA_VISION_ERROR_NOT_SUPPORTED;
    }

    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

int ImageHelper::convertY800ToRGB(
        const unsigned char *pInBuffer,
        const ImageData& imageData,
        unsigned char **pOutBuffer)
{
    if (imageData.imageColorspace != MEDIA_VISION_COLORSPACE_Y800)
    {
        return MEDIA_VISION_ERROR_INVALID_PARAMETER;
    }
    const int inDataSize = imageData.imageHeight * imageData.imageWidth;
    int numberOfComponents = 0;
    getNumberOfComponents(MEDIA_VISION_COLORSPACE_RGB888, &numberOfComponents);
    const int dataSize = inDataSize * numberOfComponents;
    (*pOutBuffer) = new unsigned char[dataSize];
    for (int i = 0; i < inDataSize; ++i)
    {
        int pixelStartIndex = i*numberOfComponents;
        (*pOutBuffer)[pixelStartIndex]   = pInBuffer[i];
        (*pOutBuffer)[pixelStartIndex+1] = pInBuffer[i];
        (*pOutBuffer)[pixelStartIndex+2] = pInBuffer[i];
    }
    return MEDIA_VISION_ERROR_NONE;
}

} /* namespace Common */
} /* namespace MediaVision */
