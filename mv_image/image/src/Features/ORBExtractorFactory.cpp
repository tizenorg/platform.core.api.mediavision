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

#include "Features/ORBExtractorFactory.h"

#include "ImageMathUtil.h"

#include <opencv/cv.h>

namespace MediaVision {
namespace Image {
ORBExtractorFactory::ORBExtractorFactory(
		float scaleFactor,
		size_t maximumFeaturesNumber)
{
	setScaleFactor(scaleFactor);
	setMaximumFeaturesNumber(maximumFeaturesNumber);
}

cv::Ptr<FeatureExtractor> ORBExtractorFactory::buildFeatureExtractor()
{
	cv::Ptr<FeatureExtractor> featureExtractor(new FeatureExtractor());

	cv::Ptr<cv::OrbFeatureDetector> detector(
			new cv::ORB(
				m_maximumFeaturesNumber,
				m_scaleFactor));

	cv::Ptr<cv::OrbDescriptorExtractor> extractor = detector;

	featureExtractor->setFeatureDetector(detector, KT_ORB);
	featureExtractor->setDescriptorExtractor(extractor, DT_ORB);
	featureExtractor->setRecognitionRateMetric(computeRecognitionRate);

	return featureExtractor;
}

float ORBExtractorFactory::getScaleFactor() const
{
	return m_scaleFactor;
}

void ORBExtractorFactory::setScaleFactor(float scaleFactor)
{
	m_scaleFactor = scaleFactor;
}

size_t ORBExtractorFactory::getMaximumFeaturesNumber() const
{
	return m_scaleFactor;
}

void ORBExtractorFactory::setMaximumFeaturesNumber(size_t maximumFeaturesNumber)
{
	m_maximumFeaturesNumber = maximumFeaturesNumber;
}

float ORBExtractorFactory::computeRecognitionRate(
		const cv::Mat& image,
		const std::vector<cv::KeyPoint>& keypoints)
{
	const size_t numberOfKeypoints = keypoints.size();

	/* it is impossible to calculate the perspective transformation parameters
	 * if number of key points less than MinimumNumberOfFeatures (4) */
	if (numberOfKeypoints < MinimumNumberOfFeatures)
		return 0.f;

	static const size_t xCellsNumber = 10u;
	static const size_t yCellsNumber = 10u;

	cv::Mat cells[xCellsNumber][yCellsNumber];
	size_t accumulationCounter[xCellsNumber][yCellsNumber];

	const size_t cellWidth = image.cols / xCellsNumber;
	const size_t cellHeight = image.rows / yCellsNumber;

	for (size_t x = 0u; x < xCellsNumber; ++x) {
		for (size_t y = 0u; y < yCellsNumber; ++y) {
			cells[x][y] = image(cv::Rect(
					x * cellWidth,
					y * cellHeight,
					cellWidth,
					cellHeight));

			accumulationCounter[x][y] = 0;
		}
	}

	for (size_t i = 0u; i < numberOfKeypoints; ++i) {
		size_t xCellIdx = keypoints[i].pt.x / cellWidth;
		if (xCellIdx >= xCellsNumber)
			xCellIdx = xCellsNumber - 1;
        
		size_t yCellIdx = keypoints[i].pt.y / cellHeight;
		if (yCellIdx >= yCellsNumber)
			yCellIdx = yCellsNumber - 1;

		++(accumulationCounter[xCellIdx][yCellIdx]);
	}

	const float exceptedNumber = numberOfKeypoints /
				(float)(xCellsNumber * yCellsNumber);

	float distributedEvaluation = 0.f;

	for (size_t x = 0u; x < xCellsNumber; ++x) {
		for (size_t y = 0u; y < yCellsNumber; ++y) {
			distributedEvaluation += (accumulationCounter[x][y] - exceptedNumber) *
					(accumulationCounter[x][y] - exceptedNumber) / exceptedNumber;
		}
	}

	float maximumDistributedEvaluation = (xCellsNumber * yCellsNumber - 1) *
											exceptedNumber;

	maximumDistributedEvaluation += (numberOfKeypoints - exceptedNumber) *
			(numberOfKeypoints - exceptedNumber) / exceptedNumber;

	distributedEvaluation = 1 -
				(distributedEvaluation / maximumDistributedEvaluation);

	/* Exponentiation to find an approximate confidence value based on the
	 * number of key points on the image. */
	const float cardinalityEvaluation = pow(-0.9, numberOfKeypoints - 3) + 1.0f;

	/* Result metric */
	return distributedEvaluation * cardinalityEvaluation;
}

} /* Image */
} /* MediaVision */
