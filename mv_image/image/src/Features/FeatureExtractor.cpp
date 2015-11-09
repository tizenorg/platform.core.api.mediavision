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

#include "Features/FeatureExtractor.h"

#include "ImageMathUtil.h"

#include <opencv/cv.h>

namespace MediaVision {
namespace Image {
const cv::Size FeatureExtractor::MinSize = cv::Size(5, 5);

FeatureExtractor::FeatureExtractor() :
		m_kpType(KT_INVALID),
		m_detector(),
		m_descType(DT_INVALID),
		m_extractor(),
		m_computeRecognitionRate(NULL)
{
}

void FeatureExtractor::setFeatureDetector(
		const cv::Ptr<cv::FeatureDetector> detector,
		KeypointType keypointType)
{
	m_detector = detector;
	m_kpType = keypointType;
}

void FeatureExtractor::setDescriptorExtractor(
		cv::Ptr<cv::DescriptorExtractor> extractor,
		DescriptorType descriptorType)
{
	m_extractor = extractor;
	m_descType = descriptorType;
}

void FeatureExtractor::setRecognitionRateMetric(
		float (*computeRecognitionRate)(
				const cv::Mat&,
				const std::vector<cv::KeyPoint>&))
{
	m_computeRecognitionRate = computeRecognitionRate;
}

bool FeatureExtractor::extract(
		const cv::Mat& image,
		FeaturePack& result,
		const std::vector<cv::Point2f>& roi)
{
	if (m_detector.empty() || m_extractor.empty())
		return false;

	cv::Rect boundingBox;

	if (roi.empty()) {
		boundingBox.x = 0;
		boundingBox.y = 0;
		boundingBox.width = image.cols;
		boundingBox.height = image.rows;
	} else {
		if (roi.size() < 3)
			return false;

		boundingBox = cv::boundingRect(roi);
		catRect(boundingBox, image.size());
	}

	if (boundingBox.width < MinSize.width || boundingBox.height < MinSize.height)
		return false;

	result.m_objectKeypoints.clear();

	std::vector<cv::KeyPoint> keypoints;

	m_detector->detect(
			image(boundingBox),
			keypoints);

	result.m_objectKeypoints = keypoints;
	if (!roi.empty()) {
		const size_t numberOfKeypoints = keypoints.size();
		result.m_objectKeypoints.resize(numberOfKeypoints);
		for (size_t i = 0; i < numberOfKeypoints; ++i) {
			result.m_objectKeypoints[i].pt.x += boundingBox.x;
			result.m_objectKeypoints[i].pt.y += boundingBox.y;
		}
	}

	if (!roi.empty()) {
		/* TODO: Ecode roi to reduce the boundary effect. Provide new parameter
		/ for this action cause roi is a bounding contour for the object. */

		for (size_t i = 0; i < result.m_objectKeypoints.size(); ++i) {
			if (!checkAccessory(result.m_objectKeypoints[i].pt, roi)) {
				result.m_objectKeypoints.erase(result.m_objectKeypoints.begin() + i);
				--i;
			}
		}
	}

	m_extractor->compute(
			image,
			result.m_objectKeypoints,
			result.m_objectDescriptors);

	if (NULL != m_computeRecognitionRate) {
		result.m_recognitionRate = m_computeRecognitionRate(
				image(boundingBox),
				keypoints);
	} else {
		/* Default recognition rate metric */
		if (result.m_objectKeypoints.size() < MinimumNumberOfFeatures)
			result.m_recognitionRate = 0.f;
		else
			result.m_recognitionRate = 0.5f;
	}

	result.m_keypointsType = m_kpType;
	result.m_descriptorsType = m_descType;

	return true;
}

} /* Image */
} /* MediaVision */
