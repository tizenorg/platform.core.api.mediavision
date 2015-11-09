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

#ifndef __IMAGEFEATURES_FEATUREEXTRACTOR_H__
#define __IMAGEFEATURES_FEATUREEXTRACTOR_H__

#include "ImageConfig.h"

#include "Features/FeaturePack.h"

namespace cv {
class FeatureDetector;
class DescriptorExtractor;
}

namespace MediaVision {
namespace Image {
/**
 * @class FeatureExtractor
 * @brief Class contains functionality to extract features from an image
 *
 * @since_tizen 3.0
 */
class FeatureExtractor {
public:
	FeatureExtractor();

	void setFeatureDetector(
			const cv::Ptr<cv::FeatureDetector> detector,
			KeypointType keypointType);

	void setDescriptorExtractor(
			cv::Ptr<cv::DescriptorExtractor> extractor,
			DescriptorType descriptorType);

	void setRecognitionRateMetric(
			float (*computeRecognitionRate)(
					const cv::Mat&,
					const std::vector<cv::KeyPoint>&));

	bool extract(
			const cv::Mat& image,
			FeaturePack& result,
			const std::vector<cv::Point2f>& roi = std::vector<cv::Point2f>());

private:
	static const cv::Size MinSize;

private:
	KeypointType m_kpType;

	cv::Ptr<cv::FeatureDetector> m_detector;

	DescriptorType m_descType;

	cv::Ptr<cv::DescriptorExtractor> m_extractor;

	float (*m_computeRecognitionRate)(
			const cv::Mat&,
			const std::vector<cv::KeyPoint>&);
};

} /* Image */
} /* MediaVision */

#endif /* __IMAGEFEATURES_FEATUREEXTRACTOR_H__ */
