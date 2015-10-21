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

#include "BarcodeUtils.h"
#include "mv_common_c.h"

#include <mv_private.h>

#include <zbar.h>

namespace MediaVision {
namespace Barcode {
int convertSourceMV2Zbar(mv_source_h mvSource, zbar::Image& zbarSource)
{
	int err = MEDIA_VISION_ERROR_NONE;
	unsigned char *buffer = NULL;
	unsigned int height = 0;
	unsigned int width = 0;
	unsigned int size = 0;
	mv_colorspace_e colorspace = MEDIA_VISION_COLORSPACE_INVALID;

	err = mv_source_get_colorspace_c(mvSource, &colorspace);
	if (err != MEDIA_VISION_ERROR_NONE) {
		LOGW("Can't determine mv_source_h colorspace to convert to ZBar colorspace. Conversion failed");
		return err;
	}

	switch(colorspace) {
	case MEDIA_VISION_COLORSPACE_Y800:
		zbarSource.set_format("Y800");
		break;
	case MEDIA_VISION_COLORSPACE_I420:
		zbarSource.set_format("I420");
		break;
	case MEDIA_VISION_COLORSPACE_NV12:
		zbarSource.set_format("NV12");
		break;
	case MEDIA_VISION_COLORSPACE_YV12:
		zbarSource.set_format("YV12");
		break;
	case MEDIA_VISION_COLORSPACE_NV21:
		zbarSource.set_format("NV21");
		break;
	case MEDIA_VISION_COLORSPACE_YUYV:
		zbarSource.set_format("YUYV");
		break;
	case MEDIA_VISION_COLORSPACE_UYVY:
		zbarSource.set_format("UYVY");
		break;
	case MEDIA_VISION_COLORSPACE_422P:
		zbarSource.set_format("422P");
		break;
	case MEDIA_VISION_COLORSPACE_RGB565:
		zbarSource.set_format("RGBP");
		break;
	case MEDIA_VISION_COLORSPACE_RGB888:
		zbarSource.set_format("RGB3");
		break;
	case MEDIA_VISION_COLORSPACE_RGBA:
		zbarSource.set_format("RGB4");
		break;
	default:
		LOGE("Media vision colorspace is not supported by ZBar symbol");
		return MEDIA_VISION_ERROR_NOT_SUPPORTED_FORMAT;
	}

	err = mv_source_get_buffer_c(mvSource, &buffer, &size);
	if (err != MEDIA_VISION_ERROR_NONE) {
		LOGW("Can't get mv_source_h buffer to convert to ZBar image. Conversion failed");
		return err;
	}

	err = mv_source_get_height_c(mvSource, &height);
	if (err != MEDIA_VISION_ERROR_NONE) {
		LOGW("Can't get mv_source_h height for conversion. Conversion failed");
		return err;
	}

	err = mv_source_get_width_c(mvSource, &width);
	if (err != MEDIA_VISION_ERROR_NONE) {
		LOGW("Can't get mv_source_h width for conversion. Conversion failed");
		return err;
	}

	zbarSource.set_size(width, height);
	zbarSource.set_data(buffer, size);

	return err;
}

} /* Barcode */
} /* MediaVision */
