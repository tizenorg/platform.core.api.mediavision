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

#include "EventManager.h"

#include "SurveillanceHelper.h"

#include "EventTriggerPersonAppearance.h"
#include "EventTriggerPersonRecognition.h"
#include "EventTriggerMovementDetection.h"

#include <mv_private.h>

namespace mediavision {
namespace surveillance {

static const int MAX_VALUE_NAME_LENGTH = 255;

EventManager *EventManager::__pInstance = 0;
EventManagerDestroyer EventManager::Destroyer;
EventTypesMap EventManager::SupportedEventTypes;

EventManagerDestroyer::~EventManagerDestroyer()
{
	delete __pInstance;
}

void EventManagerDestroyer::initialize(EventManager* pointer)
{
	__pInstance = pointer;
}

EventManager& EventManager::getInstance()
{
	if(!__pInstance) {
		__pInstance = new EventManager();
		Destroyer.initialize(__pInstance);
		setSupportedEventTypes();
	}

	return *__pInstance;
}

void EventManager::setSupportedEventTypes()
{
	/* Add supported event types here */
	SupportedEventTypes[MV_SURVEILLANCE_EVENT_TYPE_MOVEMENT_DETECTED]
		.push_back(MV_SURVEILLANCE_MOVEMENT_NUMBER_OF_REGIONS);
	SupportedEventTypes[MV_SURVEILLANCE_EVENT_TYPE_MOVEMENT_DETECTED]
		.push_back(MV_SURVEILLANCE_MOVEMENT_REGIONS);

	SupportedEventTypes[MV_SURVEILLANCE_EVENT_TYPE_PERSON_APPEARED_DISAPPEARED]
		.push_back(MV_SURVEILLANCE_PERSONS_APPEARED_NUMBER);
	SupportedEventTypes[MV_SURVEILLANCE_EVENT_TYPE_PERSON_APPEARED_DISAPPEARED]
		.push_back(MV_SURVEILLANCE_PERSONS_DISAPPEARED_NUMBER);
	SupportedEventTypes[MV_SURVEILLANCE_EVENT_TYPE_PERSON_APPEARED_DISAPPEARED]
		.push_back(MV_SURVEILLANCE_PERSONS_TRACKED_NUMBER);
	SupportedEventTypes[MV_SURVEILLANCE_EVENT_TYPE_PERSON_APPEARED_DISAPPEARED]
		.push_back(MV_SURVEILLANCE_PERSONS_APPEARED_LOCATIONS);
	SupportedEventTypes[MV_SURVEILLANCE_EVENT_TYPE_PERSON_APPEARED_DISAPPEARED]
		.push_back(MV_SURVEILLANCE_PERSONS_DISAPPEARED_LOCATIONS);
	SupportedEventTypes[MV_SURVEILLANCE_EVENT_TYPE_PERSON_APPEARED_DISAPPEARED]
		.push_back(MV_SURVEILLANCE_PERSONS_TRACKED_LOCATIONS);

	SupportedEventTypes[MV_SURVEILLANCE_EVENT_TYPE_PERSON_RECOGNIZED]
		.push_back(MV_SURVEILLANCE_PERSONS_RECOGNIZED_NUMBER);
	SupportedEventTypes[MV_SURVEILLANCE_EVENT_TYPE_PERSON_RECOGNIZED]
		.push_back(MV_SURVEILLANCE_PERSONS_RECOGNIZED_LOCATIONS);
	SupportedEventTypes[MV_SURVEILLANCE_EVENT_TYPE_PERSON_RECOGNIZED]
		.push_back(MV_SURVEILLANCE_PERSONS_RECOGNIZED_LABELS);
	SupportedEventTypes[MV_SURVEILLANCE_EVENT_TYPE_PERSON_RECOGNIZED]
		.push_back(MV_SURVEILLANCE_PERSONS_RECOGNIZED_CONFIDENCES);
}

EventManager::EventManager()
{
	; /* NULL */
}

EventManager::~EventManager()
{
	; /* NULL */
}

int EventManager::registerEvent(
					mv_surveillance_event_trigger_h eventTrigger,
					long int triggerId,
					const char *eventType,
					int videoStreamId,
					mv_engine_config_h engineCfg,
					mv_surveillance_event_occurred_cb callback,
					void *user_data,
					int numberOfPoints,
					mv_point_s *roi)
{
	if (NULL == callback || NULL == eventType) {
		LOGE("Input event trigger or callback is NULL. Event registering failed.");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	EventTriggersConstIter iter = __eventTriggers[videoStreamId].begin();

	for (; iter != __eventTriggers[videoStreamId].end(); ++iter) {
		if ((*iter)->isCallbackSubscribed(triggerId)) {
			LOGE("Callback with id %d is already subscribed. "
					"Event registering failed.", triggerId);
			return MEDIA_VISION_ERROR_INVALID_PARAMETER;
		}
	}

	/* Add appropriate event trigger here */
	if (strncmp(eventType, MV_SURVEILLANCE_EVENT_TYPE_PERSON_APPEARED_DISAPPEARED,
				MAX_VALUE_NAME_LENGTH) == 0) {
		EventTriggerPersonAppearance* trigger =
			new EventTriggerPersonAppearance(
					eventTrigger,
					triggerId,
					videoStreamId,
					callback,
					user_data,
					numberOfPoints,
					roi);
		const int error = trigger->parseEngineConfig(engineCfg);

		if (error != MEDIA_VISION_ERROR_NONE) {
			delete trigger;

			LOGE("Input engine configuration is wrong. Event registering failed.");
			return MEDIA_VISION_ERROR_INVALID_PARAMETER;
		}

		EventTriggersIter iter2 = isTriggerExists(trigger, videoStreamId);

		if (iter2 != __eventTriggers[videoStreamId].end()) {
			(*iter2)->subscribeCallback(
						eventTrigger,
						triggerId,
						callback,
						user_data,
						numberOfPoints,
						roi);

						delete trigger;
		} else {
			__eventTriggers[videoStreamId].push_back(trigger);
		}
	} else if (strncmp(eventType, MV_SURVEILLANCE_EVENT_TYPE_PERSON_RECOGNIZED,
						MAX_VALUE_NAME_LENGTH) == 0) {
		EventTriggerPersonRecognition* trigger =
						new EventTriggerPersonRecognition(
										eventTrigger,
										triggerId,
										videoStreamId,
										callback,
										user_data,
										numberOfPoints,
										roi);

		const int error = trigger->parseEngineConfig(engineCfg);
		if (error != MEDIA_VISION_ERROR_NONE) {
			delete trigger;

			LOGE("Input engine configuration is wrong. Event registering failed.");
			return MEDIA_VISION_ERROR_INVALID_PARAMETER;
		}

		EventTriggersIter iter2 = isTriggerExists(trigger, videoStreamId);

		if (iter2 != __eventTriggers[videoStreamId].end()) {
			(*iter2)->subscribeCallback(
						eventTrigger,
						triggerId,
						callback,
						user_data,
						numberOfPoints,
						roi);

			delete trigger;
		} else {
			__eventTriggers[videoStreamId].push_back(trigger);
		}
	} else if (strncmp(eventType, MV_SURVEILLANCE_EVENT_TYPE_MOVEMENT_DETECTED,
				MAX_VALUE_NAME_LENGTH) == 0) {
			EventTriggerMovementDetection* trigger =
						new EventTriggerMovementDetection(
									eventTrigger,
									triggerId,
									videoStreamId,
									callback,
									user_data,
									numberOfPoints,
									roi);

		const int error = trigger->parseEngineConfig(engineCfg);

		if (error != MEDIA_VISION_ERROR_NONE) {
			delete trigger;

			LOGE("Input engine configuration is wrong. Event registering failed.");
			return MEDIA_VISION_ERROR_INVALID_PARAMETER;
		}

		EventTriggersIter iter2 = isTriggerExists(trigger, videoStreamId);

		if (iter2 != __eventTriggers[videoStreamId].end()) {
			(*iter2)->subscribeCallback(
						eventTrigger,
						triggerId,
						callback,
						user_data,
						numberOfPoints,
						roi);

			delete trigger;
		} else {
			__eventTriggers[videoStreamId].push_back(trigger);
		}
	} else {
		LOGE("Input event trigger has wrong type. Event registering failed.");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	return MEDIA_VISION_ERROR_NONE;
}

int EventManager::unregisterEvent(long int triggerId, int videoStreamId)
{
	EventTriggersIter iter = __eventTriggers[videoStreamId].begin();

	while (iter != __eventTriggers[videoStreamId].end()) {
		if ((*iter)->unsubscribeCallback(triggerId)) {
			if ((*iter)->isCallbacksEmpty()) {
				delete *iter;
				__eventTriggers[videoStreamId].erase(iter);
			}

			return MEDIA_VISION_ERROR_NONE;
		}

		++iter;
	}

	if (iter == __eventTriggers[videoStreamId].end()) {
		LOGE("Event trigger doesn't exist. Event unregistering failed.");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	return MEDIA_VISION_ERROR_NONE;
}

int EventManager::pushSource(mv_source_h source, int videoStreamId)
{
	if (NULL == source) {
		LOGE("Input source is NULL. Push source failed.");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	if (__eventTriggers[videoStreamId].empty()) {
		LOGE("There are no events yet. Push source failed.");
		return MEDIA_VISION_ERROR_INVALID_OPERATION;
	}

	unsigned int width = 0;
	unsigned int height = 0;
	mv_colorspace_e colorspace = MEDIA_VISION_COLORSPACE_INVALID;

	MEDIA_VISION_ASSERT(mv_source_get_width(source, &width),
						"Failed to get the width.");
	MEDIA_VISION_ASSERT(mv_source_get_height(source, &height),
						"Failed to get the height.");
	MEDIA_VISION_ASSERT(mv_source_get_colorspace(source, &colorspace),
						"Failed to get the colorspace.");

	cv::Mat grayImage;

	int error = MEDIA_VISION_ERROR_NONE;

#ifdef ENABLE_NEON
	if (colorspace == MEDIA_VISION_COLORSPACE_RGB888 && (width * height % 8) == 0)
		error = SurveillanceHelper::convertSourceMVRGB2GrayCVNeon(source, grayImage);
	else
#endif /* ENABLE_NEON */
		error = SurveillanceHelper::convertSourceMV2GrayCV(source, grayImage);

	if (error != MEDIA_VISION_ERROR_NONE || grayImage.empty()) {
		LOGE("Media source conversion failed.");
		return error;
	}

	mv_source_h graySource;
	error = mv_create_source(&graySource);
	if (MEDIA_VISION_ERROR_NONE != error) {
		LOGE("Errors were occurred during source creating %i", error);
		return error;
	}

	error = mv_source_fill_by_buffer(
				graySource,
				grayImage.data,
				grayImage.cols * grayImage.rows,
				grayImage.cols,
				grayImage.rows,
				MEDIA_VISION_COLORSPACE_Y800);

	if (MEDIA_VISION_ERROR_NONE != error) {
		mv_destroy_source(graySource);
		LOGE("Errors were occurred during source filling %i", error);
		return error;
	}

	EventTriggersConstIter iter = __eventTriggers[videoStreamId].begin();

	for (; iter != __eventTriggers[videoStreamId].end(); ++iter) {
		error = (*iter)->pushSource(source, graySource, grayImage);

		if (error != MEDIA_VISION_ERROR_NONE)
			LOGE("Push source failed for event ", (*iter)->getEventType().c_str());
	}

	error = mv_destroy_source(graySource);

	if (MEDIA_VISION_ERROR_NONE != error)
		LOGE("Errors were occurred during gray source destroying %i", error);

	return MEDIA_VISION_ERROR_NONE;
}

int EventManager::getSupportedEventTypes(StringVector& eventTypes)
{
	eventTypes.clear();

	if (!__pInstance)
		setSupportedEventTypes();

	EventTypesMapConstIter etIter = SupportedEventTypes.begin();
	while (etIter != SupportedEventTypes.end()) {
		eventTypes.push_back(etIter->first);
		++etIter;
	}

	return MEDIA_VISION_ERROR_NONE;
}

int EventManager::getSupportedEventResultValueNames(
	StringVector& eventResValNames)
{
	eventResValNames.clear();

	if (!__pInstance)
		setSupportedEventTypes();

	EventTypesMapConstIter etIter = SupportedEventTypes.begin();
	while (etIter != SupportedEventTypes.end()) {
		eventResValNames.insert(
					eventResValNames.end(),
					etIter->second.begin(),
					etIter->second.end());
		++etIter;
	}

	return MEDIA_VISION_ERROR_NONE;
}

int EventManager::getSupportedEventResultValueNames(
	const std::string& eventTypeName,
	StringVector& eventResValNames)
{
	eventResValNames.clear();

	if (!__pInstance)
		setSupportedEventTypes();

	EventTypesMapConstIter etIter = SupportedEventTypes.find(eventTypeName);
	if (etIter == SupportedEventTypes.end())
		return MEDIA_VISION_ERROR_KEY_NOT_AVAILABLE;

	eventResValNames = etIter->second;

	return MEDIA_VISION_ERROR_NONE;
}

EventTriggersIter EventManager::isTriggerExists(
		EventTrigger* trigger,
		int videoStreamId)
{
	EventTriggersIter iter = __eventTriggers[videoStreamId].begin();

	for (; iter != __eventTriggers[videoStreamId].end(); ++iter)
		if (*(*iter) == *trigger)
			return iter;

	return iter;
}

} /* surveillance */
} /* mediavision */

