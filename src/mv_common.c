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

#include "mv_private.h"
#include "mv_common.h"
#include "mv_common_c.h"

int mv_create_source(
		mv_source_h *source)
{
	MEDIA_VISION_SUPPORT_CHECK(__mv_check_system_info_feature_supported());
	MEDIA_VISION_NULL_ARG_CHECK(source);

	MEDIA_VISION_FUNCTION_ENTER();
	int ret = mv_create_source_c(source);
	MEDIA_VISION_FUNCTION_LEAVE();

	return ret;
}

int mv_destroy_source(
		mv_source_h source)
{
	MEDIA_VISION_SUPPORT_CHECK(__mv_check_system_info_feature_supported());
	MEDIA_VISION_INSTANCE_CHECK(source);

	MEDIA_VISION_FUNCTION_ENTER();
	int ret = mv_destroy_source_c(source);
	MEDIA_VISION_FUNCTION_LEAVE();

	return ret;
}

int mv_source_fill_by_media_packet(
		mv_source_h source,
		media_packet_h media_packet)
{
	MEDIA_VISION_SUPPORT_CHECK(__mv_check_system_info_feature_supported());
	MEDIA_VISION_INSTANCE_CHECK(source);
	MEDIA_VISION_INSTANCE_CHECK(media_packet);

	MEDIA_VISION_FUNCTION_ENTER();
	int ret = mv_source_fill_by_media_packet_c(source, media_packet);
	MEDIA_VISION_FUNCTION_LEAVE();

	return ret;
}

int mv_source_fill_by_buffer(
		mv_source_h source,
		unsigned char *data_buffer,
		unsigned int buffer_size,
		unsigned int image_width,
		unsigned int image_height,
		mv_colorspace_e image_colorspace)
{
	MEDIA_VISION_SUPPORT_CHECK(__mv_check_system_info_feature_supported());
	MEDIA_VISION_INSTANCE_CHECK(source);
	MEDIA_VISION_NULL_ARG_CHECK(data_buffer);

	MEDIA_VISION_FUNCTION_ENTER();
	int ret = mv_source_fill_by_buffer_c(
		source, data_buffer, buffer_size, image_width, image_height,
		image_colorspace);
	MEDIA_VISION_FUNCTION_LEAVE();

	return ret;
}

int mv_source_clear(
		mv_source_h source)
{
	MEDIA_VISION_SUPPORT_CHECK(__mv_check_system_info_feature_supported());
	MEDIA_VISION_INSTANCE_CHECK(source);

	MEDIA_VISION_FUNCTION_ENTER();
	int ret = mv_source_clear_c(source);
	MEDIA_VISION_FUNCTION_LEAVE();

	return ret;
}

int mv_source_get_buffer(
		mv_source_h source,
		unsigned char **data_buffer,
		unsigned int *buffer_size)
{
	MEDIA_VISION_SUPPORT_CHECK(__mv_check_system_info_feature_supported());
	MEDIA_VISION_INSTANCE_CHECK(source);
	MEDIA_VISION_NULL_ARG_CHECK(data_buffer);
	MEDIA_VISION_NULL_ARG_CHECK(buffer_size);

	MEDIA_VISION_FUNCTION_ENTER();
	int ret = mv_source_get_buffer_c(source, data_buffer, buffer_size);
	MEDIA_VISION_FUNCTION_LEAVE();

	return ret;
}

int mv_source_get_height(
		mv_source_h source,
		unsigned int *image_height)
{
	MEDIA_VISION_SUPPORT_CHECK(__mv_check_system_info_feature_supported());
	MEDIA_VISION_INSTANCE_CHECK(source);
	MEDIA_VISION_NULL_ARG_CHECK(image_height);

	MEDIA_VISION_FUNCTION_ENTER();
	int ret = mv_source_get_height_c(source, image_height);
	MEDIA_VISION_FUNCTION_LEAVE();

	return ret;
}

int mv_source_get_width(
		mv_source_h source,
		unsigned int *image_width)
{
	MEDIA_VISION_SUPPORT_CHECK(__mv_check_system_info_feature_supported());
	MEDIA_VISION_INSTANCE_CHECK(source);
	MEDIA_VISION_NULL_ARG_CHECK(image_width);

	MEDIA_VISION_FUNCTION_ENTER();
	int ret = mv_source_get_width_c(source, image_width);
	MEDIA_VISION_FUNCTION_LEAVE();

	return ret;
}

int mv_source_get_colorspace(
		mv_source_h source,
		mv_colorspace_e *image_colorspace)
{
	MEDIA_VISION_SUPPORT_CHECK(__mv_check_system_info_feature_supported());
	MEDIA_VISION_INSTANCE_CHECK(source);
	MEDIA_VISION_NULL_ARG_CHECK(image_colorspace);

	MEDIA_VISION_FUNCTION_ENTER();
	int ret = mv_source_get_colorspace_c(source, image_colorspace);
	MEDIA_VISION_FUNCTION_LEAVE();

	return ret;
}

int mv_create_engine_config(
		mv_engine_config_h *engine_cfg)
{
	MEDIA_VISION_SUPPORT_CHECK(__mv_check_system_info_feature_supported());
	MEDIA_VISION_NULL_ARG_CHECK(engine_cfg);

	MEDIA_VISION_FUNCTION_ENTER();
	int ret = mv_create_engine_config_c(engine_cfg);
	MEDIA_VISION_FUNCTION_LEAVE();

	return ret;
}

int mv_destroy_engine_config(
		mv_engine_config_h engine_cfg)
{
	MEDIA_VISION_SUPPORT_CHECK(__mv_check_system_info_feature_supported());
	MEDIA_VISION_INSTANCE_CHECK(engine_cfg);

	MEDIA_VISION_FUNCTION_ENTER();
	int ret = mv_destroy_engine_config_c(engine_cfg);
	MEDIA_VISION_FUNCTION_LEAVE();

	return ret;
}

int mv_engine_config_set_double_attribute(
		mv_engine_config_h engine_cfg,
		const char *name,
		double value)
{
	MEDIA_VISION_SUPPORT_CHECK(__mv_check_system_info_feature_supported());
	MEDIA_VISION_INSTANCE_CHECK(engine_cfg);
	MEDIA_VISION_NULL_ARG_CHECK(name);

	MEDIA_VISION_FUNCTION_ENTER();
	int ret = mv_engine_config_set_double_attribute_c(
					engine_cfg, name, value);
	MEDIA_VISION_FUNCTION_LEAVE();

	return ret;
}

int mv_engine_config_set_int_attribute(
		mv_engine_config_h engine_cfg,
		const char *name,
		int value)
{
	MEDIA_VISION_SUPPORT_CHECK(__mv_check_system_info_feature_supported());
	MEDIA_VISION_INSTANCE_CHECK(engine_cfg);
	MEDIA_VISION_NULL_ARG_CHECK(name);

	MEDIA_VISION_FUNCTION_ENTER();
	int ret = mv_engine_config_set_int_attribute_c(
					engine_cfg, name, value);
	MEDIA_VISION_FUNCTION_LEAVE();

	return ret;
}

int mv_engine_config_set_bool_attribute(
		mv_engine_config_h engine_cfg,
		const char *name,
		bool value)
{
	MEDIA_VISION_SUPPORT_CHECK(__mv_check_system_info_feature_supported());
	MEDIA_VISION_INSTANCE_CHECK(engine_cfg);
	MEDIA_VISION_NULL_ARG_CHECK(name);

	MEDIA_VISION_FUNCTION_ENTER();
	int ret = mv_engine_config_set_bool_attribute_c(
					engine_cfg, name, value);
	MEDIA_VISION_FUNCTION_LEAVE();

	return ret;
}

int mv_engine_config_set_string_attribute(
		mv_engine_config_h engine_cfg,
		const char *name,
		const char *value)
{
	MEDIA_VISION_SUPPORT_CHECK(__mv_check_system_info_feature_supported());
	MEDIA_VISION_INSTANCE_CHECK(engine_cfg);
	MEDIA_VISION_NULL_ARG_CHECK(name);
	MEDIA_VISION_NULL_ARG_CHECK(value);

	MEDIA_VISION_FUNCTION_ENTER();
	int ret = mv_engine_config_set_string_attribute_c(
					engine_cfg, name, value);
	MEDIA_VISION_FUNCTION_LEAVE();

	return ret;
}

int mv_engine_config_get_double_attribute(
		mv_engine_config_h engine_cfg,
		const char *name,
		double *value)
{
	MEDIA_VISION_SUPPORT_CHECK(__mv_check_system_info_feature_supported());
	MEDIA_VISION_INSTANCE_CHECK(engine_cfg);
	MEDIA_VISION_NULL_ARG_CHECK(name);
	MEDIA_VISION_NULL_ARG_CHECK(value);

	MEDIA_VISION_FUNCTION_ENTER();
	int ret = mv_engine_config_get_double_attribute_c(
					engine_cfg, name, value);
	MEDIA_VISION_FUNCTION_LEAVE();

	return ret;
}

int mv_engine_config_get_int_attribute(
		mv_engine_config_h engine_cfg,
		const char *name,
		int *value)
{
	MEDIA_VISION_SUPPORT_CHECK(__mv_check_system_info_feature_supported());
	MEDIA_VISION_INSTANCE_CHECK(engine_cfg);
	MEDIA_VISION_NULL_ARG_CHECK(name);
	MEDIA_VISION_NULL_ARG_CHECK(value);

	MEDIA_VISION_FUNCTION_ENTER();
	int ret = mv_engine_config_get_int_attribute_c(
		engine_cfg, name, value);
	MEDIA_VISION_FUNCTION_LEAVE();

	return ret;
}

int mv_engine_config_get_bool_attribute(
		mv_engine_config_h engine_cfg,
		const char *name,
		bool *value)
{
	MEDIA_VISION_SUPPORT_CHECK(__mv_check_system_info_feature_supported());
	MEDIA_VISION_INSTANCE_CHECK(engine_cfg);
	MEDIA_VISION_NULL_ARG_CHECK(name);
	MEDIA_VISION_NULL_ARG_CHECK(value);

	MEDIA_VISION_FUNCTION_ENTER();
	int ret = mv_engine_config_get_bool_attribute_c(
					engine_cfg, name, value);
	MEDIA_VISION_FUNCTION_LEAVE();

	return ret;
}

int mv_engine_config_get_string_attribute(
		mv_engine_config_h engine_cfg,
		const char *name,
		char **value)
{
	MEDIA_VISION_SUPPORT_CHECK(__mv_check_system_info_feature_supported());
	MEDIA_VISION_INSTANCE_CHECK(engine_cfg);
	MEDIA_VISION_NULL_ARG_CHECK(name);
	MEDIA_VISION_NULL_ARG_CHECK(value);

	MEDIA_VISION_FUNCTION_ENTER();
	int ret = mv_engine_config_get_string_attribute_c(
					engine_cfg, name, value);
	MEDIA_VISION_FUNCTION_LEAVE();

	return ret;
}

int mv_engine_config_foreach_supported_attribute(
		mv_supported_attribute_cb callback,
		void *user_data)
{
	MEDIA_VISION_SUPPORT_CHECK(__mv_check_system_info_feature_supported());
	MEDIA_VISION_NULL_ARG_CHECK(callback);

	MEDIA_VISION_FUNCTION_ENTER();
	int ret =
			mv_engine_config_foreach_supported_attribute_c(callback, user_data);
	MEDIA_VISION_FUNCTION_LEAVE();

	return ret;
}
