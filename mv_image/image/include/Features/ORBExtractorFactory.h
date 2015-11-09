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

#ifndef __IMAGEFEATURES_ORBEXTRACTORFACTORY_H__
#define __IMAGEFEATURES_ORBEXTRACTORFACTORY_H__

#include "Features/FeatureExtractorFactory.h"

namespace MediaVision {
namespace Image {

class ORBExtractorFactory : public FeatureExtractorFactory {
public:
	ORBExtractorFactory(
			float scaleFactor = 1.2f,
			size_t maximumFeaturesNumber = 800u);

	virtual cv::Ptr<FeatureExtractor> buildFeatureExtractor();

	float getScaleFactor() const;

	void setScaleFactor(float scaleFactor);

	size_t getMaximumFeaturesNumber() const;

	void setMaximumFeaturesNumber(size_t maximumFeaturesNumber);

private:
	static float computeRecognitionRate(
			const cv::Mat&,
			const std::vector<cv::KeyPoint>&);

	float m_scaleFactor; /**< Recognition scale factor for the ORB detector. */

	size_t m_maximumFeaturesNumber; /**< Maximum number of features, which will
										be extracted from object image. */
};

} /* Image */
} /* MediaVision */

#endif /* __IMAGEFEATURES_ORBEXTRACTORFACTORY_H__ */
