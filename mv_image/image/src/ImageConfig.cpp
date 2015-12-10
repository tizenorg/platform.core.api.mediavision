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

#include "ImageConfig.h"

namespace MediaVision {
namespace Image {
FeaturesExtractingParams::FeaturesExtractingParams() :
	mKeypointType(KT_INVALID),
	mDescriptorType(DT_INVALID)
{
	; /* NULL */
}

RecognitionParams::RecognitionParams(
					int minMatchesNumber,
					double requiredMatchesPart,
					double tolerantMatchesPartError) :
	mMinMatchesNumber(minMatchesNumber),
	mRequiredMatchesPart(requiredMatchesPart),
	mTolerantMatchesPartError(tolerantMatchesPartError)
{
	; /* NULL */
}

RecognitionParams::RecognitionParams() :
        mMinMatchesNumber(0),
        mRequiredMatchesPart(1.0),
        mTolerantMatchesPartError(0.0)
{
	; /* NULL */
}

StabilizationParams::StabilizationParams(
					bool isEnabled,
					int historyAmount,
					double tolerantShift,
					double tolerantShiftExtra,
					double stabilizationSpeed,
					double stabilizationAcceleration) :
	mIsEnabled(isEnabled),
	mHistoryAmount(historyAmount),
	mTolerantShift(tolerantShift),
	mTolerantShiftExtra(tolerantShiftExtra),
	mStabilizationSpeed(stabilizationSpeed),
	mStabilizationAcceleration(stabilizationAcceleration)
{
	; /* NULL */
}

StabilizationParams::StabilizationParams() :
	mIsEnabled(false),
	mHistoryAmount(1),
	mTolerantShift(0.0),
	mTolerantShiftExtra(0.0),
	mStabilizationSpeed(0.0),
	mStabilizationAcceleration(1.0)
{
	; /* NULL */
}

TrackingParams::TrackingParams(
				FeaturesExtractingParams framesFeaturesExtractingParams,
				RecognitionParams recognitionParams,
				StabilizationParams stabilizationParams,
				double expectedOffset) :
	mFramesFeaturesExtractingParams(framesFeaturesExtractingParams),
	mRecognitionParams(recognitionParams),
	mStabilizationParams(stabilizationParams),
	mExpectedOffset(expectedOffset)
{
	; /* NULL */
}

TrackingParams::TrackingParams() :
	mFramesFeaturesExtractingParams(),
	mRecognitionParams(),
	mStabilizationParams(),
	mExpectedOffset(0.0)
{
	; /* NULL */
}

} /* Image */
} /* MediaVision */