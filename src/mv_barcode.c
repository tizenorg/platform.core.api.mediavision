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

#include <system_info.h>

#include "mv_private.h"
#include "mv_barcode_detect.h"
#include "mv_barcode_generate.h"

#ifdef MEDIA_VISION_BARCODE_DETECTOR_LICENSE_PORT

/* Include headers of licensed barcode detect module here. */
#include "mv_barcode_detect_lic.h"

#else

/* Include headers of open barcode detect module here. */
#include "mv_barcode_detect_open.h"

#endif /* MEDIA_VISION_BARCODE_DETECTOR_LICENSE_PORT */

#ifdef MEDIA_VISION_BARCODE_GENERATOR_LICENSE_PORT

/* Include headers of licensed barcode generate module here. */
#include "mv_barcode_generate_lic.h"

#else

/* Include headers of open barcode generate module here. */
#include "mv_barcode_generate_open.h"

#endif /* MEDIA_VISION_BARCODE_GENERATOR_LICENSE_PORT */

/**
 * @file  mv_barcode.c
 * @brief This file contains the porting layer for Media Vision barcode module.
 */

int mv_barcode_detect(
		mv_source_h source,
		mv_engine_config_h engine_cfg,
		mv_rectangle_s roi,
		mv_barcode_detected_cb detect_cb,
		void *user_data)
{
	MEDIA_VISION_SUPPORT_CHECK(__mv_barcode_detect_check_system_info_feature_supported());
	MEDIA_VISION_INSTANCE_CHECK(source);
	MEDIA_VISION_NULL_ARG_CHECK(detect_cb);

	MEDIA_VISION_FUNCTION_ENTER();

	int ret = MEDIA_VISION_ERROR_NONE;
	unsigned int src_w = 0;
	unsigned int src_h = 0;

	ret = mv_source_get_width(source, &src_w);
	if (ret != MEDIA_VISION_ERROR_NONE) {
		LOGE("mv_source_get_width fail");
		return ret;
	}

	ret = mv_source_get_height(source, &src_h);
	if (ret != MEDIA_VISION_ERROR_NONE) {
		LOGE("mv_source_get_height fail");
		return ret;
	}

	if (roi.point.x < 0 || roi.point.y < 0 ||
		(roi.point.x + roi.width) > src_w ||
		(roi.point.y + roi.height) > src_h) {
		LOGE("roi is out of area on source");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

#ifdef MEDIA_VISION_BARCODE_DETECTOR_LICENSE_PORT

	/* Use licensed barcode detect functionality here. */
	ret = mv_barcode_detect_lic(
					source, engine_cfg, roi, detect_cb, user_data);

#else

	/* Use open barcode detect functionality here. */
	ret = mv_barcode_detect_open(
					source, engine_cfg, roi, detect_cb, user_data);

#endif /* MEDIA_VISION_BARCODE_DETECTOR_LICENSE_PORT */

	MEDIA_VISION_FUNCTION_LEAVE();
	return ret;
}

int mv_barcode_generate_source(
		mv_engine_config_h engine_cfg,
		const char *message,
		mv_barcode_type_e type,
		mv_barcode_qr_mode_e qr_enc_mode,
		mv_barcode_qr_ecc_e qr_ecc,
		int qr_version,
		mv_source_h image)
{
	MEDIA_VISION_SUPPORT_CHECK(__mv_barcode_generate_check_system_info_feature_supported());
	MEDIA_VISION_NULL_ARG_CHECK(message);
	MEDIA_VISION_INSTANCE_CHECK(image);

	MEDIA_VISION_FUNCTION_ENTER();

	if (type < MV_BARCODE_QR ||
		type >= MV_BARCODE_UNDEFINED) {
		LOGE("Not supported barcode type [%d]", type);
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	if (type == MV_BARCODE_QR) {
		if (qr_enc_mode < MV_BARCODE_QR_MODE_NUMERIC ||
			qr_enc_mode >= MV_BARCODE_QR_MODE_UNAVAILABLE) {
			LOGE("Not supported QR encoding mode[%d]", qr_enc_mode);
			return MEDIA_VISION_ERROR_INVALID_PARAMETER;
		}

		if (qr_ecc < MV_BARCODE_QR_ECC_LOW ||
			qr_ecc >= MV_BARCODE_QR_ECC_UNAVAILABLE) {
			LOGE("Not supported QR ECC level [%d]", qr_ecc);
			return MEDIA_VISION_ERROR_INVALID_PARAMETER;
		}

		if (qr_version < 1 || qr_version > 40) {
			LOGE("Not supported QR version [%d]", qr_version);
			return MEDIA_VISION_ERROR_INVALID_PARAMETER;
		}
	}

#ifdef MEDIA_VISION_BARCODE_GENERATOR_LICENSE_PORT

	/* Use licensed barcode generate functionality here. */
	int ret = mv_barcode_generate_source_lic(
					engine_cfg, message, type, qr_enc_mode, qr_ecc, qr_version,
					image);

#else

	/* Use open barcode generate functionality here. */
	int ret = mv_barcode_generate_source_open(
					engine_cfg, message, type, qr_enc_mode, qr_ecc, qr_version,
					image);

#endif /* MEDIA_VISION_BARCODE_GENERATOR_LICENSE_PORT */

	MEDIA_VISION_FUNCTION_LEAVE();
	return ret;
}

int mv_barcode_generate_image(
		mv_engine_config_h engine_cfg,
		const char *message,
		int image_width,
		int image_height,
		mv_barcode_type_e type,
		mv_barcode_qr_mode_e qr_enc_mode,
		mv_barcode_qr_ecc_e qr_ecc,
		int qr_version,
		const char *image_path,
		mv_barcode_image_format_e image_format)
{
	MEDIA_VISION_SUPPORT_CHECK(__mv_barcode_generate_check_system_info_feature_supported());
	MEDIA_VISION_NULL_ARG_CHECK(message);

	MEDIA_VISION_FUNCTION_ENTER();

	if (image_path == NULL) {
			LOGE("image_path is NULL\n");
			return MEDIA_VISION_ERROR_INVALID_PATH;
	}

	if (type < MV_BARCODE_QR ||
		type >= MV_BARCODE_UNDEFINED) {
		LOGE("Not supported barcode type [%d]", type);
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	if (type == MV_BARCODE_QR) {
		if (qr_enc_mode < MV_BARCODE_QR_MODE_NUMERIC ||
			qr_enc_mode >= MV_BARCODE_QR_MODE_UNAVAILABLE) {
			LOGE("Not supported QR encoding mode[%d]", qr_enc_mode);
			return MEDIA_VISION_ERROR_INVALID_PARAMETER;
		}

		if (qr_ecc < MV_BARCODE_QR_ECC_LOW ||
			qr_ecc >= MV_BARCODE_QR_ECC_UNAVAILABLE) {
			LOGE("Not supported QR ECC level [%d]", qr_ecc);
			return MEDIA_VISION_ERROR_INVALID_PARAMETER;
		}

		if (qr_version < 1 || qr_version > 40) {
			LOGE("Not supported QR version [%d]", qr_version);
			return MEDIA_VISION_ERROR_INVALID_PARAMETER;
		}
	}

	if (image_format < MV_BARCODE_IMAGE_FORMAT_BMP ||
		image_format >= MV_BARCODE_IMAGE_FORMAT_NUM) {
		LOGE("Not supported image format [%d]", image_format);
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

#ifdef MEDIA_VISION_BARCODE_GENERATOR_LICENSE_PORT

	/* Use licensed barcode generate functionality here. */
	int ret = mv_barcode_generate_image_lic(
					engine_cfg, message, image_width, image_height, type,
					qr_enc_mode, qr_ecc, qr_version, image_path, image_format);

#else

	/* Use open barcode generate functionality here. */
	int ret = mv_barcode_generate_image_open(
					engine_cfg, message, image_width, image_height, type,
					qr_enc_mode, qr_ecc, qr_version, image_path, image_format);

#endif /* MEDIA_VISION_BARCODE_GENERATOR_LICENSE_PORT */

	MEDIA_VISION_FUNCTION_LEAVE();
	return ret;
}
