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

#ifndef __IMAGEUTIL_H__
#define __IMAGEUTIL_H__

#include <opencv/cv.h>

/**
 * @file  ImageUtil.h
 * @brief This file contains Image Module utility.
 */

namespace MediaVision {
namespace Image {
/**
 * @brief Contains parameters for features extracting from image objects.
 *
 * @since_tizen 3.0
 */
struct FeaturesExtractingParams {
	FeaturesExtractingParams(
			double scaleFactor,
			int maximumFeaturesNumber);

	FeaturesExtractingParams();

	double mScaleFactor; /**< Recognition scale factor for the ORB detector. */

	int mMaximumFeaturesNumber; /**< Maximum number of features, which will be
									extracted from object image. */
};

/**
 * @brief Contains parameters for image objects recognition.
 *
 * @since_tizen 3.0
 */
struct RecognitionParams {
	RecognitionParams(
			int minMatchesNumber,
			double requiredMatchesPart,
			double allowableMatchesPartError);

	RecognitionParams();

	int mMinMatchesNumber; /**< The minimum matches number, which
								will be taken into account for image objects recognition. */

	double mRequiredMatchesPart; /**< The part of matches, which will be taken
								into account for image objects recognition. Too low value will
								result in unsustainable behavior, but effect of object overlapping
								will be reduced. Value can be from 0 to 1.*/

	double mAllowableMatchesPartError; /**< Allowable error of matches number. */
};

/**
 * @brief Contains parameters for contour stabilization during tracking of image
 *        objects.
 *
 * @since_tizen 3.0
 */
struct StabilizationParams {
	StabilizationParams(
			int historyAmount,
			double allowableShift,
			double stabilizationSpeed,
			double stabilizationAcceleration);

	StabilizationParams();

	int mHistoryAmount; /**< Number of previous recognition results, which
							will influence the stabilization. */

	double mAllowableShift; /**< Relative value of maximum shift per one frame,
								which will be ignored by stabilization.
								It is relative to the object size
								in the current frame. */

	double mStabilizationSpeed; /**< Start speed with which the object will be
									stabilized. */

	double mStabilizationAcceleration; /**< Acceleration with which the object
									will be stabilized.
									It is relative to the distance from
									current location to stabilized location.
									Value can be from 0 to 1.*/
};

/**
 * @brief Contains parameters for image objects tracking.
 *
 * @since_tizen 3.0
 */
struct TrackingParams {
	TrackingParams(
			FeaturesExtractingParams framesFeaturesExtractingParams,
			RecognitionParams recognitionParams,
			StabilizationParams stabilizationParams,
			double expectedOffset);

	TrackingParams();

	FeaturesExtractingParams mFramesFeaturesExtractingParams; /**< Parameters
														for extracting features
														from frames. */

	RecognitionParams mRecognitionParams; /**< Parameters for intermediate
												recognition. */

	StabilizationParams mStabilizationParams; /**< Parameters for contour
											stabilization during tracking. */

	double mExpectedOffset; /**< Relative offset value, for which expected the
								object offset. The value is relative to
								the object size in the current frame. */
};

} /* Image */
} /* MediaVision */

#endif /* __IMAGEUTIL_H__ */
