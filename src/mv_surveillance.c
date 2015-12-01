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

#include "mv_surveillance.h"

#include "mv_surveillance_private.h"
#include "mv_private.h"

#ifdef MEDIA_VISION_SURVEILLANCE_LICENSE_PORT

/* Include headers of licensed surveillance module here. */
#include "mv_surveillance_lic.h"

#else

/* Include headers of open surveillance module here. */
#include "mv_surveillance_open.h"

#endif /* MEDIA_VISION_SURVEILLANCE_LICENSE_PORT */

/**
 * @file  mv_surveillance.c
 * @brief This file contains the porting layer for Media Vision surveillance module.
 */

static size_t __mv_surveillance_id_counter = 0;

int mv_surveillance_event_trigger_create(
		const char *event_type,
		mv_surveillance_event_trigger_h * trigger)
{
	MEDIA_VISION_SUPPORT_CHECK(__mv_face_check_system_info_feature_supported());
	MEDIA_VISION_SUPPORT_CHECK(__mv_image_check_system_info_feature_supported());
	MEDIA_VISION_NULL_ARG_CHECK(event_type);
	MEDIA_VISION_NULL_ARG_CHECK(trigger);
	MEDIA_VISION_FUNCTION_ENTER();

	mv_surveillance_event_trigger_s *handle =
			(mv_surveillance_event_trigger_s *) malloc(
					sizeof(mv_surveillance_event_trigger_s));
	if (NULL == handle) {
		LOGE("[%s] malloc fail", __func__);
		return MEDIA_VISION_ERROR_OUT_OF_MEMORY;
	}

	memset(handle, 0, sizeof(mv_surveillance_event_trigger_s));

	// default values:
	handle->trigger_id = ++__mv_surveillance_id_counter;
	handle->event_type = strndup(event_type, 255);
	handle->number_of_roi_points = 0;
	handle->roi = NULL;

	*trigger = (mv_surveillance_event_trigger_h) handle;

	MEDIA_VISION_FUNCTION_LEAVE();
	return MEDIA_VISION_ERROR_NONE;
}

int mv_surveillance_event_trigger_destroy(
		mv_surveillance_event_trigger_h trigger)
{
	MEDIA_VISION_SUPPORT_CHECK(__mv_face_check_system_info_feature_supported());
	MEDIA_VISION_SUPPORT_CHECK(__mv_image_check_system_info_feature_supported());
	MEDIA_VISION_NULL_ARG_CHECK(trigger);
	MEDIA_VISION_FUNCTION_ENTER();

	mv_surveillance_event_trigger_s *handle =
			(mv_surveillance_event_trigger_s *) trigger;
	free(handle->event_type);
	free(handle->roi);
	free((mv_surveillance_event_trigger_s *) trigger);

	MEDIA_VISION_FUNCTION_LEAVE();
	return MEDIA_VISION_ERROR_NONE;
}

int mv_surveillance_get_event_trigger_type(
		mv_surveillance_event_trigger_h trigger,
		char **event_type)
{
	MEDIA_VISION_SUPPORT_CHECK(__mv_face_check_system_info_feature_supported());
	MEDIA_VISION_SUPPORT_CHECK(__mv_image_check_system_info_feature_supported());
	MEDIA_VISION_INSTANCE_CHECK(trigger);
	MEDIA_VISION_NULL_ARG_CHECK(event_type);
	MEDIA_VISION_FUNCTION_ENTER();

	mv_surveillance_event_trigger_s *handle =
			(mv_surveillance_event_trigger_s *)trigger;
	*event_type = strndup(handle->event_type, 255);

    MEDIA_VISION_FUNCTION_LEAVE();
    return MEDIA_VISION_ERROR_NONE;
}

int mv_surveillance_set_event_trigger_roi(
		mv_surveillance_event_trigger_h trigger,
		int number_of_points,
		mv_point_s *roi)
{
	MEDIA_VISION_SUPPORT_CHECK(__mv_face_check_system_info_feature_supported());
	MEDIA_VISION_SUPPORT_CHECK(__mv_image_check_system_info_feature_supported());
	MEDIA_VISION_INSTANCE_CHECK(trigger);
	MEDIA_VISION_NULL_ARG_CHECK(roi);
	MEDIA_VISION_FUNCTION_ENTER();

	mv_surveillance_event_trigger_s *handle =
			(mv_surveillance_event_trigger_s *)trigger;

	handle->number_of_roi_points = number_of_points;
	handle->roi = (mv_point_s*) malloc(sizeof(mv_point_s) * number_of_points);

	if (NULL == handle->roi) {
		LOGE("[%s] malloc fail", __func__);
		return MEDIA_VISION_ERROR_OUT_OF_MEMORY;
	}

	int i = 0;
	for (; i < number_of_points; ++i) {
		handle->roi[i].x = roi[i].x;
		handle->roi[i].y = roi[i].y;
	}

	MEDIA_VISION_FUNCTION_LEAVE();
	return MEDIA_VISION_ERROR_NONE;
}

int mv_surveillance_get_event_trigger_roi(
		mv_surveillance_event_trigger_h trigger,
		int *number_of_points,
		mv_point_s ** roi)
{
	MEDIA_VISION_SUPPORT_CHECK(__mv_face_check_system_info_feature_supported());
	MEDIA_VISION_SUPPORT_CHECK(__mv_image_check_system_info_feature_supported());
	MEDIA_VISION_INSTANCE_CHECK(trigger);
	MEDIA_VISION_NULL_ARG_CHECK(number_of_points);
	MEDIA_VISION_NULL_ARG_CHECK(roi);
	MEDIA_VISION_FUNCTION_ENTER();

	mv_surveillance_event_trigger_s *handle =
			(mv_surveillance_event_trigger_s *) trigger;

	*number_of_points = handle->number_of_roi_points;
	if (0 == *number_of_points)
	{
		MEDIA_VISION_FUNCTION_LEAVE();
		return MEDIA_VISION_ERROR_NONE;
	}
	*roi = (mv_point_s *) malloc(
					sizeof(mv_point_s) * handle->number_of_roi_points);

	int i = 0;
	for (; i < handle->number_of_roi_points; ++i) {
		(*roi)[i].x = handle->roi[i].x;
		(*roi)[i].y = handle->roi[i].y;
	}

	MEDIA_VISION_FUNCTION_LEAVE();
	return MEDIA_VISION_ERROR_NONE;
}

int mv_surveillance_subscribe_event_trigger(
		mv_surveillance_event_trigger_h event_trigger,
		int video_stream_id,
		mv_engine_config_h engine_cfg,
		mv_surveillance_event_occurred_cb callback,
		void *user_data)
{
	MEDIA_VISION_SUPPORT_CHECK(__mv_face_check_system_info_feature_supported());
	MEDIA_VISION_SUPPORT_CHECK(__mv_image_check_system_info_feature_supported());
	MEDIA_VISION_INSTANCE_CHECK(event_trigger);
	MEDIA_VISION_NULL_ARG_CHECK(callback);
	MEDIA_VISION_FUNCTION_ENTER();

#ifdef MEDIA_VISION_SURVEILLANCE_LICENSE_PORT

	/* Use licensed surveillance functionality here. */
	const int ret = mv_surveillance_subscribe_event_trigger_lic(
					event_trigger,
					video_stream_id,
					engine_cfg,
					callback,
					user_data);

#else

	/* Use open surveillance functionality here. */
	const int ret = mv_surveillance_subscribe_event_trigger_open(
					event_trigger,
					video_stream_id,
					engine_cfg,
					callback,
					user_data);

#endif /* MEDIA_VISION_SURVEILLANCE_LICENSE_PORT */

	MEDIA_VISION_FUNCTION_LEAVE();
	return ret;
}

int mv_surveillance_unsubscribe_event_trigger(
		mv_surveillance_event_trigger_h event_trigger,
		int video_stream_id)
{
	MEDIA_VISION_SUPPORT_CHECK(__mv_face_check_system_info_feature_supported());
	MEDIA_VISION_SUPPORT_CHECK(__mv_image_check_system_info_feature_supported());
	MEDIA_VISION_INSTANCE_CHECK(event_trigger);
	MEDIA_VISION_FUNCTION_ENTER();

#ifdef MEDIA_VISION_SURVEILLANCE_LICENSE_PORT

	/* Use licensed surveillance functionality here. */
	const int ret = mv_surveillance_unsubscribe_event_trigger_lic(
					event_trigger,
					video_stream_id);

#else

	/* Use open surveillance functionality here. */
	const int ret = mv_surveillance_unsubscribe_event_trigger_open(
					event_trigger,
					video_stream_id);

#endif /* MEDIA_VISION_SURVEILLANCE_LICENSE_PORT */

	MEDIA_VISION_FUNCTION_LEAVE();
	return ret;
}

int mv_surveillance_push_source(
		mv_source_h source,
		int video_stream_id)
{
	MEDIA_VISION_SUPPORT_CHECK(__mv_face_check_system_info_feature_supported());
	MEDIA_VISION_SUPPORT_CHECK(__mv_image_check_system_info_feature_supported());
	MEDIA_VISION_INSTANCE_CHECK(source);
	MEDIA_VISION_FUNCTION_ENTER();

#ifdef MEDIA_VISION_SURVEILLANCE_LICENSE_PORT

	/* Use licensed surveillance functionality here. */
	const int ret = mv_surveillance_push_source_lic(source, video_stream_id);

#else

	/* Use open surveillance functionality here. */
	const int ret = mv_surveillance_push_source_open(source, video_stream_id);

#endif /* MEDIA_VISION_SURVEILLANCE_LICENSE_PORT */

	MEDIA_VISION_FUNCTION_LEAVE();
	return ret;
}

int mv_surveillance_foreach_supported_event_type(
		mv_surveillance_event_type_cb callback,
		void *user_data)
{
	MEDIA_VISION_SUPPORT_CHECK(__mv_face_check_system_info_feature_supported());
	MEDIA_VISION_SUPPORT_CHECK(__mv_image_check_system_info_feature_supported());
	MEDIA_VISION_NULL_ARG_CHECK(callback);
	MEDIA_VISION_FUNCTION_ENTER();

#ifdef MEDIA_VISION_SURVEILLANCE_LICENSE_PORT

	/* Use licensed surveillance functionality here. */
	const int ret = mv_surveillance_foreach_event_type_lic(
					callback,
					user_data);

#else

	/* Use open surveillance functionality here. */
	const int ret = mv_surveillance_foreach_event_type_open(
					callback,
					user_data);

#endif /* MEDIA_VISION_SURVEILLANCE_LICENSE_PORT */

	MEDIA_VISION_FUNCTION_LEAVE();
	return ret;
}

int mv_surveillance_foreach_event_result_name(
		const char *event_type,
		mv_surveillance_event_result_name_cb callback,
		void *user_data)
{
	MEDIA_VISION_SUPPORT_CHECK(__mv_face_check_system_info_feature_supported());
	MEDIA_VISION_SUPPORT_CHECK(__mv_image_check_system_info_feature_supported());
	MEDIA_VISION_NULL_ARG_CHECK(event_type);
	MEDIA_VISION_NULL_ARG_CHECK(callback);
	MEDIA_VISION_FUNCTION_ENTER();

#ifdef MEDIA_VISION_SURVEILLANCE_LICENSE_PORT

	/* Use licensed surveillance functionality here. */
	const int ret = mv_surveillance_foreach_event_result_value_name_lic(
					event_type,
					callback,
					user_data);

#else

	/* Use open surveillance functionality here. */
	const int ret = mv_surveillance_foreach_event_result_value_name_open(
					event_type,
					callback,
					user_data);

#endif /* MEDIA_VISION_SURVEILLANCE_LICENSE_PORT */

	MEDIA_VISION_FUNCTION_LEAVE();

	return ret;
}

int mv_surveillance_get_result_value(
		mv_surveillance_result_h result,
		const char *value_name,
		void *value)
{
	MEDIA_VISION_SUPPORT_CHECK(__mv_face_check_system_info_feature_supported());
	MEDIA_VISION_SUPPORT_CHECK(__mv_image_check_system_info_feature_supported());
	MEDIA_VISION_INSTANCE_CHECK(result);
	MEDIA_VISION_NULL_ARG_CHECK(value_name);
	MEDIA_VISION_NULL_ARG_CHECK(value);
	MEDIA_VISION_FUNCTION_ENTER();

#ifdef MEDIA_VISION_SURVEILLANCE_LICENSE_PORT

	/* Use licensed surveillance functionality here. */
	const int ret = mv_surveillance_get_result_value_lic(
					result,
					value_name,
					value);

#else

	/* Use open surveillance functionality here. */
	const int ret = mv_surveillance_get_result_value_open(
					result,
					value_name,
					value);

#endif /* MEDIA_VISION_SURVEILLANCE_LICENSE_PORT */

	MEDIA_VISION_FUNCTION_LEAVE();
	return ret;
}
