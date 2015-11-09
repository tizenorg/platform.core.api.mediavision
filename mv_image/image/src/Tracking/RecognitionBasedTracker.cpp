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

#include "Tracking/RecognitionBasedTracker.h"

#include "Recognition/ImageRecognizer.h"

namespace MediaVision {
namespace Image {
RecognitionBasedTracker::RecognitionBasedTracker(
		const ImageObject& target,
		const FeaturesExtractingParams& sceneFeaturesExtractingParams,
		const RecognitionParams& recognitionParams) :
				m_target(target),
				m_sceneFeatureExtractingParams(sceneFeaturesExtractingParams),
				m_recogParams(recognitionParams)
{
	; /* NULL */
}

RecognitionBasedTracker::~RecognitionBasedTracker()
{
	; /* NULL */
}

bool RecognitionBasedTracker::track(
		const cv::Mat& frame,
		std::vector<cv::Point>& result)
{
	result.clear();

	ImageObject scene(frame, m_sceneFeatureExtractingParams);

	ImageRecognizer recognizer(scene);

	std::vector<cv::Point2f> contour;

	bool isRecognized = recognizer.recognize(m_target, m_recogParams, contour);

	if (isRecognized) {
		size_t numberOfContourPoints = contour.size();
		result.resize(numberOfContourPoints);
		for(size_t i = 0u; i < numberOfContourPoints; ++i) {
			result[i].x = (int)contour[i].x;
			result[i].y = (int)contour[i].y;
		}
	}

	return isRecognized;
}

void RecognitionBasedTracker::reinforcement(const std::vector<cv::Point>& location)
{
	; /* The tracker is based on the recognition on the entire image.
	   *The reinforcement does not make a sense.*/
}

cv::Ptr<ObjectTracker> RecognitionBasedTracker::clone() const
{
	return cv::Ptr<ObjectTracker>(new RecognitionBasedTracker(*this));
}

} /* Image */
} /* MediaVision */
