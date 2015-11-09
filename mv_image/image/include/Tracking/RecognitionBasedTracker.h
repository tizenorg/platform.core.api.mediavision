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

#ifndef __IMAGETRACKING_RECOGNITIONBASEDTRACKER_H__
#define __IMAGETRACKING_RECOGNITIONBASEDTRACKER_H__

#include "Tracking/ObjectTracker.h"

#include "Recognition/ImageObject.h"

namespace MediaVision {
namespace Image {
/**
 * @class    FeatureSubstitutionTracker
 * @brief    Tracker uses recognition of target on the entire frame.
 *
 * @since_tizen 3.0
 */
class RecognitionBasedTracker : public ObjectTracker {
public:
	/**
	 * @brief @ref RecognitionBasedTracker constructor.
	 *
	 * @since_tizen 3.0
	 * @param [in] target                          Target of recognition
	 * @param [in] sceneFeaturesExtractingParams   Parameters of feature
	 *                                             extracting from frames
	 * @param [in] recognitionParams               Parameters of recognition
	 */
	RecognitionBasedTracker(
			const ImageObject& target,
			const FeaturesExtractingParams& sceneFeaturesExtractingParams,
			const RecognitionParams& recognitionParams);

	/**
	 * @brief @ref RecognitionBasedTracker destructor
	 *
	 * @since_tizen 3.0
	 */
	virtual ~RecognitionBasedTracker();

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
	ImageObject m_target;

	FeaturesExtractingParams m_sceneFeatureExtractingParams;

	RecognitionParams m_recogParams;
};

} /* Image */
} /* MediaVision */

#endif /* __IMAGETRACKING_RECOGNITIONBASEDTRACKER_H__ */
