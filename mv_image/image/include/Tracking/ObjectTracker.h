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

#ifndef __IMAGETRACKING_OBJECTTRACKER_H__
#define __IMAGETRACKING_OBJECTTRACKER_H__

#include <opencv2/core/core.hpp>

namespace MediaVision {
namespace Image {
/**
 * @class    ObjectTracker
 * @brief    Basic object tracker.
 *
 * @since_tizen 3.0
 */
class EXPORT_API ObjectTracker {
public:
	/**
	 * @brief @ref ObjectTracker destructor
	 *
	 * @since_tizen 3.0
	 */
	virtual ~ObjectTracker();

	/**
	 * @brief Tracks the target for the video stream consisting of frames.
	 *
	 * @since_tizen 3.0
	 * @remarks Call this function alternately for each frame
	 * @param [in]   frame    Current frame of the video stream
	 * @param [out]  result   Result contour
	 * @return true if object is tracked, otherwise return false
	 */
	virtual bool track(const cv::Mat& frame, std::vector<cv::Point>& result) = 0;

	/**
	 * @brief Provides the current location of a target.
	 *
	 * @since_tizen 3.0
	 * @param [in] location  Current location of a target
	 */
	virtual void reinforcement(const std::vector<cv::Point>& location) = 0;

	/*
	 * @brief Creates a copy of itself
	 *
	 * @since_tizen 3.0
	 * @return clone
	 */
	virtual cv::Ptr<ObjectTracker> clone() const = 0;

private:
	/**
	 * @brief Assignment operator for the base class @ref ObjectTracker.
	 *
	 * @since_tizen 3.0
	 * @param [in] copy @ref ObjectTracker which will be copied
	 * @return itself
	 */
	ObjectTracker& operator=(const ObjectTracker& copy);
};

} /* Image */
} /* MediaVision */

#endif /* __IMAGETRACKING_OBJECTTRACKER_H__ */
