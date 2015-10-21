/*M///////////////////////////////////////////////////////////////////////////////////////
 //
 //  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
 //
 //  By downloading, copying, installing or using the software you agree to this license.
 //  If you do not agree to this license, do not download, install,
 //  copy or use the software.
 //
 //
 //                           License Agreement
 //                For Open Source Computer Vision Library
 //
 // Copyright (C) 2013, OpenCV Foundation, all rights reserved.
 // Third party copyrights are property of their respective owners.
 //
 // Redistribution and use in source and binary forms, with or without modification,
 // are permitted provided that the following conditions are met:
 //
 //   * Redistribution's of source code must retain the above copyright notice,
 //     this list of conditions and the following disclaimer.
 //
 //   * Redistribution's in binary form must reproduce the above copyright notice,
 //     this list of conditions and the following disclaimer in the documentation
 //     and/or other materials provided with the distribution.
 //
 //   * The name of the copyright holders may not be used to endorse or promote products
 //     derived from this software without specific prior written permission.
 //
 // This software is provided by the copyright holders and contributors "as is" and
 // any express or implied warranties, including, but not limited to, the implied
 // warranties of merchantability and fitness for a particular purpose are disclaimed.
 // In no event shall the Intel Corporation or contributors be liable for any direct,
 // indirect, incidental, special, exemplary, or consequential damages
 // (including, but not limited to, procurement of substitute goods or services;
 // loss of use, data, or profits; or business interruption) however caused
 // and on any theory of liability, whether in contract, strict liability,
 // or tort (including negligence or otherwise) arising in any way out of
 // the use of this software, even if advised of the possibility of such damage.
 //
 //M*/

#include "TrackerMedianFlow.h"

#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <algorithm>
#include <cmath>

namespace {
	float FloatEps = 10e-6f;
} /* anonymous namespace */

namespace cv {
TrackerMedianFlow::Params::Params()
{
	mPointsInGrid = 10;
	mWindowSize = Size(3, 3);
	mPyrMaxLevel = 5;
}

void TrackerMedianFlow::Params::read(const cv::FileNode& fn)
{
	mPointsInGrid = fn["pointsInGrid"];
	int winSizeHeight = fn["windowSizeHeight"];
	int winSizeWidth = fn["windowSizeWidth"];
	mWindowSize = Size(winSizeHeight, winSizeWidth);
	mPyrMaxLevel = fn["pyrMaxLevel"];
}

void TrackerMedianFlow::Params::write(cv::FileStorage& fs) const
{
	fs << "pointsInGrid" << mPointsInGrid;
	fs << "windowSizeHeight" << mWindowSize.height;
	fs << "windowSizeWidth" << mWindowSize.width;
	fs << "pyrMaxLevel" << mPyrMaxLevel;
}

TrackerMedianFlow::TrackerMedianFlow(Params paramsIn) :
	termcrit(TermCriteria::COUNT | TermCriteria::EPS, 20, 0.3),
	m_confidence(0.0)
{
	params = paramsIn;
	isInit = false;
}

bool TrackerMedianFlow::copyTo(TrackerMedianFlow& copy) const
{
	copy.isInit = isInit;
	copy.params = params;
	copy.termcrit = termcrit;
	copy.m_boundingBox = m_boundingBox;
	copy.m_confidence = m_confidence;
	m_image.copyTo(copy.m_image);
	return true;
}

bool TrackerMedianFlow::init(const Mat& image, const Rect_<float>& boundingBox)
{
	if (image.empty()) {
		return false;
	}

	image.copyTo(m_image);
	buildOpticalFlowPyramid(
			m_image, m_pyramid, params.mWindowSize, params.mPyrMaxLevel);
	m_boundingBox = boundingBox;

	isInit = true;
	return isInit;
}

bool TrackerMedianFlow::update(const Mat& image, Rect_<float>& boundingBox)
{
	if (!isInit || image.empty())
		return false;

	/* Handles such behaviour when preparation frame has the size
	 * different to the tracking frame size. In such case, we resize preparation
	 * frame and bounding box. Then, track as usually:
	 */
	if (m_image.rows != image.rows || m_image.cols != image.cols) {
		const float xFactor = (float) image.cols / m_image.cols;
		const float yFactor = (float) image.rows / m_image.rows;

		resize(m_image, m_image, Size(), xFactor, yFactor);

		m_boundingBox.x *= xFactor;
		m_boundingBox.y *= yFactor;
		m_boundingBox.width *= xFactor;
		m_boundingBox.height *= yFactor;
	}

	Mat oldImage = m_image;

	Rect_<float> oldBox = m_boundingBox;
	if(!medianFlowImpl(oldImage, image, oldBox)) {
		return false;
	}

	boundingBox = oldBox;
	image.copyTo(m_image);
	m_boundingBox = boundingBox;
	return true;
}

bool TrackerMedianFlow::isInited() const
{
	return isInit;
}

float TrackerMedianFlow::getLastConfidence() const
{
	return m_confidence;
}

Rect_<float> TrackerMedianFlow::getLastBoundingBox() const
{
	return m_boundingBox;
}

bool TrackerMedianFlow::medianFlowImpl(
		Mat oldImage_gray, Mat newImage_gray, Rect_<float>& oldBox)
{
	std::vector<Point2f> pointsToTrackOld, pointsToTrackNew;

	const float gridXStep = oldBox.width / params.mPointsInGrid;
	const float gridYStep = oldBox.height / params.mPointsInGrid;
	for (int i = 0; i < params.mPointsInGrid; i++) {
		for (int j = 0; j < params.mPointsInGrid; j++) {
			pointsToTrackOld.push_back(
						Point2f(oldBox.x + .5f*gridXStep + 1.f*gridXStep*j,
								oldBox.y + .5f*gridYStep + 1.f*gridYStep*i));
		}
	}

	std::vector<uchar> status(pointsToTrackOld.size());
	std::vector<float> errors(pointsToTrackOld.size());

	std::vector<Mat> tempPyramid;
	buildOpticalFlowPyramid(
							newImage_gray,
							tempPyramid,
							params.mWindowSize,
							params.mPyrMaxLevel);

	calcOpticalFlowPyrLK(m_pyramid,
						tempPyramid,
						pointsToTrackOld,
						pointsToTrackNew,
						status,
						errors,
						params.mWindowSize,
						params.mPyrMaxLevel,
						termcrit);

	std::vector<Point2f> di;
	for (size_t idx = 0u; idx < pointsToTrackOld.size(); idx++) {
		if (status[idx] == 1) {
			di.push_back(pointsToTrackNew[idx] - pointsToTrackOld[idx]);
		}
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

	if (pointsToTrackOld.size() == 0 || di.size() == 0) {
		return false;
	}

	Point2f mDisplacement;
	Rect_<float> boxCandidate =
			vote(pointsToTrackOld, pointsToTrackNew, oldBox, mDisplacement);

	std::vector<float> displacements;
	for (size_t idx = 0u; idx < di.size(); idx++) {
		di[idx] -= mDisplacement;
		displacements.push_back(sqrt(di[idx].ddot(di[idx])));
	}

	m_confidence =
				(10.f - getMedian(displacements, (int)displacements.size())) / 10.f;
	if (m_confidence <= 0.f) {
		m_confidence = 0.f;
		return false;
	}

	m_pyramid.swap(tempPyramid);
	oldBox = boxCandidate;
	return true;
}

Rect_<float> TrackerMedianFlow::vote(
		const std::vector<Point2f>& oldPoints,
		const std::vector<Point2f>& newPoints,
		const Rect_<float>& oldRect,
		Point2f& mD)
{
	Rect_<float> newRect;
	Point2d newCenter(oldRect.x + oldRect.width/2.0,
						oldRect.y + oldRect.height/2.0);

	int n = (int)oldPoints.size();
	std::vector<float>buf(std::max(n*(n-1) / 2, 3), 0.f);

	if(oldPoints.size() == 1) {
		newRect.x = oldRect.x+newPoints[0].x-oldPoints[0].x;
		newRect.y = oldRect.y+newPoints[0].y-oldPoints[0].y;
		newRect.width = oldRect.width;
		newRect.height = oldRect.height;
		return newRect;
	}

	float xshift = 0.f;
	float yshift = 0.f;
	for(int i = 0; i < n; i++) {
		buf[i] = newPoints[i].x - oldPoints[i].x;
	}

	xshift = getMedian(buf, n);
	newCenter.x += xshift;
	for(int idx = 0; idx < n; idx++) {
		buf[idx] = newPoints[idx].y - oldPoints[idx].y;
	}

	yshift = getMedian(buf, n);
	newCenter.y += yshift;
	mD = Point2f(xshift, yshift);

	if(oldPoints.size() == 1) {
		newRect.x = newCenter.x - oldRect.width / 2.0;
		newRect.y = newCenter.y - oldRect.height / 2.0;
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

template<typename T>
T TrackerMedianFlow::getMedian(std::vector<T>& values, int size)
{
	if (size == -1) {
		size = (int)values.size();
	}

	std::vector<T> copy(values.begin(), values.begin() + size);
	std::sort(copy.begin(), copy.end());
	if(size%2 == 0) {
		return (copy[size/2-1]+copy[size/2])/((T)2.0);
	} else {
		return copy[(size - 1) / 2];
	}
}

float TrackerMedianFlow::l2distance(Point2f p1, Point2f p2)
{
	float dx = p1.x - p2.x;
	float dy = p1.y - p2.y;
	return sqrt(dx * dx + dy * dy);
}

void TrackerMedianFlow::check_FB(
		std::vector<Mat> newPyramid,
		const std::vector<Point2f>& oldPoints,
		const std::vector<Point2f>& newPoints,
		std::vector<bool>& status)
{
	if(status.size() == 0) {
		status = std::vector<bool>(oldPoints.size(), true);
	}

	std::vector<uchar> LKstatus(oldPoints.size());
	std::vector<float> errors(oldPoints.size());
	std::vector<float> FBerror(oldPoints.size());
	std::vector<Point2f> pointsToTrackReprojection;

	calcOpticalFlowPyrLK(newPyramid,
						m_pyramid,
						newPoints,
						pointsToTrackReprojection,
						LKstatus,
						errors,
						params.mWindowSize,
						params.mPyrMaxLevel,
						termcrit);

	for (size_t idx = 0u; idx < oldPoints.size(); idx++) {
		FBerror[idx] =
					l2distance(oldPoints[idx], pointsToTrackReprojection[idx]);
	}

	float FBerrorMedian = getMedian(FBerror) + FloatEps;
	for (size_t idx = 0u; idx < oldPoints.size(); idx++) {
		status[idx] = (FBerror[idx] < FBerrorMedian);
	}
}

void TrackerMedianFlow::check_NCC(
		const Mat& oldImage,
		const Mat& newImage,
		const std::vector<Point2f>& oldPoints,
		const std::vector<Point2f>& newPoints,
		std::vector<bool>& status)
{
	std::vector<float> NCC(oldPoints.size(), 0.f);
	Size patch(30, 30);
	Mat p1;
	Mat p2;

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
		NCC[idx] = (sq2 == 0 ? sq1 / std::abs(sq1)
					: (prod - s1 * s2 / N) / sq1 / sq2);
	}

	float median = getMedian(NCC) - FloatEps;
	for(size_t idx = 0u; idx < oldPoints.size(); idx++) {
		status[idx] = status[idx] && (NCC[idx] > median);
	}
}

void TrackerMedianFlow::read(cv::FileStorage& fs)
{
	params.read(fs.root());
	float bbX = 0.f;
	float bbY = 0.f;
	float bbW = 0.f;
	float bbH = 0.f;
	fs["lastLocationX"] >> bbX;
	fs["lastLocationY"] >> bbY;
	fs["lastLocationW"] >> bbW;
	fs["lastLocationH"] >> bbH;
	m_boundingBox = Rect_<float>(bbX, bbY, bbW, bbH);
	fs["lastImage"] >> m_image;
}

void TrackerMedianFlow::write(cv::FileStorage& fs) const
{
	params.write(fs);
	fs << "lastLocationX" << m_boundingBox.x;
	fs << "lastLocationY" << m_boundingBox.y;
	fs << "lastLocationW" << m_boundingBox.width;
	fs << "lastLocationH" << m_boundingBox.height;
	fs << "lastImage" << m_image;
}

} /* namespace cv */
