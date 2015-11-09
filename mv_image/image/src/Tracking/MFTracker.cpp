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

#include "Tracking/MFTracker.h"

#include <opencv/cv.h>

namespace MediaVision {
namespace Image {
namespace {
	const float FloatEps = 10e-6f;

	template<typename T>
	T getMedian(std::vector<T>& values, int size = -1) {
		if (size == -1)
			size = (int)values.size();

		std::vector<T> copy(values.begin(), values.begin() + size);
		std::sort(copy.begin(),copy.end());
		if(size%2==0)
			return (copy[size/2-1]+copy[size/2])/((T)2.0);
		else
			return copy[(size - 1) / 2];

	}

	inline float l2distance(cv::Point2f p1, cv::Point2f p2) {
		const float dx = p1.x - p2.x;
		const float dy = p1.y - p2.y;
		return sqrtf(dx * dx + dy * dy);
	}
} /* anonymous namespace */

MFTracker::Params::Params()
{
	mPointsInGrid = 10;
	mWindowSize = cv::Size(3, 3);
	mPyrMaxLevel = 5;
}

MFTracker::MFTracker(Params params) :
		m_isInit(false),
		m_params(params),
		m_termcrit(cv::TermCriteria::COUNT | cv::TermCriteria::EPS, 20, 0.3),
		m_confidence(0.0f)
{
}

bool MFTracker::track(const cv::Mat& frame, std::vector<cv::Point>& result)
{
	result.clear();

	if (!m_isInit) {
		if (m_startLocation.empty())
			return false;

		if (!init(frame))
			return false;
	} else {
		if (!update(frame)) {
			m_isInit = false;
			m_startLocation.clear();
			return false;
		}
	}

	const size_t numberOfContourPoints = m_startLocation.size();
	result.resize(numberOfContourPoints);

	for (size_t i = 0; i < numberOfContourPoints; ++i) {
		result[i].x = (int)(m_boundingBox.x +
					m_startLocation[i].x * m_boundingBox.width);
		result[i].y = (int)(m_boundingBox.y +
					m_startLocation[i].y * m_boundingBox.height);
	}

	return true;
}

void MFTracker::reinforcement(const std::vector<cv::Point>& location)
{
	m_isInit = false;

	if (location.size() < 3) {
		m_startLocation.clear();
		m_boundingBox.x = 0;
		m_boundingBox.y = 0;
		m_boundingBox.width = 0;
		m_boundingBox.height = 0;

		return;
	}

	const cv::Rect_<float>& boundingBox = cv::boundingRect(location);
	m_boundingBox = boundingBox;

	const size_t numberOfContourPoints = location.size();
	m_startLocation.resize(numberOfContourPoints);
	for (size_t i = 0; i < numberOfContourPoints; ++i) {
		m_startLocation[i].x = (location[i].x - boundingBox.x) / boundingBox.width;
		m_startLocation[i].y = (location[i].y - boundingBox.y) / boundingBox.height;
	}
}

cv::Ptr<ObjectTracker> MFTracker::clone() const
{
	return cv::Ptr<ObjectTracker>(new MFTracker(*this));
}

bool MFTracker::init(const cv::Mat& image)
{
	if (image.empty())
		return false;

	image.copyTo(m_image);
	buildOpticalFlowPyramid(
			m_image,
			m_pyramid,
			m_params.mWindowSize,
			m_params.mPyrMaxLevel);

	m_isInit = true;
	return m_isInit;
}

bool MFTracker::update(const cv::Mat& image)
{
	if (!m_isInit || image.empty())
		return false;

	/* Handles such behaviour when preparation frame has the size
	 * different to the tracking frame size. In such case, we resize preparation
	 *frame and bounding box. Then, track as usually:
	 */
	if (m_image.rows != image.rows || m_image.cols != image.cols) {
		const float xFactor = (float) image.cols / m_image.cols;
		const float yFactor = (float) image.rows / m_image.rows;

		resize(m_image, m_image, cv::Size(), xFactor, yFactor);

		m_boundingBox.x *= xFactor;
		m_boundingBox.y *= yFactor;
		m_boundingBox.width *= xFactor;
		m_boundingBox.height *= yFactor;
	}

	cv::Mat oldImage = m_image;

	cv::Rect_<float> oldBox = m_boundingBox;
	if(!medianFlowImpl(oldImage, image, oldBox))
		return false;

	image.copyTo(m_image);
	m_boundingBox = oldBox;

	return true;
}

bool MFTracker::isInited() const
{
	return m_isInit;
}

float MFTracker::getLastConfidence() const
{
	return m_confidence;
}

cv::Rect_<float> MFTracker::getLastBoundingBox() const
{
	return m_boundingBox;
}

bool MFTracker::medianFlowImpl(
		cv::Mat oldImage_gray, cv::Mat newImage_gray, cv::Rect_<float>& oldBox)
{
	std::vector<cv::Point2f> pointsToTrackOld, pointsToTrackNew;

	const float gridXStep = oldBox.width / m_params.mPointsInGrid;
	const float gridYStep = oldBox.height / m_params.mPointsInGrid;
	for (int i = 0; i < m_params.mPointsInGrid; i++) {
		for (int j = 0; j < m_params.mPointsInGrid; j++) {
			pointsToTrackOld.push_back(
					cv::Point2f(oldBox.x + .5f*gridXStep + 1.f*gridXStep*j,
							oldBox.y + .5f*gridYStep + 1.f*gridYStep*i));
		}
	}

	const size_t numberOfPointsToTrackOld = pointsToTrackOld.size();
	std::vector<uchar> status(numberOfPointsToTrackOld);
	std::vector<float> errors(numberOfPointsToTrackOld);

	std::vector<cv::Mat> tempPyramid;
	cv::buildOpticalFlowPyramid(
							newImage_gray,
							tempPyramid,
							m_params.mWindowSize,
							m_params.mPyrMaxLevel);

	cv::calcOpticalFlowPyrLK(m_pyramid,
							tempPyramid,
							pointsToTrackOld,
							pointsToTrackNew,
							status,
							errors,
							m_params.mWindowSize,
							m_params.mPyrMaxLevel,
							m_termcrit);

	std::vector<cv::Point2f> di;
	for (size_t idx = 0u; idx < numberOfPointsToTrackOld; idx++) {
		if (status[idx] == 1)
			di.push_back(pointsToTrackNew[idx] - pointsToTrackOld[idx]);
	}

	std::vector<bool> filter_status;
	check_FB(tempPyramid,
			pointsToTrackOld,
			pointsToTrackNew,
			filter_status);

	check_NCC(oldImage_gray,
			newImage_gray,
			pointsToTrackOld,
			pointsToTrackNew,
			filter_status);

	for (size_t idx = 0u; idx < pointsToTrackOld.size(); idx++) {
		if (!filter_status[idx]) {
			pointsToTrackOld.erase(pointsToTrackOld.begin() + idx);
			pointsToTrackNew.erase(pointsToTrackNew.begin() + idx);
			filter_status.erase(filter_status.begin() + idx);
			idx--;
		}
	}

	if (pointsToTrackOld.empty() || di.empty())
		return false;

	cv::Point2f mDisplacement;
	cv::Rect_<float> boxCandidate =
				vote(pointsToTrackOld, pointsToTrackNew, oldBox, mDisplacement);

	std::vector<float> displacements;
	for (size_t idx = 0u; idx < di.size(); idx++) {
		di[idx] -= mDisplacement;
		displacements.push_back(sqrt(di[idx].ddot(di[idx])));
	}

	m_confidence =
				(10.f - getMedian(displacements,(int)displacements.size())) / 10.f;

	if (m_confidence < 0.f) {
		m_confidence = 0.f;
		return false;
	}

	m_pyramid.swap(tempPyramid);
	oldBox = boxCandidate;
	return true;
}

cv::Rect_<float> MFTracker::vote(
		const std::vector<cv::Point2f>& oldPoints,
		const std::vector<cv::Point2f>& newPoints,
		const cv::Rect_<float>& oldRect,
		cv::Point2f& mD)
{
	cv::Rect_<float> newRect;
	cv::Point2f newCenter(
			oldRect.x + oldRect.width / 2.f,
			oldRect.y + oldRect.height / 2.f);

	const int n = (int)oldPoints.size();
	std::vector<float> buf(std::max( n*(n-1) / 2, 3), 0.f);

	if(oldPoints.size() == 1) {
		newRect.x = oldRect.x+newPoints[0].x-oldPoints[0].x;
		newRect.y = oldRect.y+newPoints[0].y-oldPoints[0].y;
		newRect.width=oldRect.width;
		newRect.height=oldRect.height;

		return newRect;
	}

	float xshift = 0.f;
	float yshift = 0.f;
	for(int i = 0; i < n; i++)
		buf[i] = newPoints[i].x - oldPoints[i].x;

	xshift = getMedian(buf, n);
	newCenter.x += xshift;
	for(int idx = 0; idx < n; idx++)
		buf[idx] = newPoints[idx].y - oldPoints[idx].y;

	yshift = getMedian(buf, n);
	newCenter.y += yshift;
	mD = cv::Point2f(xshift, yshift);

	if(oldPoints.size() == 1) {
		newRect.x = newCenter.x - oldRect.width / 2.f;
		newRect.y = newCenter.y - oldRect.height / 2.f;
		newRect.width = oldRect.width;
		newRect.height = oldRect.height;

		return newRect;
	}

	float nd = 0.f;
	float od = 0.f;
	for (int i = 0, ctr = 0; i < n; i++) {
		for(int j = 0; j < i; j++) {
			nd = l2distance(newPoints[i], newPoints[j]);
			od = l2distance(oldPoints[i], oldPoints[j]);
			buf[ctr] = (od == 0.f ? 0.f : nd / od);
			ctr++;
		}
	}

	float scale = getMedian(buf, n*(n-1) / 2);
	newRect.x = newCenter.x - scale * oldRect.width / 2.f;
	newRect.y = newCenter.y-scale * oldRect.height / 2.f;
	newRect.width = scale * oldRect.width;
	newRect.height = scale * oldRect.height;

	return newRect;
}

void MFTracker::check_FB(
		std::vector<cv::Mat> newPyramid,
		const std::vector<cv::Point2f>& oldPoints,
		const std::vector<cv::Point2f>& newPoints,
		std::vector<bool>& status)
{
	const size_t numberOfOldPoints = oldPoints.size();

	if(status.empty())
		status = std::vector<bool>(numberOfOldPoints, true);

	std::vector<uchar> LKstatus(numberOfOldPoints);
	std::vector<float> errors(numberOfOldPoints);
	std::vector<float> FBerror(numberOfOldPoints);
	std::vector<cv::Point2f> pointsToTrackReprojection;

	calcOpticalFlowPyrLK(newPyramid,
						m_pyramid,
						newPoints,
						pointsToTrackReprojection,
						LKstatus,
						errors,
						m_params.mWindowSize,
						m_params.mPyrMaxLevel,
						m_termcrit);

	for (size_t idx = 0u; idx < numberOfOldPoints; idx++)
		FBerror[idx] = l2distance(oldPoints[idx], pointsToTrackReprojection[idx]);

	float FBerrorMedian = getMedian(FBerror) + FloatEps;
	for (size_t idx = 0u; idx < numberOfOldPoints; idx++)
		status[idx] = (FBerror[idx] < FBerrorMedian);
}

void MFTracker::check_NCC(
		const cv::Mat& oldImage,
		const cv::Mat& newImage,
		const std::vector<cv::Point2f>& oldPoints,
		const std::vector<cv::Point2f>& newPoints,
		std::vector<bool>& status)
{
	std::vector<float> NCC(oldPoints.size(), 0.f);
	cv::Size patch(30, 30);
	cv::Mat p1;
	cv::Mat p2;

	for (size_t idx = 0u; idx < oldPoints.size(); idx++) {
		getRectSubPix(oldImage, patch, oldPoints[idx], p1);
		getRectSubPix(newImage, patch, newPoints[idx], p2);

		const int N = 900;
		const float s1 = sum(p1)(0);
		const float s2 = sum(p2)(0);
		const float n1 = norm(p1);
		const float n2 = norm(p2);
		const float prod = p1.dot(p2);
		const float sq1 = sqrt(n1 * n1 - s1 * s1 / N);
		const float sq2 = sqrt(n2 * n2 - s2 * s2 / N);
		NCC[idx] = (sq2==0 ? sq1 / std::abs(sq1)
							: (prod - s1 * s2 / N) / sq1 / sq2);
	}

	float median = getMedian(NCC) - FloatEps;
	for(size_t idx = 0u; idx < oldPoints.size(); idx++)
		status[idx] = status[idx] && (NCC[idx] > median);
}

} /* Image */
} /* MediaVision */
