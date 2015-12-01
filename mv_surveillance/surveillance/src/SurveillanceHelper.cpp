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

#include "SurveillanceHelper.h"

#include <mv_private.h>

#include "opencv2/highgui/highgui.hpp"

namespace mediavision {
namespace surveillance {

int SurveillanceHelper::convertSourceMV2GrayCV(mv_source_h mvSource, cv::Mat& cvSource)
{
	MEDIA_VISION_INSTANCE_CHECK(mvSource);

	int depth = CV_8U; /* Default depth. 1 byte per channel. */
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

	int conversionType = -1; /* Type of conversion from given colorspace to gray */
	switch(colorspace) {
	case MEDIA_VISION_COLORSPACE_INVALID:
		LOGE("Error: mv_source has invalid colorspace.");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	case MEDIA_VISION_COLORSPACE_Y800:
		channelsNumber = 1;
		/* Without convertion */
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

	if (conversionType == -1) { /* Without conversion */
		cvSource = cv::Mat(cv::Size(width, height),
				CV_MAKETYPE(depth, channelsNumber), buffer).clone();
	} else { /* Conversion */
		/* Class for representation the given image as cv::Mat before conversion */
		cv::Mat origin(cv::Size(width, height),
				CV_MAKETYPE(depth, channelsNumber), buffer);
		cv::cvtColor(origin, cvSource, conversionType);
	}

	return MEDIA_VISION_ERROR_NONE;
}

#ifdef ENABLE_NEON
int SurveillanceHelper::convertSourceMVRGB2GrayCVNeon(
		mv_source_h mvSource,
		cv::Mat& cvSource)
{
	MEDIA_VISION_INSTANCE_CHECK(mvSource);

	const int depth = CV_8U; /* Default depth. 1 byte per channel. */
	unsigned int width = 0, height = 0;
	unsigned int bufferSize = 0;
	unsigned char *src = NULL;

	mv_colorspace_e colorspace = MEDIA_VISION_COLORSPACE_INVALID;

	MEDIA_VISION_ASSERT(mv_source_get_width(mvSource, &width),
						"Failed to get the width.");
	MEDIA_VISION_ASSERT(mv_source_get_height(mvSource, &height),
						"Failed to get the height.");
	MEDIA_VISION_ASSERT(mv_source_get_colorspace(mvSource, &colorspace),
						"Failed to get the colorspace.");
	MEDIA_VISION_ASSERT(mv_source_get_buffer(mvSource, &src, &bufferSize),
						"Failed to get the buffer size.");

	if (colorspace != MEDIA_VISION_COLORSPACE_RGB888) {
		LOGE("Error: mv_source has unsupported colorspace.");
		return MEDIA_VISION_ERROR_NOT_SUPPORTED_FORMAT;
	}

	cvSource = cv::Mat(cv::Size(width, height), CV_MAKETYPE(depth, 1));
	const unsigned int cvSourceSize = width * height;

#if defined(__aarch64__)
	asm volatile ("lsr  %2, %2, #3     \n"
		"# channel multimpliers:       \n"
		"mov         w4, #28           \n"
		"mov         w5, #151          \n"
		"mov         w6, #77           \n"
		"dup         v3.8b, w4         \n"
		"dup         v4.8b, w5         \n"
		"dup         v5.8b, w6         \n"
		".loop:                        \n"
		"# load 8 pixels:              \n"
		"ld3         {v0.8b,v1.8b,v2.8b}, [%0],#24 \n"
		"# conversion:                             \n"
		"umull       v7.8h, v0.8b, v3.8b           \n"
		"umlal       v7.8h, v1.8b, v4.8b           \n"
		"umlal       v7.8h, v2.8b, v5.8b           \n"
		"# shift and store:                        \n"
		"shrn        v6.8b, v7.8h, #8              \n"
		"st1         {v6.8b}, [%1],#8              \n"
		"subs        %2, %2, #1                    \n"
		"bne         .loop             \n"::"r" (src), "r" (cvSource.data), "r" (cvSourceSize)
		:"memory", "w4", "w5", "w6");
#else
	asm volatile ("lsr  %2, %2, #3     \n"
		"# channel multimpliers:       \n"
		"mov         r4, #77           \n"
		"mov         r5, #151          \n"
		"mov         r6, #28           \n"
		"vdup.8      d3, r4            \n"
		"vdup.8      d4, r5            \n"
		"vdup.8      d5, r6            \n"
		".loop:                        \n"
		"# load 8 pixels:              \n"
		"vld3.8      {d0-d2}, [%0]!    \n"
		"# conversion:                 \n"
		"vmull.u8    q7, d0, d3        \n"
		"vmlal.u8    q7, d1, d4        \n"
		"vmlal.u8    q7, d2, d5        \n"
		"# shift and store:            \n"
		"vshrn.u16   d6, q7, #8        \n"
		"vst1.8      {d6}, [%1]!       \n"
		"subs        %2, %2, #1        \n"
		"bne         .loop             \n"::"r" (src), "r" (cvSource.data), "r" (cvSourceSize)
		:"memory", "r4", "r5", "r6");
#endif

	return MEDIA_VISION_ERROR_NONE;
}
#endif

} /* surveillance */
} /* mediavision */
