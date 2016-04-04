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

#ifndef __IMAGECONFIG_H__
#define __IMAGECONFIG_H__

#include <string>

/**
 * @file  ImageConfig.h
 * @brief This file contains Image Module Configuration.
 */

namespace MediaVision {
namespace Image {
/**
 * @brief Keypoint's type enumeration.
 *
 * @since_tizen 3.0
 */
enum EXPORT_API KeypointType {
	KT_INVALID = -1,  /**< Undefined keypoint's type */
	KT_ORB,           /**< Oriented FAST keypoint's type */
	KT_GFTT,          /**< Keypoint's type of good features to track */
	KT_SIZE           /**< Number of keypoint's types */
};

const std::string KeypointNames[KT_SIZE] = {
	[KT_ORB] = "ORB",
	[KT_GFTT] = "GFTT"
};

/*
 * @brief Descriptor's type enumeration.
 *
 * @since_tizen 3.0
 */
enum EXPORT_API DescriptorType {
	DT_INVALID = -1,  /**< Undefined descriptor's type */
	DT_ORB,           /**< Rotated BRIEF descriptor's type */
	DT_BRIEF,         /**< Descriptor's type of binary robust independent
							elementary features */
	DT_SIZE           /**< Number of descriptor's types */
};

const std::string DescriptorNames[DT_SIZE] = {
        [DT_ORB] = "ORB",
        [DT_BRIEF] = "BRIEF"
};

/**
 * @brief Contains parameters for features extracting from image objects.
 *
 * @since_tizen 3.0
 */
struct EXPORT_API FeaturesExtractingParams {
	FeaturesExtractingParams();

	KeypointType mKeypointType; /**< Keypoint's type. */

	DescriptorType mDescriptorType; /**< Descriptor's type. */

	union { /**< Extracting parameters for concretes algorithms */
		struct { /**< Extracting parameters for ORB algorithm. */
			double mScaleFactor; /**< Recognition scale factor for the ORB detector. */
			int mMaximumFeaturesNumber; /**< Maximum number of features,
									which will be extracted from object image.*/
		} ORB;
	};
};

/**
 * @class RecognitionParams
 * @brief Contains parameters for image objects recognition.
 *
 * @since_tizen 3.0
 */
struct EXPORT_API RecognitionParams {
	RecognitionParams(
			int minMatchesNumber,
			double requiredMatchesPart,
			double tolerantMatchesPartError);

	RecognitionParams();

	int mMinMatchesNumber; /**< The minimum matches number, which
								will be taken into account for image objects recognition. */

	double mRequiredMatchesPart; /**< The part of matches, which will be taken
								into account for image objects recognition. Too low value will
								result in unsustainable behavior, but effect of object overlapping
								will be reduced. Value can be from 0 to 1.*/

	double mTolerantMatchesPartError; /**< Tolerant error of matches number. */
};

/**
 * @class StabilizationParams
 * @brief Contains parameters for contour stabilization during tracking of image
 *        objects.
 *
 * @since_tizen 3.0
 */
struct EXPORT_API StabilizationParams {
	StabilizationParams(
			bool isEnabled,
			int historyAmount,
			double tolerantShift,
			double tolerantShiftExtra,
			double stabilizationSpeed,
			double stabilizationAcceleration);

	StabilizationParams();

	bool mIsEnabled; /**< Flag that specifies whether to use the stabilization. */

	int mHistoryAmount; /**< Number of previous recognition results, which
							will influence the stabilization. */

	double mTolerantShift; /**< Relative value of maximum shift per one frame,
								which will be ignored by stabilization.
								It is relative to the object size
								in the current frame. */

	double mTolerantShiftExtra; /**< Constant value which will be added to
									maximum shift per one frame,
									which will be ignored by stabilization. */

	double mStabilizationSpeed; /**< Start speed with which the object will be
									stabilized. */

	double mStabilizationAcceleration; /**< Acceleration with which the object
									will be stabilized.
									It is relative to the distance from
									current location to stabilized location.
									Value can be from 0 to 1.*/
};

/**
 * @calss TrackingParams
 * @brief Contains parameters for image objects tracking.
 *
 * @since_tizen 3.0
 */
struct EXPORT_API TrackingParams {
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

#endif /* __IMAGECONFIG_H__ */
