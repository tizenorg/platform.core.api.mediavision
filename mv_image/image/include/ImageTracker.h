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

#ifndef __IMAGETRACKER_H__
#define __IMAGETRACKER_H__

#include "ImageConfig.h"

#include <opencv/cv.h>

/**
 * @file  ImageTracker.h
 * @brief This file contains functionality for image object tracking.
 */

namespace MediaVision {
namespace Image {
class ImageRecognizer;
class ImageTrackingModel;
/**
 * @class    ImageTracker
 * @brief    This class contains functionality for image object tracking.
 *
 * @since_tizen 3.0
 */
class ImageTracker {
private:
	struct RecognitionInfo {
		cv::Mat mFrame;

		RecognitionParams mRecognitionParams;

		FeaturesExtractingParams mSceneFeaturesExtractingParams;

		ImageTrackingModel *mpTarget;
	};

	static void *recognitionThreadFunc(void *recognitionInfo);

public:
	/**
	 * @brief   @ref ImageTracker constructor based on tracking algorithm
	 *          parameters.
	 *
	 * @since_tizen 3.0
	 * @param [in] trackingParams   Parameters for image objects tracking
	 */
	ImageTracker(const TrackingParams& trackingParams);

	/**
	 * @brief Tracks the @a target for the video stream consisting of frames.
	 *
	 * @since_tizen 3.0
	 * @remarks Call this function alternately for each frame
	 * @param [in]      frame    Current frame of the video stream
	 * @param [in,out]  target   @ref ImageTrackingModel, which will be tracked
	 */
	void track(const cv::Mat& frame, ImageTrackingModel& target);

private:
	void trackDetectedObject(
			const cv::Mat& frame,
			ImageTrackingModel& target);

	void trackUndetectedObject(
			const cv::Mat& frame,
			ImageTrackingModel& target);

	cv::Rect computeExpectedArea(
			const ImageTrackingModel& target,
			const cv::Size& frameSize);

private:
	TrackingParams m_trackingParams;
};

} /* Image */
} /* MediaVision */

#endif /* __IMAGETRACKER_H__ */
