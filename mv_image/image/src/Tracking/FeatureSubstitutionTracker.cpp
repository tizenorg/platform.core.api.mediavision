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

#include "Tracking/FeatureSubstitutionTracker.h"

#include "Recognition/ImageRecognizer.h"

namespace MediaVision {
namespace Image {
FeatureSubstitutionTracker::FeatureSubstitutionTracker(
		const FeaturesExtractingParams& featuresExtractingParams,
		const RecognitionParams& recognitionParams,
		float expectedOffset,
		float sceneScalingFactor,
		float objectScalingFactor) :
				m_isInit(false),
				m_target(),
				m_location(),
				m_featureExtractingParams(featuresExtractingParams),
				m_recogParams(recognitionParams),
				m_expectedOffset(expectedOffset),
				m_sceneScalingFactor(sceneScalingFactor),
				m_objectScalingFactor(objectScalingFactor)
{
	; /* NULL */
}

bool FeatureSubstitutionTracker::track(
		const cv::Mat& frame,
		std::vector<cv::Point>& result)
{
	std::vector<cv::Point2f> contour;
	size_t numberOfContourPoints = m_location.size();
	contour.resize(numberOfContourPoints);
	for(size_t i = 0u; i < numberOfContourPoints; ++i) {
		contour[i].x = m_location[i].x;
		contour[i].y = m_location[i].y;
	}

	if (!m_isInit) {
		if (m_location.empty()) {
			return false;
		} else {
			m_target = new ImageObject;
			m_target->fill(
					frame,
					m_featureExtractingParams,
					contourResize(contour, m_objectScalingFactor));
			m_target->setContour(contour);
			m_isInit = true;
			result = m_location;
			return true;
		}
	}

	cv::Ptr<ImageObject> sceneImageObject = new ImageObject;

	sceneImageObject->fill(frame, m_featureExtractingParams, computeExpectedArea());

	ImageRecognizer recognizer(*sceneImageObject.obj);

	const bool isTracked =
			recognizer.recognize(
					*m_target.obj,
					m_recogParams,
					contour,
					m_objectScalingFactor);

	if (isTracked) {
		numberOfContourPoints = contour.size();
		m_location.resize(numberOfContourPoints);
		for(size_t i = 0u; i < numberOfContourPoints; ++i) {
			m_location[i].x = (int)contour[i].x;
			m_location[i].y = (int)contour[i].y;
		}

		result = m_location;
		m_target = sceneImageObject;
		m_target->setContour(contour);
	} else {
		m_location.clear();
		m_isInit = false;
	}

	return isTracked;
}

void FeatureSubstitutionTracker::reinforcement(const std::vector<cv::Point>& location)
{
	m_isInit = false;

	if (location.size() < 3) {
		m_location.clear();
		return;
	}

	m_location = location;
}

cv::Ptr<ObjectTracker> FeatureSubstitutionTracker::clone() const
{
	return cv::Ptr<ObjectTracker>(new FeatureSubstitutionTracker(*this));
}

std::vector<cv::Point2f> FeatureSubstitutionTracker::computeExpectedArea()
{
	std::vector<cv::Point2f> contour;
	const size_t numberOfContourPoints = m_location.size();
	contour.resize(numberOfContourPoints);
	for(size_t i = 0u; i < numberOfContourPoints; ++i) {
		contour[i].x = m_location[i].x;
		contour[i].y = m_location[i].y;
	}

	return contourResize(contour, m_sceneScalingFactor);
}

} /* Image */
} /* MediaVision */
