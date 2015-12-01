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

#include "EventTriggerMovementDetection.h"

#include "EventDefs.h"
#include "mv_absdiff.h"
#include "SurveillanceHelper.h"

#include <mv_private.h>

#include "opencv2/highgui/highgui.hpp"

namespace mediavision {
namespace surveillance {

static const int DEFAULT_DIFF_THRESHOLD = 10;

static const int MAX_VALUE_NAME_LENGTH = 255;

const cv::Mat EventTriggerMovementDetection::__ERODE_KERNEL =
	cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4));

const cv::Mat EventTriggerMovementDetection::__DILATE_KERNEL =
	cv::getStructuringElement(cv::MORPH_RECT, cv::Size(24, 24));

static const cv::Rect DEFAULT_RECT = cv::Rect(0, 0, 0, 0);

namespace {

inline void convertRectCV2MV(const cv::Rect& src, mv_rectangle_s& dst)
{
	dst.point.x = src.x;
	dst.point.y = src.y;
	dst.width = src.width;
	dst.height = src.height;
}

void mergeOverlappedRects(CVRectangles& rects)
{
	const size_t rectsSize = rects.size();

	for (size_t i = 0; i < rectsSize; ++i) {
		const int area1 = rects[i].area();

		for (size_t j = i + 1; j < rectsSize; ++j) {
			const int area2 = rects[j].area();
			const int intersectionArea = (rects[i] & rects[j]).area();

			if (intersectionArea != 0 &&
				intersectionArea > std::min(area1, area2) / 2) {
				rects[i] |= rects[j];
				rects[j] = DEFAULT_RECT;
			}
		}
	}
}

} /* anonymous namespace */

int EventResultMovementDetection::getResultValue(
	const char *valueName,
	void *value) const
{
	if (valueName == NULL) {
		LOGE("Invalid pointer for value name. Getting result value failed.");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	if (value == NULL) {
		LOGE("Invalid pointer for value. Getting result value failed.");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	if (strncmp(valueName, MV_SURVEILLANCE_MOVEMENT_NUMBER_OF_REGIONS,
				MAX_VALUE_NAME_LENGTH) == 0) {
		size_t *const numberOfDetectedMovements = (size_t*) value;
		*numberOfDetectedMovements = __movementRegions.size();
	} else if (strncmp(valueName, MV_SURVEILLANCE_MOVEMENT_REGIONS,
				MAX_VALUE_NAME_LENGTH) == 0) {
		mv_rectangle_s *const movementsRegions = (mv_rectangle_s*) value;

		const size_t numberOfDetectedMovements = __movementRegions.size();

		for (size_t i = 0u; i < numberOfDetectedMovements; ++i) {
			movementsRegions[i] = __movementRegions[i];
		}
	} else {
		LOGE("This value name doesn't exist. Getting result value failed.");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	return MEDIA_VISION_ERROR_NONE;
}

EventTriggerMovementDetection::EventTriggerMovementDetection(
		mv_surveillance_event_trigger_h eventTrigger,
		long int triggerId,
		int videoStreamId,
		mv_surveillance_event_occurred_cb callback,
		void *userData,
		int numberOfPoints,
		mv_point_s *roi) : EventTrigger(
								eventTrigger,
								triggerId,
								videoStreamId,
								callback,
								userData,
								numberOfPoints,
								roi),
		__previousImage(),
		__eventResult(new EventResultMovementDetection()),
		__diffThreshold(DEFAULT_DIFF_THRESHOLD)
{
	; /* NULL */
}

EventTriggerMovementDetection::~EventTriggerMovementDetection()
{
	delete __eventResult;
}

int EventTriggerMovementDetection::parseEngineConfig(mv_engine_config_h engineConfig)
{
	if (NULL == engineConfig) {
		LOGI("Default value for movement detection threshold was set.");
		return MEDIA_VISION_ERROR_NONE;
	}

	const int error = mv_engine_config_get_int_attribute(
			engineConfig,
			MV_SURVEILLANCE_MOVEMENT_DETECTION_THRESHOLD,
			&__diffThreshold);

	if (error != MEDIA_VISION_ERROR_NONE) {
		LOGE("Getting movement detection threshold from engine configuration failed.");
		return error;
	}

	return MEDIA_VISION_ERROR_NONE;
}

int EventTriggerMovementDetection::pushSource(
		mv_source_h source,
		mv_source_h graySource,
		const cv::Mat& grayImage)
{
	if (source == NULL || graySource == NULL || grayImage.empty()) {
		LOGE("Media source is NULL. Pushing source failed.");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	__eventResult->__movementRegions.clear();

	int error = MEDIA_VISION_ERROR_NONE;

	if (__previousImage.empty()) {
		__previousImage = grayImage.clone();

		LOGI("Previous media source is empty. Push next source.");
		return error;
	}

	cv::Mat image = grayImage.clone();

	const int bufSize = image.cols * image.rows * sizeof(uint8_t);
	uint8_t *diffBuffer = (uint8_t*) malloc(bufSize * sizeof(uint8_t));
	memset(diffBuffer, 0, bufSize);

	error = mv_absdiff(
			image.data,
			__previousImage.data,
			image.cols,
			image.rows,
			image.cols,
			diffBuffer);

	if (error != MEDIA_VISION_ERROR_NONE) {
		LOGE("Absolute difference calculation failed. Pushing source failed.");
		return error;
	}

	error = applyROIToImage(diffBuffer, image.cols, image.rows);

	if (error != MEDIA_VISION_ERROR_NONE || image.empty()) {
		LOGE("Applying ROI failed with error %d.", error);
		return error;
	}

	cv::Mat imgDiff = cv::Mat(cv::Size(image.cols, image.rows),
			CV_8UC1, diffBuffer);

	cv::erode(imgDiff, imgDiff, __ERODE_KERNEL);
	cv::dilate(imgDiff, imgDiff, __DILATE_KERNEL);

	cv::threshold(imgDiff, imgDiff, __diffThreshold, 255, CV_THRESH_BINARY);

	Contours contours;

	cv::findContours(imgDiff, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	free(diffBuffer);

	const size_t contoursSize = contours.size();
	CVRectangles rects(contoursSize);

	for (size_t i = 0u; i < contoursSize; ++i)
		rects[i] = cv::boundingRect(cv::Mat(contours[i]));

	mergeOverlappedRects(rects);

	const size_t roiSize = __roi.size();
	CVPoints roi(roiSize);

	cv::Rect roiRect(0, 0, imgDiff.cols, imgDiff.rows);

	if (roiSize >= 3u) {
		for (size_t i = 0u; i < roiSize; ++i)
			roi[i] = cv::Point(__roi[i].x, __roi[i].y);

		roiRect = cv::boundingRect(roi);
	}

	const size_t rectsSize = rects.size();
	for (size_t i = 0u; i < rectsSize; ++i)
		if (rects[i] != DEFAULT_RECT &&
			roiRect.contains(rects[i].tl()) &&
			roiRect.contains(rects[i].br())) {
			mv_rectangle_s rectMV;
			convertRectCV2MV(rects[i], rectMV);

			__eventResult->__movementRegions.push_back(rectMV);
		}

	__previousImage = image;
	__eventResult->__grayImage = __previousImage;

	// Don't invoke the callback if movement wasn't detected at the frame
	if (__eventResult->__movementRegions.size() > 0) {
		CallbackDataMapConstIter iter = __callbackDataMap.begin();

		for (; iter != __callbackDataMap.end(); ++iter) {
			mv_surveillance_event_occurred_cb callback = iter->second.callback;
			callback(
				iter->second.eventTrigger,
				source,
				__videoStreamId,
				__eventResult,
				iter->second.userData);
		}
	}

	return MEDIA_VISION_ERROR_NONE;
}

std::string EventTriggerMovementDetection::getEventType() const
{
	return MV_SURVEILLANCE_EVENT_TYPE_MOVEMENT_DETECTED;
}

bool EventTriggerMovementDetection::operator==(const EventTriggerMovementDetection& other) const
{
	if (EventTrigger::operator !=(other))
		return false;

	/* TODO: compare private values if necessary */

	return true;
}

bool EventTriggerMovementDetection::operator!=(const EventTriggerMovementDetection& other) const
{
	return !(*this == other);
}

} /* surveillance */
} /* mediavision */
