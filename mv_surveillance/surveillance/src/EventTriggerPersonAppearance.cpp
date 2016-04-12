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

#include "EventTriggerPersonAppearance.h"

#include "EventManager.h"
#include "FaceDetector.h"
#include "SurveillanceHelper.h"
#include "EventTriggerMovementDetection.h"

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <mv_private.h>

#include <sys/time.h>
#include <unistd.h>

namespace mediavision {
namespace surveillance {

using namespace cv;

static const int MAX_VALUE_NAME_LENGHT = 255;

static const int DEFAULT_SKIP_FRAMES_COUNT = 6;

static const int DEFAULT_FRAME_WIDTH = 640;

static const int DEFAULT_FRAME_HEIGHT = 480;

static const cv::Size DEFAULT_FRAME_SIZE(DEFAULT_FRAME_WIDTH, DEFAULT_FRAME_HEIGHT);

static const cv::Rect ALL_IMAGE_RECT(0, 0, DEFAULT_FRAME_WIDTH, DEFAULT_FRAME_HEIGHT);

static const cv::Size DEFAULT_DETECTION_STEPS = cv::Size(8, 8);

static const std::vector<float> DEFAULT_SVM_PEOPLE_DETECTOR =
		cv::HOGDescriptor::getDefaultPeopleDetector();

namespace {

inline void convertRectMV2CV(const mv_rectangle_s& src, cv::Rect& dst)
{
	dst.x = src.point.x;
	dst.y = src.point.y;
	dst.width = src.width;
	dst.height = src.height;
}

inline void convertRectCV2MV(const cv::Rect& src, mv_rectangle_s& dst)
{
	dst.point.x = src.x;
	dst.point.y = src.y;
	dst.width = src.width;
	dst.height = src.height;
}

} /* Anonymous namespace*/

int EventResultPersonAppearance::getResultValue(const char *valueName,
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

	if (strncmp(valueName, MV_SURVEILLANCE_PERSONS_APPEARED_NUMBER,
			MAX_VALUE_NAME_LENGHT) == 0) {
		size_t * const numberOfAppearedPersons = (size_t*) value;
		*numberOfAppearedPersons = __appearedLocations.size();
	} else if (strncmp(valueName, MV_SURVEILLANCE_PERSONS_APPEARED_LOCATIONS,
			MAX_VALUE_NAME_LENGHT) == 0) {
		mv_rectangle_s * const appearedLocations = (mv_rectangle_s*) value;

		const size_t numberOfAppearedPersons = __appearedLocations.size();

		for (size_t i = 0u; i < numberOfAppearedPersons; ++i)
			appearedLocations[i] = __appearedLocations[i];
	} else if (strncmp(valueName, MV_SURVEILLANCE_PERSONS_TRACKED_NUMBER,
			MAX_VALUE_NAME_LENGHT) == 0) {
		size_t * const numberOfTrackedPersons = (size_t*) value;
		*numberOfTrackedPersons = __trackedLocations.size();
	} else if (strncmp(valueName, MV_SURVEILLANCE_PERSONS_TRACKED_LOCATIONS,
			MAX_VALUE_NAME_LENGHT) == 0) {
		mv_rectangle_s * const trackedLocations = (mv_rectangle_s*) value;

		const size_t numberOfTrackedPersons = __trackedLocations.size();

		for (size_t i = 0u; i < numberOfTrackedPersons; ++i)
			trackedLocations[i] = __trackedLocations[i];
	} else if (strncmp(valueName, MV_SURVEILLANCE_PERSONS_DISAPPEARED_NUMBER,
			MAX_VALUE_NAME_LENGHT) == 0) {
		size_t * const numberOfDisappearedPersons = (size_t*) value;
		*numberOfDisappearedPersons = __disappearedLocations.size();
	} else if (strncmp(valueName, MV_SURVEILLANCE_PERSONS_DISAPPEARED_LOCATIONS,
			MAX_VALUE_NAME_LENGHT) == 0) {
		mv_rectangle_s * const disappearedLocations = (mv_rectangle_s*) value;

		const size_t numberOfDisappearedPersons = __disappearedLocations.size();

		for (size_t i = 0u; i < numberOfDisappearedPersons; ++i)
			disappearedLocations[i] = __disappearedLocations[i];
	} else {
		LOGE("This value name doesn't exist. Getting result value failed.");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	return MEDIA_VISION_ERROR_NONE;
}

EventTriggerPersonAppearance::EventTriggerPersonAppearance(
		mv_surveillance_event_trigger_h eventTrigger, long int triggerId,
		int videoStreamId, mv_surveillance_event_occurred_cb callback,
		void *userData, int numberOfPoints, mv_point_s *roi) :
				EventTrigger(eventTrigger, triggerId, videoStreamId, callback, userData,
				numberOfPoints, roi), __skipFramesCount(DEFAULT_SKIP_FRAMES_COUNT),
				__frameCounter(0), __movementDetectedEventId(InternalTriggersCounter--),
				__factorX(1.f), __factorY(1.f), __rectToDetect(ALL_IMAGE_RECT),
				__rectToDetectPrevious(ALL_IMAGE_RECT), __trackedRects(),
				__appearedRects(), __disappearedRects(), __hogClassifier(),
				__eventResult(new EventResultPersonAppearance())
{
	__hogClassifier.setSVMDetector(DEFAULT_SVM_PEOPLE_DETECTOR);

	EventManager::getInstance().registerEvent(
	NULL, __movementDetectedEventId,
	MV_SURVEILLANCE_EVENT_TYPE_MOVEMENT_DETECTED, videoStreamId,
	NULL, movementDetectedCB, this, numberOfPoints, roi);
}

EventTriggerPersonAppearance::~EventTriggerPersonAppearance()
{
	EventManager::getInstance().unregisterEvent(__movementDetectedEventId,
			__videoStreamId);

	delete __eventResult;
}

int EventTriggerPersonAppearance::parseEngineConfig(
		mv_engine_config_h engineConfig)
{
	if (NULL == engineConfig) {
		LOGI("Default value for frame skip count was set.");
		return MEDIA_VISION_ERROR_NONE;
	}

	const int error = mv_engine_config_get_int_attribute(engineConfig,
	MV_SURVEILLANCE_SKIP_FRAMES_COUNT, &__skipFramesCount);

	if (error != MEDIA_VISION_ERROR_NONE) {
		LOGE("Getting frame skip count from engine configuration failed.");
		return error;
	}

	return MEDIA_VISION_ERROR_NONE;
}

int EventTriggerPersonAppearance::pushSource(mv_source_h source,
		mv_source_h graySource, const cv::Mat& grayImage)
{
	if (source == NULL || graySource == NULL || grayImage.empty()) {
		LOGE("Media source is NULL. Pushing source failed.");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	return MEDIA_VISION_ERROR_NONE;
}

std::string EventTriggerPersonAppearance::getEventType() const
{
	return MV_SURVEILLANCE_EVENT_TYPE_PERSON_APPEARED_DISAPPEARED;
}

bool EventTriggerPersonAppearance::operator==(
		const EventTriggerPersonAppearance& other) const
{
	if (EventTrigger::operator !=(other))
		return false;

	/* TODO: compare private values if necessary */

	return true;
}

bool EventTriggerPersonAppearance::operator!=(
		const EventTriggerPersonAppearance& other) const
{
	return !(*this == other);
}

void EventTriggerPersonAppearance::movementDetectedCB(
		mv_surveillance_event_trigger_h /*event_trigger*/, mv_source_h source,
		int /*video_stream_id*/, mv_surveillance_result_h event_result,
		void *user_data)
{
	EventTriggerPersonAppearance *trigger =
			(EventTriggerPersonAppearance*) user_data;

	/* 1. Get input image in grayscale and resize it */
	EventResultMovementDetection *result =
			static_cast<EventResultMovementDetection*>(event_result);

	cv::Mat resizedImage;
	cv::resize(result->__grayImage, resizedImage, DEFAULT_FRAME_SIZE);

	int error = trigger->applyROIToImage(resizedImage.data, resizedImage.cols,
			resizedImage.rows, true, DEFAULT_FRAME_WIDTH, DEFAULT_FRAME_HEIGHT);

	if (error != MEDIA_VISION_ERROR_NONE || resizedImage.empty()) {
		trigger->runCallbacks(source);
		LOGE("Applying ROI failed with error %d.", error);
		return;
	}

	trigger->__factorX = (float) DEFAULT_FRAME_WIDTH / result->__grayImage.cols;
	trigger->__factorY = (float) DEFAULT_FRAME_HEIGHT / result->__grayImage.rows;

	/* 2. Get detected movement regions */
	const size_t numberOfMovementRegions = result->__movementRegions.size();
	CVRectangles movementRegions(numberOfMovementRegions);

	for (size_t i = 0u; i < numberOfMovementRegions; ++i) {
		convertRectMV2CV(result->__movementRegions[i], movementRegions[i]);
		movementRegions[i].x *= trigger->__factorX;
		movementRegions[i].y *= trigger->__factorY;
		movementRegions[i].width *= trigger->__factorX;
		movementRegions[i].height *= trigger->__factorY;
	}

	/* 3. Calculate rectangle where person will be detect */
	if (movementRegions.empty()) {
		trigger->__rectToDetect = ALL_IMAGE_RECT;
	} else {
		trigger->__rectToDetect = movementRegions[0];

		for (size_t j = 1u; j < numberOfMovementRegions; ++j)
			trigger->__rectToDetect |= movementRegions[j];

		if (trigger->__rectToDetect.width
				< trigger->__hogClassifier.winSize.width
				|| trigger->__rectToDetect.height
						< trigger->__hogClassifier.winSize.height)
			trigger->__rectToDetect |= trigger->__rectToDetectPrevious;
	}

	trigger->__rectToDetect &= ALL_IMAGE_RECT;

	/* 4. Perform Hog detector or try to track using movement regions */
	if ((trigger->__skipFramesCount == 0 ||
			trigger->__frameCounter % trigger->__skipFramesCount == 0) &&
			(trigger->__rectToDetect != ALL_IMAGE_RECT)) {
		/* 4.1 Perform Hog detector */
		TrackedRectanglesConstIter iter = trigger->__trackedRects.begin();
		for (; iter != trigger->__trackedRects.end(); ++iter)
			trigger->__rectToDetect |= iter->rect;

		// Slightly extend detection area...
		const int xShift = .25f * trigger->__rectToDetect.width;
		const int yShift = .25f * trigger->__rectToDetect.height;
		trigger->__rectToDetect.x -= xShift / 2;
		trigger->__rectToDetect.y -= yShift / 2;
		trigger->__rectToDetect.width += xShift;
		trigger->__rectToDetect.height += yShift;
		trigger->__rectToDetect &= ALL_IMAGE_RECT;
		// and fit it to the HOG cell size
		const int xRest = trigger->__rectToDetect.width % 8;
		const int yRest = trigger->__rectToDetect.height % 8;
		trigger->__rectToDetect.x += xRest / 2;
		trigger->__rectToDetect.y += yRest / 2;
		trigger->__rectToDetect.width -= xRest;
		trigger->__rectToDetect.height -= yRest;

		CVRectangles hogRects;

		trigger->__hogClassifier.detectMultiScale(
				resizedImage(trigger->__rectToDetect), hogRects, 0,
				DEFAULT_DETECTION_STEPS, cv::Size(32, 32), 1.059, 2);

		const size_t hogRectsSize = hogRects.size();

		for (size_t i = 0u; i < hogRectsSize; ++i) {
			hogRects[i].x += trigger->__rectToDetect.x;
			hogRects[i].y += trigger->__rectToDetect.y;
		}

		std::vector<bool> trackChecks(hogRectsSize, false);
		TrackedRectanglesIter trackRectIter = trigger->__trackedRects.begin();
		for (; trackRectIter != trigger->__trackedRects.end();
				++trackRectIter) {
			size_t bestArea = 0;
			size_t bestIdx = 0;
			for (size_t idx = 0u; idx < hogRectsSize; ++idx) {
				if (trackChecks[idx])
					continue;
				const size_t curArea =
						(hogRects[idx] & trackRectIter->rect).area();
				if (bestArea < curArea) {
					bestArea = curArea;
					bestIdx = idx;
				}
			}
			if (bestArea > 10) {
				trackChecks[bestIdx] = true;
				trackRectIter->rect = hogRects[bestIdx];
			}
		}

		trigger->__appearedRects.clear();
		for (size_t idx = 0u; idx < hogRectsSize; ++idx)
			if (!trackChecks[idx])
				trigger->__appearedRects.push_back(hogRects[idx]);
	} else {
		/* 4.2 Try to track */
		CVRectanglesConstIter appearedIter = trigger->__appearedRects.begin();
		for (; appearedIter != trigger->__appearedRects.end(); ++appearedIter)
			trigger->__trackedRects.push_back(
					TrackedRectangle(*appearedIter, 7));

		trigger->__appearedRects.clear();

		TrackedRectanglesIter iter = trigger->__trackedRects.begin();
		while (iter != trigger->__trackedRects.end()) {
			bool tracked = false;

			for (size_t j = 0u; j < numberOfMovementRegions; ++j) {
				cv::Rect rect = iter->rect;
				if ((rect & movementRegions[j]).area() != 0 &&
						movementRegions[j].area() <= 3 * rect.area() / 2) {
					cv::Rect r1 = rect | movementRegions[j];
					const int dx = r1.width - rect.width;
					const int dy = r1.height - rect.height;

					if (r1.x < movementRegions[j].x)
						r1.x += dx;
					else if (r1.x > movementRegions[j].x)
						r1.x -= dx;

					if (r1.y < movementRegions[j].y)
						r1.y += dy;
					else if (r1.y > movementRegions[j].y)
						r1.y -= dy;

					r1.height = rect.height;
					r1.width = rect.width;

					iter->rect = r1;

					tracked = true;
				}
			}

			if (tracked) {
				++iter;
			} else {
				if (iter->framesCount == 0) {
					trigger->__disappearedRects.push_back(iter->rect);
					iter = trigger->__trackedRects.erase(iter);
				} else {
					--(iter->framesCount);
					++iter;
				}
			}
		}
	}

	trigger->__rectToDetectPrevious = trigger->__rectToDetect;
	++trigger->__frameCounter;

	/* 5. Update event result and run callbacks */
	trigger->runCallbacks(source);

	trigger->__disappearedRects.clear();
}

void EventTriggerPersonAppearance::runCallbacks(mv_source_h source)
{
	__eventResult->__appearedLocations.clear();
	__eventResult->__disappearedLocations.clear();
	__eventResult->__trackedLocations.clear();

	const size_t appearedLocationsSize = __appearedRects.size();
	__eventResult->__appearedLocations.resize(appearedLocationsSize);

	for (size_t i = 0u; i < appearedLocationsSize; ++i) {
		convertRectCV2MV(__appearedRects[i],
				__eventResult->__appearedLocations[i]);
		__eventResult->__appearedLocations[i].point.x /= __factorX;
		__eventResult->__appearedLocations[i].point.y /= __factorY;
		__eventResult->__appearedLocations[i].width /= __factorX;
		__eventResult->__appearedLocations[i].height /= __factorY;
	}

	const size_t disappearedLocationsSize = __disappearedRects.size();
	__eventResult->__disappearedLocations.resize(disappearedLocationsSize);

	for (size_t i = 0u; i < disappearedLocationsSize; ++i) {
		convertRectCV2MV(__disappearedRects[i],
				__eventResult->__disappearedLocations[i]);
		__eventResult->__disappearedLocations[i].point.x /= __factorX;
		__eventResult->__disappearedLocations[i].point.y /= __factorY;
		__eventResult->__disappearedLocations[i].width /= __factorX;
		__eventResult->__disappearedLocations[i].height /= __factorY;
	}

	const size_t trackedLocationsSize = __trackedRects.size();
	__eventResult->__trackedLocations.resize(trackedLocationsSize);

	TrackedRectanglesConstIter trackedIter = __trackedRects.begin();
	for (size_t i = 0u; i < trackedLocationsSize; ++i, ++trackedIter) {
		convertRectCV2MV(trackedIter->rect,
				__eventResult->__trackedLocations[i]);
		__eventResult->__trackedLocations[i].point.x /= __factorX;
		__eventResult->__trackedLocations[i].point.y /= __factorY;
		__eventResult->__trackedLocations[i].width /= __factorX;
		__eventResult->__trackedLocations[i].height /= __factorY;
	}

	// Don't invoke the callback if no appearance, disappearance or tracking
	if (appearedLocationsSize > 0 || disappearedLocationsSize > 0
			|| trackedLocationsSize > 0) {
		CallbackDataMapConstIter iter = __callbackDataMap.begin();

		for (; iter != __callbackDataMap.end(); ++iter) {
			mv_surveillance_event_occurred_cb callback = iter->second.callback;
			callback(iter->second.eventTrigger, source, __videoStreamId,
					__eventResult, iter->second.userData);
		}
	}
}

} /* surveillance */
} /* mediavision */
