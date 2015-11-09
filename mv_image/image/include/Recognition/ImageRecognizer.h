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

#ifndef __IMAGERECOGNIZER_H__
#define __IMAGERECOGNIZER_H__

#include "ImageMathUtil.h"
#include "ImageConfig.h"

#include "Recognition/ImageObject.h"

#include <opencv/cv.h>

/**
 * @file  ImageRecognizer.h
 * @brief This file contains functionality for image object recognition.
 */

namespace MediaVision {
namespace Image {
/**
 * @class    ImageRecognizer
 * @brief    This class contains functionality for image object recognition.
 *
 * @since_tizen 3.0
 */
class ImageRecognizer {
public:
	/**
	 * @brief   @ref ImageRecognizer constructor based on the scene @ref ImageObject.
	 *
	 * @since_tizen 3.0
	 * @param [in] scene   The scene for which the objects will be recognized by
	 *                     calling method recognize()
	 */
	ImageRecognizer(const ImageObject& scene);

	/**
	 * @brief   @ref ImageRecognizer destructor.
	 *
	 * @since_tizen 3.0
	 */
	virtual ~ImageRecognizer();

	/**
	 * @brief Recognizes the @a target on the scene.
	 *
	 * @since_tizen 3.0
	 * @param [in]  target    @ref ImageObject, which will be recognized
	 * @param [in]  params    Recognition parameters
	 * @param [out] contour   The result contour of @a target object on the
	 *                        scene
	 * @param [out] ignoreFactor Scaling factor of area near the contour
	 *              of object which will be ignored
	 * @return true if object is found on the scene, otherwise return false
	 */
	bool recognize(
			const ImageObject& target,
			const RecognitionParams& params,
			std::vector<cv::Point2f>& contour,
			float ignoreFactor = 0.f) const;

private:
	ImageRecognizer();

	bool findHomophraphyMatrix(
			const ImageObject& target,
			const RecognitionParams& params,
			cv::Mat& homophraphyMatrix,
			float ignoreFactor) const;

	size_t matchesSelection(
			std::vector<cv::DMatch>& examples,
			unsigned int filterAmount, unsigned int allowableError) const;

	float computeLinearSupportElement(
			const std::vector<cv::DMatch>& examples,
			int requiredNumber, int leftLimit, int rightLimit) const;

	static bool isPossibleQuadrangleCorners(
			const cv::Point2f corners[NumberOfQuadrangleCorners]);

private:
	/* TODO: Replace to cv::Ptr<ImageObject> */
	ImageObject m_scene;

	cv::BFMatcher m_matcher;
};

} /* Image */
} /* MediaVision */

#endif /* __IMAGERECOGNIZER_H__ */
