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

#ifndef __IMAGETRACKING_ASYNCTRACKER_H__
#define __IMAGETRACKING_ASYNCTRACKER_H__

#include "Tracking/ObjectTracker.h"

#include <pthread.h>

namespace MediaVision {
namespace Image {
/**
 * @class    AsyncTracker
 * @brief    Tracker is based on the another tracker and extends the
 *           functionality for asynchronous use
 *
 * @since_tizen 3.0
 */
class EXPORT_API AsyncTracker : public ObjectTracker {
public:
	/**
	 * @brief @ref AsyncTracker copy constructor.
	 *
	 * @since_tizen 3.0
	 * @param   [in] copy @ref AsyncTracker which will be copied
	 */
	AsyncTracker(const AsyncTracker& copy);

	/**
	 * @brief @ref AsyncTracker constructor based on the another tracker.
	 *
	 * @since_tizen 3.0
	 * @remarks You guarantee that frame will not be changed while tracking is
	 *          working if @a copyingPolicy value equal false.
	 * @param   [in] baseTracker    Tracker which will be aggregated
	 * @param   [in] copyingPolicy  Flag that determines whether the frame is
	 *                              copied inside @ref track() function
	 */
	AsyncTracker(cv::Ptr<ObjectTracker> baseTracker, bool copyingPolicy = true);

	/**
	 * @brief @ref AsyncTracker destructor
	 *
	 * @since_tizen 3.0
	 */
	virtual ~AsyncTracker();

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

	bool wait();

	bool isRun();

	bool isUpdated(std::vector<cv::Point>& result);

	bool getResult(std::vector<cv::Point>& location);

private:
	AsyncTracker& operator= (const AsyncTracker& copy);

	bool baseTrack(std::vector<cv::Point>& result);

	static void *asyncTrack(void *data);

private:
	cv::Ptr<ObjectTracker> m_baseTracker;

	cv::Mat m_frame;

	std::vector<cv::Point> m_result;

	bool m_isRun;

	bool m_isUpdated;

	bool m_copyingPolicy;

	pthread_t m_thread;

	mutable pthread_mutex_t m_globalGuard;

	mutable pthread_spinlock_t m_resultGuard;

	mutable pthread_spinlock_t m_isRunGuard;

	mutable pthread_spinlock_t m_isUpdatedGuard;
};

} /* Image */
} /* MediaVision */

#endif /* __IMAGETRACKING_ASYNCTRACKER_H__ */
