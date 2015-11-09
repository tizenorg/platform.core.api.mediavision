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

#include "Features/BasicExtractorFactory.h"

#include <opencv/cv.h>

namespace MediaVision {
namespace Image {
BasicExtractorFactory::BasicExtractorFactory(
		KeypointType keypointsType,
		DescriptorType descType) :
				m_kpType(keypointsType),
				m_descType(descType)
{
}

cv::Ptr<FeatureExtractor> BasicExtractorFactory::buildFeatureExtractor()
{
	cv::Ptr<FeatureExtractor> featureExtractor(new FeatureExtractor());

	cv::Ptr<cv::FeatureDetector> detector =
			cv::FeatureDetector::create(KeypointNames[m_kpType]);

	cv::Ptr<cv::DescriptorExtractor> extractor =
			cv::DescriptorExtractor::create(DescriptorNames[m_descType]);

	featureExtractor->setFeatureDetector(detector, m_kpType);
	featureExtractor->setDescriptorExtractor(extractor, m_descType);

	return featureExtractor;
}

} /* Image */
} /* MediaVision */
