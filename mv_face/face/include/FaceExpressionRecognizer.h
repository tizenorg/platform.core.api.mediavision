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

#ifndef __FACEEXPRESSIONRECOGNIZER_H__
#define __FACEEXPRESSIONRECOGNIZER_H__

#include "mv_common_c.h"
#include "mv_face_open.h"

#include <string>

namespace cv {
    class Mat;
}

/**
 * @file FaceExpressionRecognizer.h
 * @brief This file contains the FaceExpressionRecognizer class which implements
 *        the facial expressions recognition functionality.
 */

namespace MediaVision {
namespace Face {
/**
 * @brief Face expression recognition configuration.
 *
 * @since_tizen 3.0
 */
struct FaceRecognizerConfig {
    FaceRecognizerConfig();
    std::string mHaarcascadeFilepath;
};

/**
 * @class FaceExpressionRecognizer
 * @brief The FaceExpressionRecognizer implements the facial expressions
 *        recognition functionality.
 *
 * @since_tizen 3.0
 */
class FaceExpressionRecognizer {
public:
	/**
	 * @brief Recognizes facial expression on the image with known face location.
	 *
	 * @since_tizen 3.0
	 * @param [in]     grayImage         The grayscale image with face
	 * @param [in]     faceLocation      The location of the face on the @a image
	 * @param [out]    faceExpression    Expression recognized for the face at
	 *                                   @a faceLocation
	 * @param [in]     config            The configuration will be used for
	 *                                   facial expression recognition
	 *
	 * @see MediaVision::Face::FaceRecognizerConfig
	 */
	static int recognizeFaceExpression(
					const cv::Mat& grayImage,
					const mv_rectangle_s& faceLocation,
					mv_face_facial_expression_e *faceExpression,
					const FaceRecognizerConfig& config = FaceRecognizerConfig());
};

} /* Face */
} /* MediaVision */

#endif /* __FACEEXPRESSIONRECOGNIZER_H__ */
