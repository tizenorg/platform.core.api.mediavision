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

#include "mv_common_c.h"

#include "MediaSource.h"
#include "EngineConfig.h"

#include <mv_private.h>

#include <string.h>
#include <media_packet.h>

int mv_create_source_c(
	mv_source_h *source_ptr)
{
	if (source_ptr == NULL) {
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	LOGD("Creating media vision source");
	(*source_ptr) = ((mv_source_h)new MediaVision::Common::MediaSource());

	if (*source_ptr == NULL) {
		LOGE("Failed to create media vision source");
		return MEDIA_VISION_ERROR_OUT_OF_MEMORY;
	}
	LOGD("Media vision source [%p] has been created", *source_ptr);

	return MEDIA_VISION_ERROR_NONE;
}

int mv_destroy_source_c(
	mv_source_h source)
{
	if (!source) {
		LOGE("Media source can't be destroyed because handle is NULL");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	LOGD("Destroying media vision source [%p]", source);
	delete ((MediaVision::Common::MediaSource*)source);
	LOGD("Media vision source has been destroyed");

	return MEDIA_VISION_ERROR_NONE;
}

int mv_source_fill_by_media_packet_c(
	mv_source_h source,
	media_packet_h media_packet)
{
	if (!source || !media_packet) {
		LOGE("Media source can't be filled by media_packet handle because "
				"one of the source or media_packet handles is NULL. "
				"source = %p; media_packet = %p", source, media_packet);
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	bool is_video = false;
	int image_width = 0;
	int image_height = 0;
	media_format_h format = NULL;
	media_format_mimetype_e mimetype = MEDIA_FORMAT_I420;
	unsigned char *data_buffer = NULL;
	uint64_t buffer_size = 0;
	mv_colorspace_e image_colorspace = MEDIA_VISION_COLORSPACE_INVALID;

	int ret = media_packet_is_video(media_packet, &is_video);
	if (ret != MEDIA_PACKET_ERROR_NONE) {
		LOGE("media_packet_is_video() failed, mv_source_h fill skipped");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	if (!is_video) {
		LOGE("Media packet isn't video, mv_source_h fill skipped");
		return MEDIA_VISION_ERROR_NOT_SUPPORTED_FORMAT;
	}

	ret = media_packet_get_format(media_packet, &format);
	if (ret != MEDIA_PACKET_ERROR_NONE) {
		LOGE("media_packet_get_format() failed, mv_source_h fill skipped");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	ret = media_format_get_video_info(
			format, &mimetype, &image_width, &image_height, NULL, NULL);
	if (ret != MEDIA_PACKET_ERROR_NONE) {
		LOGE("media_format_get_video_info() failed, mv_source_h fill skipped");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	if (mimetype > MEDIA_FORMAT_H261 && mimetype <= MEDIA_FORMAT_MPEG4_ASP) {
		LOGE("Media format mimetype is not the raw video, mv_source_h fill skipped");
		return MEDIA_VISION_ERROR_NOT_SUPPORTED_FORMAT;
	}

	switch (mimetype) {
	case MEDIA_FORMAT_I420:
		image_colorspace = MEDIA_VISION_COLORSPACE_I420;
		break;
	case MEDIA_FORMAT_NV12:
		image_colorspace = MEDIA_VISION_COLORSPACE_NV12;
		break;
	case MEDIA_FORMAT_YV12:
		image_colorspace = MEDIA_VISION_COLORSPACE_YV12;
		break;
	case MEDIA_FORMAT_NV21:
		image_colorspace = MEDIA_VISION_COLORSPACE_NV21;
		break;
	case MEDIA_FORMAT_YUYV:
		image_colorspace = MEDIA_VISION_COLORSPACE_YUYV;
		break;
	case MEDIA_FORMAT_UYVY:
		image_colorspace = MEDIA_VISION_COLORSPACE_UYVY;
		break;
	case MEDIA_FORMAT_422P:
		image_colorspace = MEDIA_VISION_COLORSPACE_422P;
		break;
	case MEDIA_FORMAT_RGB565:
		image_colorspace = MEDIA_VISION_COLORSPACE_RGB565;
		break;
	case MEDIA_FORMAT_RGB888:
		image_colorspace = MEDIA_VISION_COLORSPACE_RGB888;
		break;
	case MEDIA_FORMAT_RGBA:
		image_colorspace = MEDIA_VISION_COLORSPACE_RGBA;
		break;
	default:
		LOGE("Format of the media packet buffer is not supported by media "
			"vision source (media_format_h mimetype=%i)", mimetype);
		return MEDIA_VISION_ERROR_NOT_SUPPORTED_FORMAT;
	}

	ret = media_packet_get_buffer_data_ptr(media_packet, (void**)&data_buffer);
	if (ret != MEDIA_PACKET_ERROR_NONE) {
		LOGE("media_packet_get_buffer_data_ptr() failed, mv_source_h fill skipped");
			return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	ret = media_packet_get_buffer_size(media_packet, &buffer_size);
	if (ret != MEDIA_PACKET_ERROR_NONE) {
		LOGE("media_packet_get_buffer_size() failed, mv_source_h fill skipped");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	if (!((MediaVision::Common::MediaSource*)source)->fill(data_buffer, buffer_size,
		(unsigned int)image_width, (unsigned int)image_height, image_colorspace)) {
		LOGE("mv_source_h filling from media_packet_h failed");
		return MEDIA_VISION_ERROR_OUT_OF_MEMORY;
	}

	LOGD("Media source has been filled from media packet");
	return MEDIA_VISION_ERROR_NONE;
}

int mv_source_fill_by_buffer_c(
		mv_source_h source,
		unsigned char *data_buffer,
		unsigned int buffer_size,
		unsigned int image_width,
		unsigned int image_height,
		mv_colorspace_e image_colorspace)
{
	if (!source || buffer_size == 0 || data_buffer == NULL) {
		LOGE("Media source can't be filled by buffer because "
				"one of the source or data_buffer is NULL or buffer_size = 0. "
				"source = %p; data_buffer = %p; buffer_size = %u",
				source, data_buffer, buffer_size);
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	if (!((MediaVision::Common::MediaSource*)source)->fill(data_buffer,
		buffer_size, image_width, image_height, image_colorspace)) {
		LOGE("mv_source_h filling from buffer failed");
		return MEDIA_VISION_ERROR_OUT_OF_MEMORY;
	}

	LOGD("Media source has been filled from buffer");
	return MEDIA_VISION_ERROR_NONE;
}

int mv_source_clear_c(
	mv_source_h source)
{
	if (!source) {
		LOGE("Media source can't be cleared because source handle is NULL");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	LOGD("Clear media vision source [%p]", source);
	((MediaVision::Common::MediaSource*)source)->clear();
	LOGD("Media vision source [%p] has been cleared", source);

	return MEDIA_VISION_ERROR_NONE;
}

int mv_source_get_buffer_c(
	mv_source_h source,
	unsigned char **buffer,
	unsigned int *size)
{
	if (!source) {
		LOGE("Impossible to get buffer for NULL mv_source_h handle");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	LOGD("Get media vision source [%p] buffer and buffer size to be returned", source);
	*buffer = ((MediaVision::Common::MediaSource*)source)->getBuffer();
	*size = ((MediaVision::Common::MediaSource*)source)->getBufferSize();
	LOGD("Media vision source [%p] buffer (%p) and buffer size (%ui) has been returned", source, buffer, *size);

	return MEDIA_VISION_ERROR_NONE;
}

int mv_source_get_height_c(
	mv_source_h source,
	unsigned int *height)
{
	if (!source) {
		LOGE("Impossible to get height for NULL mv_source_h handle");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	LOGD("Get media vision source [%p] height to be returned", source);
	*height = ((MediaVision::Common::MediaSource*)source)->getHeight();
	LOGD("Media vision source [%p] height (%ui) has been returned", source, *height);

	return MEDIA_VISION_ERROR_NONE;
}

int mv_source_get_width_c(
	mv_source_h source,
	unsigned int *width)
{
	if (!source) {
		LOGE("Impossible to get width for NULL mv_source_h handle");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	LOGD("Get media vision source [%p] width to be returned", source);
	*width = ((MediaVision::Common::MediaSource*)source)->getWidth();
	LOGD("Media vision source [%p] width (%ui) has been returned", source, *width);

	return MEDIA_VISION_ERROR_NONE;
}

int mv_source_get_colorspace_c(
	mv_source_h source,
	mv_colorspace_e *colorspace)
{
	if (!source) {
		LOGE("Impossible to get colorspace for NULL mv_source_h handle");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

    LOGD("Get media vision source [%p] colorspace to be returned", source);
    *colorspace = ((MediaVision::Common::MediaSource*)source)->getColorspace();
    LOGD("Media vision source [%p] colorspace (%i) has been returned", source, *colorspace);

    return MEDIA_VISION_ERROR_NONE;
}

int mv_create_engine_config_c(
        mv_engine_config_h *engine_cfg)
{
	if (engine_cfg == NULL) {
		LOGE("Impossible to create mv_engine_config_h handle");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	LOGD("Creating media vision engine config");
	(*engine_cfg) = ((mv_engine_config_h)new MediaVision::Common::EngineConfig());
	LOGD("Media vision engine config [%p] has been created", *engine_cfg);

	if (*engine_cfg == NULL) {
		LOGE("Failed to create mv_engine_config_h handle");
		return MEDIA_VISION_ERROR_OUT_OF_MEMORY;
	}

	return MEDIA_VISION_ERROR_NONE;
}

int mv_destroy_engine_config_c(
		mv_engine_config_h engine_cfg)
{
	if (!engine_cfg) {
		LOGE("Impossible to destroy NULL mv_engine_config_h handle");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	LOGD("Destroying media vision engine config [%p]", engine_cfg);
	delete ((MediaVision::Common::EngineConfig*)engine_cfg);
	LOGD("Media vision engine config has been destroyed");

	return MEDIA_VISION_ERROR_NONE;
}

int mv_engine_config_set_double_attribute_c(
		mv_engine_config_h engine_cfg,
		const char *name,
		double value)
{
	if (!engine_cfg || name == NULL) {
		LOGE("Impossible to set attribute. One of the required parameters is "
				"NULL. engine_cfg = %p; name = %p;",
				 engine_cfg, name);
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	int ret = ((MediaVision::Common::EngineConfig*)engine_cfg)->setAttribute(
			std::string(name), value);

	if (ret != MEDIA_VISION_ERROR_NONE) {
		LOGE("Failed to set attribute [%s] with value %f. Error code (0x%08x)",
				name, value, ret);
		return ret;
	}

	LOGD("Attribute [%s] (value %f) has been set", name, value);
	return ret;
}

int mv_engine_config_set_int_attribute_c(
		mv_engine_config_h engine_cfg,
		const char *name,
		int value)
{
	if (!engine_cfg || name == NULL) {
		LOGE("Impossible to set attribute. One of the required parameters is "
				"NULL. engine_cfg = %p; name = %p;",
				engine_cfg, name);
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	int ret = ((MediaVision::Common::EngineConfig*)engine_cfg)->setAttribute(
			std::string(name), value);

	if (ret != MEDIA_VISION_ERROR_NONE) {
		LOGE("Failed to set attribute [%s] with value %i. Error code (0x%08x)",
				name, value, ret);
		return ret;
	}

	LOGD("Attribute [%s] (value %i) has been set", name, value);

	return ret;
}

int mv_engine_config_set_bool_attribute_c(
		mv_engine_config_h engine_cfg,
		const char *name,
		bool value)
{
	if (!engine_cfg || name == NULL) {
		LOGE("Impossible to set attribute. One of the required parameters is "
				"NULL. engine_cfg = %p; name = %p;",
				engine_cfg, name);
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	int ret = ((MediaVision::Common::EngineConfig*)engine_cfg)->setAttribute(
			std::string(name), value);

	if (ret != MEDIA_VISION_ERROR_NONE) {
		LOGE("Failed to set attribute [%s] with value %s. Error code (0x%08x)",
				name, value ? "TRUE" : "FALSE", ret);
		return ret;
	}

	LOGD("Attribute [%s] (value %s) has been set",
			name, value ? "TRUE" : "FALSE");
	return ret;
}

int mv_engine_config_set_string_attribute_c(
		mv_engine_config_h engine_cfg,
		const char *name,
		const char *value)
{
	if (!engine_cfg || name == NULL || value == NULL) {
		LOGE("Impossible to set attribute. One of the required parameters is "
				"NULL. engine_cfg = %p; name = %p; value = %p;",
				engine_cfg, name, value);
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	int ret = ((MediaVision::Common::EngineConfig*)engine_cfg)->setAttribute(
			std::string(name), std::string(value));

	if (ret != MEDIA_VISION_ERROR_NONE) {
		LOGE("Failed to set attribute [%s] with value %s. Error code (0x%08x)",
				name, value, ret);
		return ret;
	}

	LOGD("Attribute [%s] (value %s) has been set", name, value);
	return ret;
}

int mv_engine_config_get_double_attribute_c(
		mv_engine_config_h engine_cfg,
		const char *name,
		double *value)
{
	if (!engine_cfg || name == NULL || value == NULL) {
		LOGE("Impossible to get attribute. One of the required parameters is "
				"NULL. engine_cfg = %p; name = %p; value = %p;",
				engine_cfg, name, value);
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	int ret = ((MediaVision::Common::EngineConfig*)engine_cfg)->getDoubleAttribute(
			std::string(name), value);

	if (ret != MEDIA_VISION_ERROR_NONE) {
		LOGE("Failed to get attribute [%s]. Error code (0x%08x)",
				name, ret);
		return ret;
	}

	LOGD("Attribute [%s] (value %f) has been gotten",
			name, *value);
	return ret;
}

int mv_engine_config_get_int_attribute_c(
		mv_engine_config_h engine_cfg,
		const char *name,
		int *value)
{
	if (!engine_cfg || name == NULL || value == NULL) {
		LOGE("Impossible to get attribute. One of the required parameters is "
				"NULL. engine_cfg = %p; name = %p; value = %p;",
				engine_cfg, name, value);
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	int ret = ((MediaVision::Common::EngineConfig*)engine_cfg)->getIntegerAttribute(
			std::string(name), value);

	if (ret != MEDIA_VISION_ERROR_NONE) {
		LOGE("Failed to get attribute [%s]. Error code (0x%08x)",
				name, ret);
		return ret;
	}

	LOGD("Attribute [%s] (value %i) has been gotten",
			name, *value);
	return ret;
}

int mv_engine_config_get_bool_attribute_c(
		mv_engine_config_h engine_cfg,
		const char *name,
		bool *value)
{
	if (!engine_cfg || name == NULL || value == NULL) {
		LOGE("Impossible to get attribute. One of the required parameters is "
				"NULL. engine_cfg = %p; name = %p; value = %p;",
				engine_cfg, name, value);
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	int ret = ((MediaVision::Common::EngineConfig*)engine_cfg)->getBooleanAttribute(
			std::string(name), value);

	if (ret != MEDIA_VISION_ERROR_NONE) {
		LOGE("Failed to get attribute [%s]. Error code (0x%08x)",
				name, ret);
		return ret;
	}

	LOGD("Attribute [%s] (value %s) has been gotten",
			name, *value ? "TRUE" : "FALSE");
	return ret;
}

int mv_engine_config_get_string_attribute_c(
		mv_engine_config_h engine_cfg,
		const char *name,
		char **value)
{
	if (!engine_cfg || name == NULL || value == NULL) {
		LOGE("Impossible to get attribute. One of the required parameters is "
				"NULL. engine_cfg = %p; name = %p; value = %p;",
				engine_cfg, name, value);
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	std::string attributeValue;
	int ret = ((MediaVision::Common::EngineConfig*)engine_cfg)->getStringAttribute(
			std::string(name), &attributeValue);

	if (ret != MEDIA_VISION_ERROR_NONE) {
		LOGE("Failed to get attribute [%s]. Error code (0x%08x)",
				name, ret);
		return ret;
	}

	LOGD("Convert string to char*");
	int stringSize = attributeValue.size();
	(*value) = new char[stringSize + 1];

	if (attributeValue.copy(*value, stringSize) != attributeValue.size()) {
		LOGE("Conversion from string to char* failed");
		delete[] (*value);
		(*value) = NULL;
		return MEDIA_VISION_ERROR_INVALID_OPERATION;
	}
	(*value)[stringSize] = '\0';

	LOGD("Attribute [%s] (value %s) has been gotten",
			name, *value);
	return ret;
}

int mv_engine_config_foreach_supported_attribute_c(
        mv_supported_attribute_cb callback,
        void *user_data)
{
	if (NULL == callback) {
		LOGE("Impossible to traverse supported by Media Vision engine "
				"configuration attributes. Callback is NULL");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	using namespace MediaVision::Common;

	int err = EngineConfig::cacheDictionaries();

	if (MEDIA_VISION_ERROR_NONE != err) {
		LOGE("Failed to get attribute names/types. "
				"Failed to cache attributes from file");
		return err;
	}

	DictDblConstIter dblDictIter = EngineConfig::getDefaultDblDict().begin();
	DictIntConstIter intDictIter = EngineConfig::getDefaultIntDict().begin();
	DictBoolConstIter boolDictIter = EngineConfig::getDefaultBoolDict().begin();
	DictStrConstIter strDictIter = EngineConfig::getDefaultStrDict().begin();

	while (dblDictIter != EngineConfig::getDefaultDblDict().end()) {
		if (!callback(MV_ENGINE_CONFIG_ATTR_TYPE_DOUBLE,
				dblDictIter->first.c_str(), user_data)) {
			LOGD("Attribute names/types traverse has been stopped by the user");
			return MEDIA_VISION_ERROR_NONE;
		}
		++dblDictIter;
	}

	while (intDictIter != EngineConfig::getDefaultIntDict().end()) {
		if (!callback(MV_ENGINE_CONFIG_ATTR_TYPE_INTEGER,
				intDictIter->first.c_str(), user_data)) {
			LOGD("Attribute names/types traverse has been stopped by the user");
			return MEDIA_VISION_ERROR_NONE;
		}
		++intDictIter;
	}

	while (boolDictIter != EngineConfig::getDefaultBoolDict().end()) {
		if (!callback(MV_ENGINE_CONFIG_ATTR_TYPE_BOOLEAN,
				boolDictIter->first.c_str(), user_data)) {
			LOGD("Attribute names/types traverse has been stopped by the user");
			return MEDIA_VISION_ERROR_NONE;
		}
		++boolDictIter;
	}

	while (strDictIter != EngineConfig::getDefaultStrDict().end()) {
		if (!callback(MV_ENGINE_CONFIG_ATTR_TYPE_STRING,
				strDictIter->first.c_str(), user_data)) {
			LOGD("Attribute names/types traverse has been stopped by the user");
			return MEDIA_VISION_ERROR_NONE;
		}
		++strDictIter;
	}

	LOGD("Attribute names/types has been gotten");
	return MEDIA_VISION_ERROR_NONE;
}
