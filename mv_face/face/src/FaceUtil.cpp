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

#include "FaceUtil.h"

#include "mv_private.h"

#include <opencv2/imgproc/types_c.h>
#include <opencv2/highgui/highgui.hpp>

namespace MediaVision
{
namespace Face
{

RecognitionParams::RecognitionParams(FaceRecognitionModelType algType) :
        mRecognitionAlgType(algType)
{
    ; /* NULL */
}

RecognitionParams::RecognitionParams() :
        mRecognitionAlgType(MEDIA_VISION_FACE_MODEL_TYPE_LBPH)
{
    ; /* NULL */
}

int convertSourceMV2GrayCV(mv_source_h mvSource, cv::Mat& cvSource)
{
    MEDIA_VISION_INSTANCE_CHECK(mvSource);

    int depth = CV_8U; // Default depth. 1 byte for channel.
    unsigned int channelsNumber = 0;
    unsigned int width = 0, height = 0;
    unsigned int bufferSize = 0;
    unsigned char *buffer = NULL;

    mv_colorspace_e colorspace = MEDIA_VISION_COLORSPACE_INVALID;

    MEDIA_VISION_ASSERT(mv_source_get_width(mvSource, &width),
            "Failed to get the width.");
    MEDIA_VISION_ASSERT(mv_source_get_height(mvSource, &height),
            "Failed to get the height.");
    MEDIA_VISION_ASSERT(mv_source_get_colorspace(mvSource, &colorspace),
            "Failed to get the colorspace.");
    MEDIA_VISION_ASSERT(mv_source_get_buffer(mvSource, &buffer, &bufferSize),
            "Failed to get the buffer size.");

    int conversionType = -1; // Type of conversion from given colorspace to gray
    switch(colorspace)
    {
        case MEDIA_VISION_COLORSPACE_INVALID:
            LOGE("Error: mv_source has invalid colorspace.");
            return MEDIA_VISION_ERROR_INVALID_PARAMETER;
        case MEDIA_VISION_COLORSPACE_Y800:
            channelsNumber = 1;
            // Without convertion
            break;
        case MEDIA_VISION_COLORSPACE_I420:
            channelsNumber = 1;
            height *= 1.5;
            conversionType = CV_YUV2GRAY_I420;
            break;
        case MEDIA_VISION_COLORSPACE_NV12:
            channelsNumber = 1;
            height *= 1.5;
            conversionType = CV_YUV2GRAY_NV12;
            break;
        case MEDIA_VISION_COLORSPACE_YV12:
            channelsNumber = 1;
            height *= 1.5;
            conversionType = CV_YUV2GRAY_YV12;
            break;
        case MEDIA_VISION_COLORSPACE_NV21:
            channelsNumber = 1;
            height *= 1.5;
            conversionType = CV_YUV2GRAY_NV21;
            break;
        case MEDIA_VISION_COLORSPACE_YUYV:
            channelsNumber = 2;
            conversionType = CV_YUV2GRAY_YUYV;
            break;
        case MEDIA_VISION_COLORSPACE_UYVY:
            channelsNumber = 2;
            conversionType = CV_YUV2GRAY_UYVY;
            break;
        case MEDIA_VISION_COLORSPACE_422P:
            channelsNumber = 2;
            conversionType = CV_YUV2GRAY_Y422;
            break;
        case MEDIA_VISION_COLORSPACE_RGB565:
            channelsNumber = 2;
            conversionType = CV_BGR5652GRAY;
            break;
        case MEDIA_VISION_COLORSPACE_RGB888:
            channelsNumber = 3;
            conversionType = CV_RGB2GRAY;
            break;
        case MEDIA_VISION_COLORSPACE_RGBA:
            channelsNumber = 4;
            conversionType = CV_RGBA2GRAY;
            break;
        default:
            LOGE("Error: mv_source has unsupported colorspace.");
            return MEDIA_VISION_ERROR_NOT_SUPPORTED_FORMAT;
    }

    if (conversionType == -1) // Without conversion
    {
        cvSource = cv::Mat(cv::Size(width, height),
                CV_MAKETYPE(depth, channelsNumber), buffer).clone();
    }
    else // Conversion
    {
        // Class for representation the given image as cv::Mat before conversion
        cv::Mat origin(cv::Size(width, height),
                CV_MAKETYPE(depth, channelsNumber), buffer);
        cv::cvtColor(origin, cvSource, conversionType);
    }

    return MEDIA_VISION_ERROR_NONE;
}

} /* Face */
} /* MediaVision */
