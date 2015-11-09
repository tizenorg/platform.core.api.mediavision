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

#include "Recognition/ImageRecognizer.h"
#include "Recognition/ImageObject.h"

#include "mv_private.h"

namespace MediaVision {
namespace Image {
ImageRecognizer::ImageRecognizer(const ImageObject& scene) :
		m_scene(scene)
{
	; /* NULL */
}

ImageRecognizer::~ImageRecognizer()
{
	; /* NULL */
}

bool ImageRecognizer::recognize(
		const ImageObject& target,
		const RecognitionParams& params,
		std::vector<cv::Point2f>& contour,
		float ignoreFactor) const
{
	cv::Mat homophraphyMatrix;

	contour.clear();

	if (MinimumNumberOfFeatures > target.m_features.m_objectKeypoints.size()) {
		LOGW("[%s] Image object can't be recognized (Recognition rate is too small).", __FUNCTION__);
		return false;
	}

	if (MinimumNumberOfFeatures > m_scene.m_features.m_objectKeypoints.size()) {
		LOGW("[%s] Scene image can't be analyzed (Too few features for recognition).", __FUNCTION__);
		return false;
	}

	if(!findHomophraphyMatrix(target, params, homophraphyMatrix, ignoreFactor)) {
		LOGE("[%s] Can't match the features.", __FUNCTION__);
		return false;
	}

	cv::perspectiveTransform(target.m_boundingContour, contour, homophraphyMatrix);

	if (target.m_boundingContour.size() == NumberOfQuadrangleCorners) {
		if (!isPossibleQuadrangleCorners(contour.data())) {
			LOGI("[%s] Image object isn't recognized.", __FUNCTION__);
			contour.clear();
			return false;
		}
	}

	LOGI("[%s] Image object is recognized.", __FUNCTION__);
	return true;
}

bool ImageRecognizer::findHomophraphyMatrix(
		const ImageObject& target,
		const RecognitionParams& params,
		cv::Mat& homophraphyMatrix,
		float ignoreFactor) const
{
	std::vector<cv::DMatch> matches;

	m_matcher.match(
			target.m_features.m_objectDescriptors,
			m_scene.m_features.m_objectDescriptors,
			matches);

	size_t matchesNumber = matches.size();

	if (MinimumNumberOfFeatures > matchesNumber) {
		LOGE("[%s] Can't match the features.", __FUNCTION__);
		return false;
	}

	size_t requiredMatchesNumber =
			params.mRequiredMatchesPart * matchesNumber;

	size_t allowableMatchesNumberError =
			params.mTolerantMatchesPartError * requiredMatchesNumber;

	if (matchesNumber - allowableMatchesNumberError >
			(size_t)params.mMinMatchesNumber &&
			requiredMatchesNumber + allowableMatchesNumberError <
			matchesNumber) {
		if (requiredMatchesNumber - allowableMatchesNumberError <
				(size_t)params.mMinMatchesNumber) {
			if (requiredMatchesNumber + allowableMatchesNumberError >
					(size_t)params.mMinMatchesNumber) {
				requiredMatchesNumber = ((size_t)params.mMinMatchesNumber +
				requiredMatchesNumber + allowableMatchesNumberError) / 2;

				allowableMatchesNumberError = requiredMatchesNumber-
						(size_t)params.mMinMatchesNumber +
						allowableMatchesNumberError;
			} else {
				const size_t minimalAllowableMatchesNumberError = 2u;

				requiredMatchesNumber = params.mMinMatchesNumber +
						minimalAllowableMatchesNumberError;

				allowableMatchesNumberError = minimalAllowableMatchesNumberError;
			}
		}

		const size_t filterAmount = matchesSelection(matches,
													requiredMatchesNumber,
													allowableMatchesNumberError);

		if (filterAmount >= MinimumNumberOfFeatures)
			matches.resize(filterAmount);
		else
			LOGW("[%s] Wrong filtration of feature matches.", __FUNCTION__);

		matchesNumber = matches.size();
	}

	std::vector<cv::Point2f> objectPoints(matchesNumber);
	std::vector<cv::Point2f> scenePoints(matchesNumber);

	for (size_t matchIdx = 0; matchIdx < matchesNumber; ++matchIdx) {
		objectPoints[matchIdx] =
				target.m_features.m_objectKeypoints[matches[matchIdx].queryIdx].pt;

		scenePoints[matchIdx] =
				m_scene.m_features.m_objectKeypoints[matches[matchIdx].trainIdx].pt;
	}

	if (ignoreFactor > FLT_EPSILON) {
		const std::vector<cv::Point2f> significantArea = contourResize(
				target.m_boundingContour,
				ignoreFactor);

		for (size_t matchIdx = 0; matchIdx < objectPoints.size(); ++matchIdx) {
			if (!checkAccessory(objectPoints[matchIdx], significantArea)) {
				objectPoints.erase(objectPoints.begin() + matchIdx);
				scenePoints.erase(scenePoints.begin() + matchIdx);
				--matchIdx;
			}
		}
	}

	if (objectPoints.size() < MinimumNumberOfFeatures)
		return false;

	homophraphyMatrix = cv::findHomography(objectPoints, scenePoints, CV_RANSAC);

	return true;
}

size_t ImageRecognizer::matchesSelection(
		std::vector<cv::DMatch>& examples,
		unsigned int filterAmount, unsigned int allowableError) const
{
	size_t sizeOfExamples = examples.size();

	if ((filterAmount + allowableError) > sizeOfExamples)
		return examples.size();

	int startLeftLimit = 0;
	int startRightLimit = sizeOfExamples - 1;

	int leftLimit = startLeftLimit;
	int rightLimit = startRightLimit;

	int requiredNumber = filterAmount;

	float supportElement = 0.f;

	while (true) {
		if (leftLimit >= rightLimit) {
			if (leftLimit < (requiredNumber - (int)allowableError))
				leftLimit = requiredNumber + (int)allowableError;

			break;
		}

		supportElement = computeLinearSupportElement(examples, requiredNumber,
					leftLimit, rightLimit);

		/* Iteration similar quicksort */
		while (true) {
			/* Search the leftmost element which have bigger confidence than support element */
			while (examples[leftLimit].distance <= supportElement &&
					leftLimit < startRightLimit) {
				++leftLimit;
			}

			/* Search the rightmost element which have smaller confidence than support element */
			while (examples[rightLimit].distance >= supportElement &&
					rightLimit >= startLeftLimit) {
				--rightLimit;
			}

			if (leftLimit >= rightLimit)
				break;

			/* Swap */
			std::swap(examples[leftLimit], examples[rightLimit]);
		}

		if (abs(filterAmount - leftLimit) <= (int)allowableError)
			break;

		if ((int)filterAmount > leftLimit) {
			requiredNumber -= leftLimit - startLeftLimit;

			rightLimit = startRightLimit;
			startLeftLimit = leftLimit;
		} else {
			leftLimit = startLeftLimit;
			startRightLimit = rightLimit;
		}
	}

	return (size_t)leftLimit;
}

float ImageRecognizer::computeLinearSupportElement(const std::vector<cv::DMatch>& examples,
		int requiredNumber, int leftLimit, int rightLimit) const
{
	int sizeOfExamples = rightLimit - leftLimit + 1;

	if (sizeOfExamples <= 1) {
		return examples[leftLimit].distance;
	}

	float minValue = examples[leftLimit].distance;
	float maxValue = examples[leftLimit].distance;

	/* Finding the maximum and minimum values */
	for (int i = leftLimit + 1; i <= rightLimit; ++i) {
		if (minValue > examples[i].distance) {
			minValue = examples[i].distance;
		} else if (maxValue < examples[i].distance) {
			maxValue = examples[i].distance;
		}
	}

	/* Linear approximation. f(x) = k*x + b */
	/* f(sizeOfExamples) = maxValue; f(1) = minValue; */
	const float b = (maxValue - minValue * sizeOfExamples) / (1 - sizeOfExamples);
	const float k = minValue - b;

	/* Calculation of the support element */
	return k * requiredNumber + b;
}

bool ImageRecognizer::isPossibleQuadrangleCorners(
		const cv::Point2f corners[NumberOfQuadrangleCorners])
{
	static const float Epsilon = 0.1f;

	/* TODO: move the MinSizeOfDetectedArea out of the ImageRecognizer */
	static const float MinSizeOfDetectedArea = 64.f;

	const float firstSemiArea = getTriangleArea(corners[0], corners[2], corners[1]) +
			getTriangleArea(corners[0], corners[2], corners[3]);

	const float secondSemiArea = getTriangleArea(corners[1], corners[3], corners[2]) +
			getTriangleArea(corners[1], corners[3], corners[0]);

	if (Epsilon < fabs(firstSemiArea - secondSemiArea) ||
			MinSizeOfDetectedArea > (firstSemiArea + secondSemiArea))
		return false;

	return true;
}

} /* Image */
} /* MediaVision */
