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

#include "FaceEyeCondition.h"

#include <mv_private.h>

#include <vector>

namespace MediaVision {
namespace Face {
void FaceEyeCondition::splitEyes(
							const cv::Mat& grayImage,
							mv_rectangle_s faceLocation,
							cv::Mat& leftEye,
							cv::Mat& rightEye)
{
	leftEye = grayImage.rowRange(0, grayImage.rows / 2 - grayImage.rows / 10)
						.colRange(grayImage.cols / 2 + grayImage.cols / 10,
								grayImage.cols)
						.clone();

	rightEye = grayImage.rowRange(grayImage.rows / 2 + grayImage.rows / 10,
									grayImage.rows)
						.colRange(grayImage.cols / 2 + grayImage.cols / 10,
									grayImage.cols)
						.clone();

	const cv::Rect faceRect(
						faceLocation.point.x,
						faceLocation.point.y,
						faceLocation.width,
						faceLocation.height);

	const cv::Rect eyeAreaRight(
						faceRect.x + faceRect.width / 16,
						(int) (faceRect.y + (faceRect.height / 4.5)),
						(faceRect.width - 2 * faceRect.width / 16) / 2,
						(int) (faceRect.height / 3.0));

	const cv::Rect eyeAreaLeft(
						faceRect.x + faceRect.width / 16
						+ (faceRect.width - 2 * faceRect.width / 16) / 2,
						(int) (faceRect.y + (faceRect.height / 4.5)),
						(faceRect.width - 2 * faceRect.width / 16) / 2,
						(int) (faceRect.height / 3.0));

	const double xLeftEyeCenter = (2 * eyeAreaLeft.x + eyeAreaLeft.width) / 2.;
	const double yLeftEyeCenter = (2 * eyeAreaLeft.y + eyeAreaLeft.height) / 2.;

	const double xRightEyeCenter = (2 * eyeAreaRight.x + eyeAreaRight.width) / 2.;
	const double yRightEyeCenter = (2 * eyeAreaRight.y + eyeAreaRight.height) / 2.;

	const cv::Rect leftEyeRect((int)(xLeftEyeCenter - (double)eyeAreaLeft.width / 4),
								(int)(yLeftEyeCenter - (double)eyeAreaLeft.height / 4),
								eyeAreaLeft.width / 2,
								eyeAreaLeft.height / 2);

	const cv::Rect rightEyeRect((int)(xRightEyeCenter - (double)eyeAreaRight.width / 4),
								(int)(yRightEyeCenter - (double)eyeAreaRight.height / 4),
								eyeAreaRight.width / 2,
								eyeAreaRight.height / 2);

	cv::resize(
			grayImage(leftEyeRect),
			leftEye,
			leftEye.size());
    cv::resize(
			grayImage(rightEyeRect),
			rightEye,
			rightEye.size());
}

int FaceEyeCondition::isEyeOpen(const cv::Mat& eye)
{
	int isOpen = MV_FACE_EYES_CLOSED;

	cv::Mat eyeEqualized;
	cv::equalizeHist(eye, eyeEqualized);

	const int thresold = 8;
	eyeEqualized = eyeEqualized < thresold;

	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;

	cv::findContours(
			eyeEqualized,
			contours,
			hierarchy,
			CV_RETR_CCOMP,
			CV_CHAIN_APPROX_SIMPLE);

	const size_t contoursSize = contours.size();

	if (!contoursSize) {
		return MV_FACE_EYES_NOT_FOUND;
	}

	const int xCenter = eyeEqualized.cols / 2;
	const int yCenter = eyeEqualized.rows / 2;
	const int width = eyeEqualized.cols / 2.5;
	const int height = eyeEqualized.rows / 2.5;

	const cv::Rect boundThresold(xCenter - width, yCenter - height, 2 * width, 2 * height);

	const int widthHeightRatio = 3;
	const double areaRatio = 0.005;
	const double areaSmallRatio = 0.0005;
	size_t rectanglesInsideCount = 0u;

	for (size_t i = 0; i < contoursSize; ++i) {
		const cv::Rect currentRect = cv::boundingRect(contours[i]);
		const double currentArea = cv::contourArea(contours[i]);

		if (boundThresold.contains(currentRect.br()) &&
			boundThresold.contains(currentRect.tl()) &&
			currentArea > areaRatio * boundThresold.area() &&
			currentRect.width < widthHeightRatio * currentRect.height) {
			isOpen = MV_FACE_EYES_OPEN;
		} else if (boundThresold.contains(currentRect.br()) &&
			boundThresold.contains(currentRect.tl()) &&
		currentArea > areaSmallRatio * boundThresold.area()) {
			++rectanglesInsideCount;
		}
	}

	if (rectanglesInsideCount > 8u) {
		isOpen = MV_FACE_EYES_CLOSED;
	}

	return isOpen;
}

int FaceEyeCondition::recognizeEyeCondition(
		const cv::Mat& grayImage,
		mv_rectangle_s faceLocation,
		mv_face_eye_condition_e *eyeCondition)
{
	if (grayImage.empty()) {
		*eyeCondition = MV_FACE_EYES_NOT_FOUND;

		LOGE("Input image is empty. Eye condition recognition failed.");
		return MEDIA_VISION_ERROR_NO_DATA;
	}

	if (faceLocation.height <= 0 || faceLocation.width <= 0 ||
		faceLocation.point.x < 0 || faceLocation.point.y < 0 ||
		(faceLocation.point.x + faceLocation.width) > grayImage.cols ||
		(faceLocation.point.y + faceLocation.height) > grayImage.rows) {
		*eyeCondition = MV_FACE_EYES_NOT_FOUND;

		LOGE("Input face location is wrong. Eye condition recognition failed.");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	if (NULL == eyeCondition) {
		LOGE("Output eye condition is NULL. Eye condition recognition failed.");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	/* split left and right eyes */
	cv::Mat leftEye;
	cv::Mat rightEye;
	splitEyes(grayImage, faceLocation, leftEye, rightEye);

	/* recognize eyes conditions */
	const int isOpenLeft = isEyeOpen(leftEye);

	if (isOpenLeft == MV_FACE_EYES_CLOSED) {
		*eyeCondition = MV_FACE_EYES_CLOSED;

		return MEDIA_VISION_ERROR_NONE;
	} else if (isOpenLeft == MV_FACE_EYES_NOT_FOUND) {
		*eyeCondition = MV_FACE_EYES_NOT_FOUND;

		return MEDIA_VISION_ERROR_NONE;
	}

	const int isOpenRight = isEyeOpen(rightEye);

	if (isOpenRight == MV_FACE_EYES_OPEN) {
		*eyeCondition = MV_FACE_EYES_OPEN;
	} else if (isOpenRight == MV_FACE_EYES_CLOSED) {
		*eyeCondition = MV_FACE_EYES_CLOSED;
	} else {
		*eyeCondition = MV_FACE_EYES_NOT_FOUND;
	}

	return MEDIA_VISION_ERROR_NONE;
}

} /* Face */
} /* MediaVision */
