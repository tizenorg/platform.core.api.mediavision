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

#ifndef __IMAGETRACKING_FEATURESUBSTITUTIONTRACKER_H__
#define __IMAGETRACKING_FEATURESUBSTITUTIONTRACKER_H__

#include "Tracking/ObjectTracker.h"

#include "Recognition/ImageObject.h"

namespace MediaVision {
namespace Image {
/**
 * @class    FeatureSubstitutionTracker
 * @brief    Tracker uses feature substitution.
 *
 * @since_tizen 3.0
 */
class EXPORT_API FeatureSubstitutionTracker : public ObjectTracker {
public:
	/**
	 * @brief @ref FeatureSubstitutionTracker constructor.
	 *
	 * @since_tizen 3.0
	 * @param [in] featuresExtractingParams   Parameters of feature extracting
	 * @param [in] recognitionParams          Parameters of recognition
	 * @param [in] expectedOffset             Expected offset
	 * @param [in] sceneScalingFactor         Additional area around target
	 *                                        contour on the frame which will
	 *                                        be use for recognition
	 *                                        (recommended value is 1 - 1.5)
	 * @param [in] objectScalingFactor        Additional area near the contour
	 *                                        of object which will be ignored by
	 *                                        recognition
	 *                                        (recommended value is 0.5 - 1)
	 */
	FeatureSubstitutionTracker(
			const FeaturesExtractingParams& featuresExtractingParams,
			const RecognitionParams& recognitionParams,
			float expectedOffset,
			float sceneScalingFactor = 1.2f,
			float objectScalingFactor = 0.85f);

	/**
	 * @brief Tracks the target for the video stream consisting of frames.
	 *
	 * @since_tizen 3.0
	 * @remarks Call this function alternately for each frame
	 * @param [in]   frame    Current frame of the video stream
	 * @param [out]  result   Result contour
	 * @return true if object is tracked, otherwise return false
	 */
	virtual bool track(const cv::Mat& frame, std::vector<cv::Point>& result);

	/**
	 * @brief Provides the current location of a target.
	 *
	 * @since_tizen 3.0
	 * @param [in] location  Current location of a target
	 */
	virtual void reinforcement(const std::vector<cv::Point>& location);

	/*
	 * @brief Creates a copy of itself
	 *
	 * @since_tizen 3.0
	 * @return clone
	 */
	virtual cv::Ptr<ObjectTracker> clone() const;

private:
	std::vector<cv::Point2f> computeExpectedArea();

private:
	bool m_isInit;

	cv::Ptr<ImageObject> m_target;

	std::vector<cv::Point> m_location;

	FeaturesExtractingParams m_featureExtractingParams;

	RecognitionParams m_recogParams;

	float m_expectedOffset;

	float m_sceneScalingFactor;

	float m_objectScalingFactor;
};

} /* Image */
} /* MediaVision */

#endif /* __IMAGETRACKING_FEATURESUBSTITUTIONTRACKER_H__ */
