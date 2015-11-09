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

#include "Tracking/CascadeTracker.h"
#include "Tracking/AsyncTracker.h"

#include "ImageMathUtil.h"

namespace MediaVision {
namespace Image {
CascadeTracker::CascadeTracker(float minimumArea) :
		m_trackers(),
		m_minimumArea(minimumArea)
{
	; /* NULL */
}

CascadeTracker::CascadeTracker(const CascadeTracker& copy) :
		m_trackers(),
		m_minimumArea(copy.m_minimumArea)
{
	*this = copy;
}

CascadeTracker::~CascadeTracker()
{
	; /* NULL */
}

bool CascadeTracker::track(const cv::Mat& frame, std::vector<cv::Point>& result)
{
	internalReinforcement();

	std::set<TrackerInfo>::iterator it = m_trackers.begin();

	for (; it != m_trackers.end(); ++it) {
		if (!it->mTracker.obj->track(frame, it->mResult)) {
			it->mResult.clear();
		}
	}

	return mergeResults(result);
}

void CascadeTracker::reinforcement(const std::vector<cv::Point>& location)
{
	std::set<TrackerInfo>::iterator it = m_trackers.begin();

	for (; it != m_trackers.end(); ++it)
		it->mTracker.obj->reinforcement(location);
}

cv::Ptr<ObjectTracker> CascadeTracker::clone() const
{
	return cv::Ptr<ObjectTracker>(new CascadeTracker(*this));
}

CascadeTracker& CascadeTracker::operator=(const CascadeTracker& copy)
{
	if (this != &copy) {
		this->m_minimumArea = copy.m_minimumArea;
		this->m_trackers.clear();

		std::set<TrackerInfo>::iterator it = copy.m_trackers.begin();
		for (; it != copy.m_trackers.end(); ++it) {
			TrackerInfo temp(it->mTracker.obj->clone(), it->mPriority);
			temp.mResult = it->mResult;

			m_trackers.insert(temp);
		}
	}

	return *this;
}

bool CascadeTracker::enableTracker(cv::Ptr<ObjectTracker> tracker, float priority)
{
	TrackerInfo temp(tracker, priority);

	std::set<TrackerInfo>::iterator it =
			std::find(m_trackers.begin(), m_trackers.end(), temp);

	if (it != m_trackers.end())
		m_trackers.erase(it);

	return m_trackers.insert(temp).second;
}

bool CascadeTracker::disableTracker(cv::Ptr<ObjectTracker> tracker)
{
	TrackerInfo target(tracker, 0);

	std::set<TrackerInfo>::iterator it =
			std::find(m_trackers.begin(), m_trackers.end(), target);

	if (it == m_trackers.end())
		return false;

	m_trackers.erase(it);

	return true;
}

void CascadeTracker::internalReinforcement()
{
	std::set<TrackerInfo>::iterator it1 = m_trackers.begin();
	for (; it1 != m_trackers.end(); ++it1) {
		bool isUpdated = true;

		/* TODO: Redesign without dynamic_cast */
		AsyncTracker *asyncView = dynamic_cast<AsyncTracker*>(it1->mTracker.obj);
		if (NULL != asyncView)
			isUpdated = asyncView->isUpdated(it1->mResult);

		if (!it1->mResult.empty() && isUpdated) {
			const size_t numberOfContourPoints = it1->mResult.size();
			std::vector<cv::Point2f> checkedArea(numberOfContourPoints);
			for (size_t i = 0; i < numberOfContourPoints; ++i) {
				checkedArea[i].x = it1->mResult[i].x;
				checkedArea[i].y = it1->mResult[i].y;
			}

			if (getQuadrangleArea(checkedArea.data()) < m_minimumArea) {
				it1->mResult = std::vector<cv::Point>(0);
				it1->mTracker.obj->reinforcement(it1->mResult);
			}

			float priority = it1->mPriority;
			std::set<TrackerInfo>::iterator it2 = m_trackers.begin();

			for (; it2 != m_trackers.end(); ++it2) {
				if (it1 != it2 &&
						priority > it2->mPriority) {
					 it2->mTracker.obj->reinforcement(it1->mResult);
				}
			}
		}
	}
}

bool CascadeTracker::mergeResults(std::vector<cv::Point>& result) const
{
	result.clear();

	std::set<TrackerInfo>::iterator it = m_trackers.begin();

	float resPriotiry = 0.f;
	for (; it != m_trackers.end(); ++it) {
		if (result.empty() || resPriotiry > it->mPriority) {
			resPriotiry = it->mPriority;
			result = it->mResult;
		}
	}

	return !(result.empty());
}

CascadeTracker::TrackerInfo::TrackerInfo(cv::Ptr<ObjectTracker> tracker, float priority) :
		mTracker(tracker),
		mPriority(priority),
		mResult()
{
	; /* NULL */
}

bool CascadeTracker::TrackerInfo::operator<(const TrackerInfo& second) const
{
	return (this->mPriority < second.mPriority);
}

bool CascadeTracker::TrackerInfo::operator==(const TrackerInfo& second) const
{
	return (this->mTracker == second.mTracker);
}

bool CascadeTracker::TrackerInfo::operator!=(const TrackerInfo& second) const
{
	return !(*this == second);
}

} /* Image */
} /* MediaVision */
