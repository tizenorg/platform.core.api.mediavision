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

#include "mv_surveillance_open.h"

#include "EventManager.h"
#include "EventResult.h"

#include <mv_private.h>

using namespace mediavision::surveillance;

int mv_surveillance_subscribe_event_trigger_open(
	mv_surveillance_event_trigger_h event_trigger,
	int video_stream_id,
	mv_engine_config_h engine_cfg,
	mv_surveillance_event_occurred_cb callback,
	void *user_data)
{
	mv_surveillance_event_trigger_s *handle =
		(mv_surveillance_event_trigger_s *)event_trigger;

	return EventManager::getInstance().registerEvent(
			event_trigger,
			static_cast<long int>(handle->trigger_id),
			handle->event_type,
			video_stream_id,
			engine_cfg,
			callback,
			user_data,
			handle->number_of_roi_points,
			handle->roi);
}

int mv_surveillance_unsubscribe_event_trigger_open(
	mv_surveillance_event_trigger_h event_trigger,
	int video_stream_id)
{
	mv_surveillance_event_trigger_s *handle =
		(mv_surveillance_event_trigger_s *)event_trigger;

	return EventManager::getInstance().unregisterEvent(
			static_cast<long int>(handle->trigger_id),
			video_stream_id);
}

int mv_surveillance_push_source_open(
		mv_source_h source,
		int video_stream_id)
{
	MEDIA_VISION_INSTANCE_CHECK(source);

	return EventManager::getInstance().pushSource(source, video_stream_id);
}

int mv_surveillance_foreach_event_type_open(
		mv_surveillance_event_type_cb callback,
		void *user_data)
{
	StringVector eventTypes;
	const int error = EventManager::getInstance().getSupportedEventTypes(eventTypes);

	if (error != MEDIA_VISION_ERROR_NONE) {
		LOGE("Query events failed due to internal issues. Error code: %i",
				error);
		return error;
	}

	StringConstIter eventIter = eventTypes.begin();
	while (eventIter != eventTypes.end()) {
		if (!callback((*eventIter).c_str(), user_data)) {
			break;
		}
		++eventIter;
	}

	return MEDIA_VISION_ERROR_NONE;
}

int mv_surveillance_foreach_event_result_value_name_open(
	const char *event_type,
	mv_surveillance_event_result_name_cb callback,
	void *user_data)
{
	StringVector eventResultValueNames;

	int error = MEDIA_VISION_ERROR_NONE;

	if (NULL == event_type) {
		error = EventManager::getInstance().getSupportedEventResultValueNames(
					eventResultValueNames);
	} else {
		error = EventManager::getInstance().getSupportedEventResultValueNames(
					event_type,
					eventResultValueNames);
	}

	if (error != MEDIA_VISION_ERROR_NONE) {
		LOGE("Query result value names failed due to internal issues. "
				"Error code: %i", error);
		return error;
	}

	StringConstIter ervnIter = eventResultValueNames.begin();
	while (ervnIter != eventResultValueNames.end()) {
		if (!callback((*ervnIter).c_str(), user_data)) {
			break;
		}
		++ervnIter;
	}

	return MEDIA_VISION_ERROR_NONE;
}

int mv_surveillance_get_result_value_open(
	mv_surveillance_result_h result,
	const char *value_name,
	void *value)
{
	MEDIA_VISION_INSTANCE_CHECK(result);
	MEDIA_VISION_NULL_ARG_CHECK(value_name);
	MEDIA_VISION_NULL_ARG_CHECK(value);

	EventResult *eventResult = (EventResult*) result;

	return eventResult->getResultValue(value_name, value);
}
