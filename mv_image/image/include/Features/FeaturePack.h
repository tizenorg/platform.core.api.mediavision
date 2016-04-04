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

#ifndef __IMAGEFEATURES_FEATUREPACK_H__
#define __IMAGEFEATURES_FEATUREPACK_H__

#include "ImageConfig.h"

#include <vector>
#include <opencv2/core/core.hpp>

namespace cv {
class KeyPoint;
}

namespace MediaVision {
namespace Image {
/**
 * @class    FeaturePack
 * @brief    This class contains information about features and can be used for
 *           recognition.
 *
 * @since_tizen 3.0
 */
class EXPORT_API FeaturePack {
public:
	/**
	 * @brief   @ref FeaturePack default constructor.
	 *
	 * @since_tizen 3.0
	 */
	FeaturePack();

	/**
	 * @brief   @ref FeaturePack copy constructor.
	 * @details Creates copy of @ref FeaturePack
	 *
	 * @since_tizen 3.0
	 * @param   [in] copy @ref FeaturePack which will be copied
	 */
	FeaturePack(const FeaturePack& copy);

	/**
	 * @brief   @ref FeaturePack copy assignment operator.
	 * @details Fills the information based on the @a copy
	 *
	 * @since_tizen 3.0
	 * @param   [in] copy @ref FeaturePack which will be copied
	 */
	FeaturePack& operator= (const FeaturePack& copy);

	KeypointType m_keypointsType;

	std::vector<cv::KeyPoint> m_objectKeypoints;

	DescriptorType m_descriptorsType;

	cv::Mat m_objectDescriptors;

	float m_recognitionRate;
};

} /* Image */
} /* MediaVision */

#endif /* __IMAGEFEATURES_FEATUREPACK_H__ */
