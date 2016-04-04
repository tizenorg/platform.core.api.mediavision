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

#ifndef __FACEUTIL_H__
#define __FACEUTIL_H__

#include <opencv/cv.h>

#include "mv_common_c.h"

/**
 * @file FaceUtil.h
 * @brief This file contains the useful functionality for Face module.
 */
namespace MediaVision {
namespace Face {
/**
 * @brief Enumeration of supported learning algorithms.
 *
 * @since_tizen 3.0
 */
enum EXPORT_API FaceRecognitionModelType {
	MEDIA_VISION_FACE_MODEL_TYPE_UNKNOWN = 0,     /**< Unknown algorithm type */
	MEDIA_VISION_FACE_MODEL_TYPE_EIGENFACES = 1,  /**< Eigenfaces algorithm */
	MEDIA_VISION_FACE_MODEL_TYPE_FISHERFACES = 2, /**< Fisherfaces algorithm */
	MEDIA_VISION_FACE_MODEL_TYPE_LBPH = 3         /**< Local Binary Patterns
														Histograms algorithm */
};

/**
 * @brief Contains parameters for face recognition.
 *
 * @since_tizen 3.0
 */
struct EXPORT_API RecognitionParams {
	RecognitionParams(FaceRecognitionModelType algType);

	RecognitionParams();

	FaceRecognitionModelType mRecognitionAlgType;  /**< The type of
														the learning algorithm */
};

/**
 * @brief Converts mv_source_h to cv::Mat class with grayscale type.
 *
 * @since_tizen 3.0
 * @param [in] mvSource    The handle to the image from Media Vision API.
 * @param [out] cvSource   The cv::Mat class, which will be filled.
 * @return @c 0 on success, otherwise a negative error value
 */
EXPORT_API int convertSourceMV2GrayCV(mv_source_h mvSource, cv::Mat& cvSource);

} /* Face */
} /* MediaVision */

#endif /* __FACEUTIL_H__ */
