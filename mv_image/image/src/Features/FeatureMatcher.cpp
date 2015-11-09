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

#include "Features/FeatureMatcher.h"

#include "ImageMathUtil.h"

#include <opencv/cv.h>

namespace MediaVision {
namespace Image {

namespace {
float computeLinearSupportElement(
		const std::vector<cv::DMatch>& examples,
		int requiredNumber,
		int leftLimit,
		int rightLimit)
{
	int sizeOfExamples = rightLimit - leftLimit + 1;

	if (sizeOfExamples <= 1)
		return examples[leftLimit].distance;

	float minValue = examples[leftLimit].distance;
	float maxValue = examples[leftLimit].distance;

	/* Finding the maximum and minimum values */
	for (int i = leftLimit + 1; i <= rightLimit; ++i) {
		if (minValue > examples[i].distance)
			minValue = examples[i].distance;
		else if (maxValue < examples[i].distance)
			maxValue = examples[i].distance;
	}

	/* Linear approximation. f(x) = k*x + b */
	/* f(sizeOfExamples) = maxValue; f(1) = minValue; */
	const float b = (maxValue - minValue * sizeOfExamples) / (1 - sizeOfExamples);
	const float k = minValue - b;

	/* Calculation of the support element */
	return k * requiredNumber + b;
}

size_t matchesSelection(
		std::vector<cv::DMatch>& examples,
		size_t filterAmount,
		size_t allowableError)
{
	size_t sizeOfExamples = examples.size();

	if ((filterAmount + allowableError) > sizeOfExamples)
		return sizeOfExamples;

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

} /* anonymous namespace */

FeatureMatcher::FeatureMatcher(
		float affectingPart,
		float tolerantError,
		size_t minimumMatchesNumber)
{
	setAffectingPart(affectingPart);
	setTolerantError(tolerantError);
	setMinimumMatchesNumber(minimumMatchesNumber);
}

FeatureMatcher::MatchError FeatureMatcher::match(
		const FeaturePack& from,
		const FeaturePack& to,
		cv::Mat& homophraphyMatrix) const
{
	if (MinimumNumberOfFeatures > from.m_objectKeypoints.size())
		return InvalidFeaturePackFrom;
    
	if (MinimumNumberOfFeatures > to.m_objectKeypoints.size())
		return InvalidFeaturePackTo;
    
	if (from.m_descriptorsType != to.m_descriptorsType)
		return DisparateTypes;

	std::vector<cv::DMatch> matches;

	m_matcher.match(from.m_objectDescriptors, to.m_objectDescriptors, matches);

	size_t matchesNumber = matches.size();

	if (MinimumNumberOfFeatures > matchesNumber)
		return MatchesNotFound;

	size_t requiredMatchesNumber = m_affectingPart * matchesNumber;
	size_t allowableMatchesNumberError = m_tolerantError * requiredMatchesNumber;

	if (matchesNumber - allowableMatchesNumberError > MinimumNumberOfFeatures &&
		requiredMatchesNumber + allowableMatchesNumberError < matchesNumber) {
		if (requiredMatchesNumber - allowableMatchesNumberError <
			m_minimumMatchesNumber) {
			if (requiredMatchesNumber + allowableMatchesNumberError >
				m_minimumMatchesNumber) {
				requiredMatchesNumber = (requiredMatchesNumber +
						m_minimumMatchesNumber + allowableMatchesNumberError) / 2;

				allowableMatchesNumberError = requiredMatchesNumber -
						m_minimumMatchesNumber + allowableMatchesNumberError;
			} else {
				const size_t minimalAllowableMatchesNumberError = 2u;

				requiredMatchesNumber = minimalAllowableMatchesNumberError +
										m_minimumMatchesNumber;

				allowableMatchesNumberError = minimalAllowableMatchesNumberError;
			}
		}

		const size_t filterAmount = matchesSelection(
													matches,
													requiredMatchesNumber,
													allowableMatchesNumberError);

		if (filterAmount >= MinimumNumberOfFeatures)
			matches.resize(filterAmount);

		matchesNumber = matches.size();
	}

	std::vector<cv::Point2f> objectPoints(matchesNumber);
	std::vector<cv::Point2f> scenePoints(matchesNumber);

	for (size_t matchIdx = 0; matchIdx < matchesNumber; ++matchIdx) {
		objectPoints[matchIdx] =
				from.m_objectKeypoints[matches[matchIdx].queryIdx].pt;

		scenePoints[matchIdx] =
				to.m_objectKeypoints[matches[matchIdx].trainIdx].pt;
	}

	homophraphyMatrix = cv::findHomography(objectPoints, scenePoints, CV_RANSAC);

	return Success;
}

float FeatureMatcher::getAffectingPart() const
{
	return m_affectingPart;
}

void FeatureMatcher::setAffectingPart(float affectingPart)
{
	m_affectingPart = std::max(0.f, std::min(1.f, affectingPart));
}

float FeatureMatcher::getTolerantError() const
{
	return m_tolerantError;
}

void FeatureMatcher::setTolerantError(float tolerantError)
{
	m_affectingPart = std::max(0.f, std::min(1.f, tolerantError));
}

size_t FeatureMatcher::getMinimumMatchesNumber() const
{
	return m_minimumMatchesNumber;
}

void FeatureMatcher::setMinimumMatchesNumber(size_t minimumMatchesNumber)
{
	m_minimumMatchesNumber = minimumMatchesNumber;
}

} /* Image */
} /* MediaVision */
