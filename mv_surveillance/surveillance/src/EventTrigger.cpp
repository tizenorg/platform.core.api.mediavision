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

#include "EventTrigger.h"

#include <mv_private.h>
#include <mv_mask_buffer.h>
#include <mv_apply_mask.h>

namespace mediavision {
namespace surveillance {

long int EventTrigger::InternalTriggersCounter = -1l;

EventTrigger::EventTrigger(
		mv_surveillance_event_trigger_h eventTrigger,
		long int triggerId,
		int videoStreamId,
		mv_surveillance_event_occurred_cb callback,
		void *userData,
		int numberOfPoints,
		mv_point_s *roi):
	__videoStreamId(videoStreamId),
	__roi(numberOfPoints)
{
	CallbackData callbackData;
	callbackData.eventTrigger = eventTrigger;
	callbackData.callback = callback;
	callbackData.userData = userData;

	__callbackDataMap.insert(CallbackDataPair(triggerId, callbackData));

	for (int i = 0; i < numberOfPoints; ++i)
		__roi[i] = roi[i];
}

EventTrigger::~EventTrigger()
{
	; /* NULL */
}

int EventTrigger::getVideoStreamId() const
{
	return __videoStreamId;
}

bool EventTrigger::isCallbackSubscribed(long int triggerId) const
{
	return __callbackDataMap.find(triggerId) != __callbackDataMap.end();
}

bool EventTrigger::subscribeCallback(
		mv_surveillance_event_trigger_h eventTrigger,
		long int triggerId,
		mv_surveillance_event_occurred_cb callback,
		void *userData,
		int numberOfPoints,
		mv_point_s *roi)
{
	if (isCallbackSubscribed(triggerId)) {
		LOGE("Callback with id %d is already subscribed. "
			"Callback subscribing failed.", triggerId);
		return false;
	}

	CallbackData callbackData;
	callbackData.eventTrigger = eventTrigger;
	callbackData.callback = callback;
	callbackData.userData = userData;

	__callbackDataMap.insert(CallbackDataPair(triggerId, callbackData));

	/* TODO: implement support of multiple ROI */
	__roi.clear();
	__roi.resize(numberOfPoints);

	for (int i = 0; i < numberOfPoints; ++i)
		__roi[i] = roi[i];

	return true;
}

bool EventTrigger::unsubscribeCallback(long int triggerId)
{
	CallbackDataMapIter iter = __callbackDataMap.find(triggerId);

	if (iter == __callbackDataMap.end()) {
		LOGE("Callback with id %d was not subscribed. "
			"Callback unsubscribing failed.", triggerId);
		return false;
	}

	iter->second.callback = NULL;
	iter->second.userData = NULL;
	__callbackDataMap.erase(iter);

	return true;
}

bool EventTrigger::isCallbacksEmpty() const
{
	return __callbackDataMap.empty();
}

int EventTrigger::applyROIToImage(
					unsigned char *image,
					int imageWidth,
					int imageHeight,
					bool scalePoints,
					int scaleX,
					int scaleY)
{
	const size_t roiSize = __roi.size();

	if (roiSize >= 3) {
		MVPoints scaledPoints = __roi;

		if (scalePoints)
			for (size_t i = 0u; i < roiSize; ++i) {
				scaledPoints[i].x /= scaleX;
				scaledPoints[i].y /= scaleY;
			}

		unsigned char *maskBuffer = NULL;

		int error = mv_get_mask_buffer(
						imageWidth,
						imageHeight,
						scaledPoints.data(),
						(int) roiSize,
						&maskBuffer);

		if (error != MEDIA_VISION_ERROR_NONE || maskBuffer == NULL) {
			if (maskBuffer != NULL)
				delete maskBuffer;

			LOGE("Getting mask buffer failed.");
			return error;
		}

		error = mv_apply_mask(
					image,
					maskBuffer,
					imageWidth / 16 * 16,
					imageHeight,
					imageWidth,
					image);

		delete maskBuffer;

		if (error != MEDIA_VISION_ERROR_NONE) {
			LOGE("Applying mask buffer failed.");
			return error;
		}
	}

	return MEDIA_VISION_ERROR_NONE;
}

bool EventTrigger::operator==(const EventTrigger& other) const
{
	const std::string currentEventType = this->getEventType();
	const std::string otherEventType = other.getEventType();

	if (__videoStreamId != other.__videoStreamId ||
			currentEventType.compare(otherEventType) != 0 ||
			__roi.size() != other.__roi.size())
		return false;

	size_t size = __roi.size();
	for (size_t i = 0; i < size; ++i)
		if (__roi[i].x != other.__roi[i].x || __roi[i].y != other.__roi[i].y)
			return false;

	return true;
}

bool EventTrigger::operator!=(const EventTrigger& other) const
{
	return !(*this == other);
}

} /* surveillance */
} /* mediavision */
