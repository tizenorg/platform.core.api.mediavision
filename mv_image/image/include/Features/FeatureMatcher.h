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

#ifndef __IMAGEFEATURES_FEATUREMATCHER_H__
#define __IMAGEFEATURES_FEATUREMATCHER_H__

#include "Features/FeaturePack.h"

#include <opencv2/features2d/features2d.hpp>

namespace MediaVision {
namespace Image {

class FeatureMatcher {
public:
	enum MatchError{
		InvalidFeaturePackFrom,
		InvalidFeaturePackTo,
		DisparateTypes,
		MatchesNotFound,
		Success
	};

public:
	FeatureMatcher(
			float affectingPart = 1.f,
			float tolerantError = 0.f,
			size_t minimumMatchesNumber = 0u);

	MatchError match(
			const FeaturePack& from,
			const FeaturePack& to,
			cv::Mat& homophraphyMatrix) const;

	float getAffectingPart() const;

	void setAffectingPart(float affectingPart);

	float getTolerantError() const;

	void setTolerantError(float tolerantError);

	size_t getMinimumMatchesNumber() const;

	void setMinimumMatchesNumber(size_t minimumMatchesNumber);

private:
	cv::BFMatcher m_matcher;

	float m_affectingPart;

	float m_tolerantError;

	size_t m_minimumMatchesNumber;
};

} /* Image */
} /* MediaVision */

#endif /* __IMAGEFEATURES_FEATUREMATCHER_H__ */
