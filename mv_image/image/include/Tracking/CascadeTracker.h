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

#ifndef __IMAGETRACKING_CASCADETRACKER_H__
#define __IMAGETRACKING_CASCADETRACKER_H__

#include "Tracking/ObjectTracker.h"

#include <opencv2/core/core.hpp>

#include <set>

namespace MediaVision {
namespace Image {
/**
 * @class    CascadeTracker
 * @brief    Tracker is based on the another trackers and combines the results.
 *
 * @since_tizen 3.0
 */
class CascadeTracker : public ObjectTracker {
public:
	/**
	 * @brief @ref CascadeTracker default constructor
	 *
	 * @since_tizen 3.0
	 * @param   [in] minimumArea   Minimum detected area
	 */
	CascadeTracker(float minimumArea = 2000);

	/**
	 * @brief @ref CascadeTracker copy constructor.
	 *
	 * @since_tizen 3.0
	 * @param   [in] copy @ref CascadeTracker which will be copied
	 */
	CascadeTracker(const CascadeTracker& copy);

	/**
	 * @brief @ref CascadeTracker destructor
	 *
	 * @since_tizen 3.0
	 */
	virtual ~CascadeTracker();

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

	/**
	 * @brief Assignment operator.
	 *
	 * @since_tizen 3.0
	 * @param [in] copy @ref CascadeTracker which will be copied
	 * @return itself
	 */
	virtual CascadeTracker& operator=(const CascadeTracker& copy);

	bool enableTracker(cv::Ptr<ObjectTracker> tracker, float priority);

	bool disableTracker(cv::Ptr<ObjectTracker> tracker);

private:
	void internalReinforcement();

	bool mergeResults(std::vector<cv::Point>& result) const;

private:
	struct TrackerInfo {
		TrackerInfo(cv::Ptr<ObjectTracker>, float);

		bool operator<(const TrackerInfo&) const;

		bool operator==(const TrackerInfo&) const;

		bool operator!=(const TrackerInfo&) const;

		cv::Ptr<ObjectTracker> mTracker;

		float mPriority;

		mutable std::vector<cv::Point> mResult;
	};

	/* don't use m_trackers.find() because
	operator==() and operator<() are independent
	TODO: Fix it with aggregator or something like that */
	std::set<TrackerInfo> m_trackers;

	float m_minimumArea;
};

} /* Image */
} /* MediaVision */

#endif /* __IMAGETRACKING_CASCADETRACKER_H__ */
