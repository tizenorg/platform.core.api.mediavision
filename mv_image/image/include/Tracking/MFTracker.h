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

#ifndef __IMAGETRACKING_MFTRACKER_H__
#define __IMAGETRACKING_MFTRACKER_H__

#include "Tracking/ObjectTracker.h"

namespace MediaVision {
namespace Image {
/**
 * @class    MFTracker
 * @brief    Median Flow tracker implementation.
 *
 * @since_tizen 3.0
 */
class MFTracker : public ObjectTracker {
public:
	struct Params {
		/**
		 * @brief TrackerMedianFlow algorithm parameters constructor
		 */
	Params();

	int mPointsInGrid; /**< Square root of number of used keypoints.
								Increase it to trade accurateness for speed.
								Default value is sensible and recommended */

	cv::Size mWindowSize; /**< Size of the search window at each pyramid level
								for Lucas-Kanade optical flow search used for
								tracking */

	int mPyrMaxLevel; /**< Number of pyramid levels for Lucas-Kanade optical
								flow search used for tracking */

		/* TODO: add lifetime*/
		/*time_t mLifetime;*/  /**< Time of tracking without reinforcement. */
    };

	/**
	 * @brief   @ref MFTracker constructor based on tracking algorithm parameters.
	 *
	 * @since_tizen 3.0
	 * @param [in] params   Parameters for objects tracking
	 */
	MFTracker(Params params = Params());

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

	/**
	 * @brief Creates a copy of itself
	 *
	 * @since_tizen 3.0
	 * @return clone
	 */
	virtual cv::Ptr<ObjectTracker> clone() const;

private:
	bool isInited() const;

	bool init(const cv::Mat& image);

	bool update(const cv::Mat& image);

	float getLastConfidence() const;

	cv::Rect_<float> getLastBoundingBox() const;

	bool medianFlowImpl(cv::Mat oldImage, cv::Mat newImage, cv::Rect_<float>& oldBox);

	cv::Rect_<float> vote(
			const std::vector<cv::Point2f>& oldPoints,
			const std::vector<cv::Point2f>& newPoints,
			const cv::Rect_<float>& oldRect,
			cv::Point2f& mD);

	void check_FB(
			std::vector<cv::Mat> newPyramid,
			const std::vector<cv::Point2f>& oldPoints,
			const std::vector<cv::Point2f>& newPoints,
			std::vector<bool>& status);

	void check_NCC(
			const cv::Mat& oldImage,
			const cv::Mat& newImage,
			const std::vector<cv::Point2f>& oldPoints,
			const std::vector<cv::Point2f>& newPoints,
			std::vector<bool>& status);

private:
	bool m_isInit;                /**< Flag is used to determine the model
										initialization */

	Params m_params;              /**< Parameters used during tracking, see
										@ref TrackerMedianFlow::Params */

	cv::TermCriteria m_termcrit;  /**< Terminating criteria for OpenCV
										Lucas–Kanade optical flow algorithm used
										during tracking */

	std::vector<cv::Point2f> m_startLocation; /**< Tracking object start
													location with relative values
													to the bounding box */

	cv::Rect_<float> m_boundingBox;  /**< Tracking object bounding box */

	float m_confidence;              /**< Confidence that object was tracked
											correctly at the last tracking iteration */

	cv::Mat m_image;                 /**< Last image for which tracking was
											performed */

	std::vector<cv::Mat> m_pyramid;  /**< The pyramid had been calculated for
											the previous frame(or when
											initialize the model) */
};

} /* Image */
} /* MediaVision */

#endif /* __IMAGETRACKING_MFTRACKER_H__ */
