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

#include "Tracking/AsyncTracker.h"

namespace MediaVision {
namespace Image {
AsyncTracker::AsyncTracker(const AsyncTracker& copy) :
		m_baseTracker(copy.m_baseTracker.obj->clone()),
		m_result(copy.m_result),
		m_isRun(false),
		m_isUpdated(copy.m_isUpdated),
		m_copyingPolicy(copy.m_copyingPolicy),
		m_thread(0)
{
	pthread_mutex_init(&m_globalGuard, NULL);
	pthread_spin_init(&m_resultGuard, PTHREAD_PROCESS_SHARED);
	pthread_spin_init(&m_isRunGuard, PTHREAD_PROCESS_SHARED);
	pthread_spin_init(&m_isUpdatedGuard, PTHREAD_PROCESS_SHARED);
}

AsyncTracker::AsyncTracker(
		cv::Ptr<ObjectTracker> baseTracker,
		bool copyingPolicy) :
		m_baseTracker(baseTracker),
		m_result(),
		m_isRun(false),
		m_isUpdated(false),
		m_copyingPolicy(copyingPolicy),
		m_thread(0)
{
	pthread_mutex_init(&m_globalGuard, NULL);
	pthread_spin_init(&m_resultGuard, PTHREAD_PROCESS_SHARED);
	pthread_spin_init(&m_isRunGuard, PTHREAD_PROCESS_SHARED);
	pthread_spin_init(&m_isUpdatedGuard, PTHREAD_PROCESS_SHARED);
}

AsyncTracker::~AsyncTracker()
{
	if(isRun())
		pthread_join(m_thread, NULL);

	pthread_mutex_destroy(&m_globalGuard);
	pthread_spin_destroy(&m_resultGuard);
	pthread_spin_destroy(&m_isRunGuard);
	pthread_spin_destroy(&m_isUpdatedGuard);
}

bool AsyncTracker::track(
		const cv::Mat& frame,
		std::vector<cv::Point>& result)
{
	while (pthread_mutex_trylock(&m_globalGuard) != 0) {
		return getResult(result);
	}

	pthread_spin_lock(&m_isRunGuard);
	m_isRun = true;
	pthread_spin_unlock(&m_isRunGuard);

	if (m_copyingPolicy)
		m_frame = frame.clone();
	else
		m_frame = frame;

	const int err = pthread_create(&m_thread, NULL, asyncTrack, this);

	if (0 == err) {
		pthread_join(m_thread, NULL);
		return getResult(result);
	}

	pthread_spin_lock(&m_isRunGuard);
	m_isRun = false;
	pthread_spin_unlock(&m_isRunGuard);

	pthread_mutex_unlock(&m_globalGuard);

	return getResult(result);
}

void AsyncTracker::reinforcement(const std::vector<cv::Point>& location)
{
	/* TODO: Unsafe. Need to redesign. */
	m_baseTracker->reinforcement(location);

	pthread_spin_lock(&m_resultGuard);
	m_result = location;
	pthread_spin_unlock(&m_resultGuard);
}

cv::Ptr<ObjectTracker> AsyncTracker::clone() const
{
	return cv::Ptr<ObjectTracker>(new AsyncTracker(*this));
}

bool AsyncTracker::baseTrack(std::vector<cv::Point>& result)
{
	return m_baseTracker->track(m_frame, result);
}

void *AsyncTracker::asyncTrack(void *data)
{
	AsyncTracker *tracker = reinterpret_cast<AsyncTracker*>(data);

	std::vector<cv::Point> result;
	tracker->baseTrack(result);

	pthread_spin_lock(&tracker->m_resultGuard);
	tracker->m_result = result;
	pthread_spin_unlock(&tracker->m_resultGuard);

	pthread_spin_lock(&tracker->m_isUpdatedGuard);
	tracker->m_isUpdated = true;
	pthread_spin_unlock(&tracker->m_isUpdatedGuard);

	pthread_mutex_unlock(&tracker->m_globalGuard);

	pthread_spin_lock(&tracker->m_isRunGuard);
	tracker->m_isRun = false;
	pthread_spin_unlock(&tracker->m_isRunGuard);

	return NULL;
}

bool AsyncTracker::wait()
{
	if(isRun()) {
		pthread_join(m_thread, NULL);
		return true;
	}
	return false;
}

bool AsyncTracker::isRun()
{
	bool result = false;

	pthread_spin_lock(&m_isRunGuard);
	result = m_isRun;
	pthread_spin_unlock(&m_isRunGuard);

	return result;
}

bool AsyncTracker::isUpdated(std::vector<cv::Point>& result)
{
	bool isUpdated = false;

	getResult(result);

	pthread_spin_lock(&m_isUpdatedGuard);
	isUpdated = m_isUpdated;
	m_isUpdated = false;
	pthread_spin_unlock(&m_isUpdatedGuard);

	return isUpdated;
}

bool AsyncTracker::getResult(std::vector<cv::Point>& result)
{
	bool isTracked = false;

	pthread_spin_lock(&m_resultGuard);
	isTracked = !m_result.empty();
	result = m_result;
	pthread_spin_unlock(&m_resultGuard);

	return isTracked;
}

} /* Image */
} /* MediaVision */
