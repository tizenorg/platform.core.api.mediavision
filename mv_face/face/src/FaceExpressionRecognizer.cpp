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

#include "FaceExpressionRecognizer.h"

#include "mv_private.h"

#include <vector>

#include <opencv/cv.h>

namespace MediaVision {
namespace Face {
static const int MinDetectionWidth = 30;
static const int MinDetectionHeight = 30;

FaceRecognizerConfig::FaceRecognizerConfig() :
	mHaarcascadeFilepath(
				"/usr/share/OpenCV/haarcascades/haarcascade_smile.xml")
{
	; /* NULL */
}

int FaceExpressionRecognizer::recognizeFaceExpression(
					const cv::Mat& grayImage,
					const mv_rectangle_s& faceLocation,
					mv_face_facial_expression_e *faceExpression,
					const FaceRecognizerConfig& config)
{
	if (NULL == faceExpression) {
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	const int smileRectHeight = cvRound((float)faceLocation.height / 2);

	const cv::Rect roi(
					faceLocation.point.x,
					faceLocation.point.y + faceLocation.height - smileRectHeight,
					faceLocation.width,
					smileRectHeight);

	if (roi.width < MinDetectionWidth ||
		roi.height < MinDetectionHeight) {
		(*faceExpression) = MV_FACE_UNKNOWN;
		return MEDIA_VISION_ERROR_NONE;
	}

	if (0 > roi.x ||
		0 > roi.y ||
		roi.x + roi.width > grayImage.cols ||
		roi.y + roi.height > grayImage.rows) {
		return MEDIA_VISION_ERROR_INVALID_OPERATION;
	}

	const cv::Mat mouthImg(grayImage, roi);

	std::vector<cv::Rect> areas;

	cv::CascadeClassifier smileClassifier;
	smileClassifier.load(config.mHaarcascadeFilepath);
	smileClassifier.detectMultiScale(
							mouthImg,
							areas,
							1.1,
							80,
							cv::CASCADE_FIND_BIGGEST_OBJECT |
							cv::CASCADE_DO_CANNY_PRUNING    |
							cv::CASCADE_SCALE_IMAGE,
							cv::Size(MinDetectionWidth, MinDetectionHeight));

	(*faceExpression) = MV_FACE_UNKNOWN;
	const size_t smilesFoundSize = areas.size();
	if (smilesFoundSize == 0) {
		(*faceExpression) = MV_FACE_NEUTRAL;
	} else if (smilesFoundSize == 1) {
		(*faceExpression) = MV_FACE_SMILE;
	}

	return MEDIA_VISION_ERROR_NONE;
}

} /* Face */
} /* MediaVision */
