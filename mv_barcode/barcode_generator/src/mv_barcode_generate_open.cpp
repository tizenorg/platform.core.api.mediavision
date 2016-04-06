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

#include "mv_barcode_generate_open.h"

#include "mv_common_c.h"
#include "BarcodeGenerator.h"

#include <mv_private.h>

#include <algorithm>
#include <cstring>

using namespace MediaVision::Barcode;

namespace {
int alphanumToUpper(std::string& strToTransform)
{
	std::string tempString = strToTransform;
	std::transform(tempString.begin(), tempString.end(),
					tempString.begin(), ::toupper);

	if (std::string::npos != tempString.find_first_not_of("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:")) {
		LOGE("Barcode message can't be converted according to support "
				"alphanumeric (0..9, A..Z, space, $, %, *, +, -, ., /, :) "
				"mode: %s", strToTransform.c_str());
		return BARCODE_ERROR_INVALID_DATA;
	}

	LOGI("Barcode message was converted according to support alphanumeric "
			"mode: %s -> %s", strToTransform.c_str(), tempString.c_str());
	strToTransform = tempString;
	return BARCODE_ERROR_NONE;
}

BarcodeType convertBarcodeType(mv_barcode_type_e type)
{
	BarcodeType barcodeType = BARCODE_QR;
	switch (type) {
	case MV_BARCODE_UPC_A:
		barcodeType = BARCODE_UPCA;
		break;
	case MV_BARCODE_UPC_E:
		barcodeType = BARCODE_UPCE;
		break;
	case MV_BARCODE_EAN_8:
		barcodeType = BARCODE_EAN8;
		break;
	case MV_BARCODE_EAN_13:
		barcodeType = BARCODE_EAN13;
		break;
	case MV_BARCODE_CODE128:
		barcodeType = BARCODE_CODE128;
		break;
	case MV_BARCODE_CODE39:
		barcodeType = BARCODE_CODE39;
		break;
	case MV_BARCODE_I2_5:
		barcodeType = BARCODE_INTERLEAVE_2_5;
		break;
	default:
		break;
	}

	LOGI("Media vision barcode type has been converted to ZInt barcode type "
			"(%i -> %i)", type, barcodeType);
	return barcodeType;
}

BarcodeQREncodingMode convertEncodingMode(mv_barcode_qr_mode_e mode)
{
	BarcodeQREncodingMode encodingMode = BARCODE_QR_MODE_ALPHANUMERIC;

	switch (mode) {
	case MV_BARCODE_QR_MODE_NUMERIC:
		encodingMode = BARCODE_QR_MODE_NUMERIC;
		break;
	case MV_BARCODE_QR_MODE_BYTE:
		encodingMode = BARCODE_QR_MODE_BYTE;
		break;
	case MV_BARCODE_QR_MODE_UTF8:
		encodingMode = BARCODE_QR_MODE_UTF8;
		break;
	default:
		break;
	}

	LOGI("Media vision QRCode encoding mode has been converted to "
			"ZInt encoding mode (%i -> %i)", mode, encodingMode);
	return encodingMode;
}

BarcodeQRErrorCorrectionLevel convertECC(mv_barcode_qr_ecc_e ecc)
{
	BarcodeQRErrorCorrectionLevel ecclevel = BARCODE_QR_ECC_LOW;

	switch (ecc) {
	case MV_BARCODE_QR_ECC_MEDIUM:
		ecclevel = BARCODE_QR_ECC_MEDIUM;
		break;
	case MV_BARCODE_QR_ECC_QUARTILE:
		ecclevel = BARCODE_QR_ECC_QUARTILE;
		break;
	case MV_BARCODE_QR_ECC_HIGH:
		ecclevel = BARCODE_QR_ECC_HIGH;
		break;
	default:
		break;
	}

	LOGI("Media vision ECC level has been converted to "
		"ZInt ECC level (%i -> %i)", ecc, ecclevel);
	return ecclevel;
}

int convertBarcodeError(int barcodeError)
{
	int mvError = MEDIA_VISION_ERROR_NONE;

	switch (barcodeError) {
	case BARCODE_WARNING_INVALID_OPTION:
		mvError = MEDIA_VISION_ERROR_INVALID_PARAMETER;
		break;
	case BARCODE_ERROR_TOO_LONG:
		mvError = MEDIA_VISION_ERROR_MSG_TOO_LONG;
		break;
	case BARCODE_ERROR_INVALID_DATA:
		mvError = MEDIA_VISION_ERROR_INVALID_DATA;
		break;
	case BARCODE_ERROR_INVALID_CHECK:
		mvError = MEDIA_VISION_ERROR_INVALID_PARAMETER;
		break;
	case BARCODE_ERROR_INVALID_OPTION:
		mvError = MEDIA_VISION_ERROR_INVALID_PARAMETER;
		break;
	case BARCODE_ERROR_ENCODING_PROBLEM:
		mvError = MEDIA_VISION_ERROR_INTERNAL;
		break;
	case BARCODE_ERROR_FILE_ACCESS:
		mvError = MEDIA_VISION_ERROR_PERMISSION_DENIED;
		break;
	case BARCODE_ERROR_MEMORY:
		mvError = MEDIA_VISION_ERROR_OUT_OF_MEMORY;
		break;
	case BARCODE_ERROR_INVALID_PATH:
		mvError = MEDIA_VISION_ERROR_INVALID_PATH;
	default:
		break;
	}

	LOGI("ZInt error code has been converted to the media vision error code "
		"(%i -> (0x%08x))", barcodeError, mvError);
	return mvError;
}

BarcodeImageFormat convertImageFormat(mv_barcode_image_format_e format)
{
	BarcodeImageFormat imageFormat = BARCODE_IMAGE_PNG;

	switch (format) {
	case MV_BARCODE_IMAGE_FORMAT_JPG:
		imageFormat = BARCODE_IMAGE_JPG;
		break;
	case MV_BARCODE_IMAGE_FORMAT_BMP:
		imageFormat = BARCODE_IMAGE_BMP;
		break;
	default:
		break;
	}

	LOGI("Media vision image format has been converted to "
		"internal image format (%i -> %i)", format, imageFormat);
	return imageFormat;
}

} /* anonymous namespace */

int mv_barcode_generate_source_open(
		mv_engine_config_h engine_cfg,
		const char *message,
		mv_barcode_type_e type,
		mv_barcode_qr_mode_e qr_enc_mode,
		mv_barcode_qr_ecc_e qr_ecc,
		int qr_version,
		mv_source_h image)
{
	std::string messageStr = std::string(message);

	if (qr_enc_mode == MV_BARCODE_QR_MODE_NUMERIC &&
		messageStr.find_first_not_of("0123456789") != std::string::npos) {
		LOGE("Barcode message can't be used according to support "
			"numeric (0..9) mode: %s", messageStr.c_str());
		return MEDIA_VISION_ERROR_INVALID_DATA;
	}

	int error = BARCODE_ERROR_NONE;
	if (MV_BARCODE_QR == type &&
		MV_BARCODE_QR_MODE_ALPHANUMERIC == qr_enc_mode) {
		error = alphanumToUpper(messageStr);
		if (BARCODE_ERROR_NONE != error) {
			return convertBarcodeError(error);
		}
	}

	unsigned char *imageBuffer = NULL;
	unsigned int imageWidth = 0u;
	unsigned int imageHeight = 0u;
	unsigned int imageChannels = 0u;

	int showText = 0;
	char value;
	char *fgcolour = NULL;
	char *bgcolour = NULL;

	if (engine_cfg != NULL) {
		error = mv_engine_config_get_int_attribute(engine_cfg, "MV_BARCODE_GENERATE_ATTR_TEXT", &showText);
		if (error != MEDIA_VISION_ERROR_NONE) {
			LOGW("mv_engine_config_get_int_attribute failed");
			return error;
		}

		if (showText == BARCODE_GEN_TEXT_VISIBLE && type == MV_BARCODE_QR) {
			LOGW("QR code generation with visible text is not supported");
			return MEDIA_VISION_ERROR_INVALID_OPERATION;
		}

		/* set color value */
		error = mv_engine_config_get_string_attribute(engine_cfg, "MV_BARCODE_GENERATE_ATTR_COLOR_FRONT", &fgcolour);
		if (error != MEDIA_VISION_ERROR_NONE) {
			if (fgcolour) {
				delete [] fgcolour;
				fgcolour = NULL;
			}

			LOGW("mv_engine_config_get_string_attribute failed");
			return error;
		}

		error = mv_engine_config_get_string_attribute(engine_cfg, "MV_BARCODE_GENERATE_ATTR_COLOR_BACK", &bgcolour);
		if (error != MEDIA_VISION_ERROR_NONE) {
			if (bgcolour) {
				delete [] bgcolour;
				bgcolour = NULL;
			}

			if (fgcolour) {
				delete [] fgcolour;
				fgcolour = NULL;
			}

			LOGW("mv_engine_config_get_string_attribute failed");
			return error;
		}
	}

	/*
	The input colorspace is RGB but the generators' is BGR.
	Replace the value of R with that of B
	*/
	value = fgcolour[0];
	fgcolour[0] = fgcolour[4];
	fgcolour[4] = value;

	value = fgcolour[1];
	fgcolour[1] = fgcolour[5];
	fgcolour[5] = value;

	value = bgcolour[0];
	bgcolour[0] = bgcolour[4];
	bgcolour[4] = value;

	value = bgcolour[1];
	bgcolour[1] = bgcolour[5];
	bgcolour[5] = value;

	error = BarcodeGenerator::generateBarcodeToBuffer(
					&imageBuffer,
					&imageWidth,
					&imageHeight,
					&imageChannels,
					messageStr,
					convertBarcodeType(type),
					convertEncodingMode(qr_enc_mode),
					convertECC(qr_ecc),
					qr_version,
					showText,
					fgcolour,
					bgcolour);

	if (fgcolour != NULL) {
		delete [] fgcolour;
		fgcolour = NULL;
	}

	if (bgcolour != NULL) {
		delete [] bgcolour;
		bgcolour = NULL;
	}

	if (error != BARCODE_ERROR_NONE) {
		LOGE("Barcode generation to the buffer failed");
		if (NULL != imageBuffer) {
			LOGI("Delete temporal buffer");
			delete[] imageBuffer;
		}
		return convertBarcodeError(error);
	}

	const unsigned int imageBufferSize = imageWidth * imageHeight * imageChannels;

	LOGI("Barcode has been generated to the buffer: "
		"Buffer size = %ui x %ui; Channels = %ui; Message = %s",
		imageWidth, imageHeight, imageChannels, messageStr.c_str());

	error = mv_source_fill_by_buffer_c(
					image,
					imageBuffer,
					imageBufferSize,
					imageWidth,
					imageHeight,
					MEDIA_VISION_COLORSPACE_RGB888);

	if (error != MEDIA_VISION_ERROR_NONE) {
		LOGE("Meidiavision source fill by generated buffer failed");
	}

	if (NULL != imageBuffer) {
		LOGI("Delete temporal buffer");
		delete[] imageBuffer;
	}

	return error;
}


int mv_barcode_generate_image_open(
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
	std::string messageStr = std::string(message);

	if (qr_enc_mode == MV_BARCODE_QR_MODE_NUMERIC &&
		messageStr.find_first_not_of("0123456789") != std::string::npos) {
		LOGE("Barcode message can't be used according to support "
		"numeric (0..9) mode: %s", messageStr.c_str());
		return MEDIA_VISION_ERROR_INVALID_DATA;
	}

	if (NULL == image_path) {
		LOGE("Can't save barcode image to the path[%p]. The path has to be specified", image_path);
		return MEDIA_VISION_ERROR_INVALID_PATH;
	}

	int error = BARCODE_ERROR_NONE;
	if (MV_BARCODE_QR == type &&
		MV_BARCODE_QR_MODE_ALPHANUMERIC == qr_enc_mode) {
		error = alphanumToUpper(messageStr);
		if (BARCODE_ERROR_NONE != error) {
			return convertBarcodeError(error);
		}
	}

	int showText = 0;
	char value;
	char *fgcolour = NULL;
	char *bgcolour = NULL;

	if (engine_cfg != NULL) {
		/* set visible text attribute */
		error = mv_engine_config_get_int_attribute(engine_cfg, "MV_BARCODE_GENERATE_ATTR_TEXT", &showText);
		if (error != MEDIA_VISION_ERROR_NONE) {
			LOGW("mv_engine_config_get_int_attribute failed");
			return error;
		}

		if (showText == BARCODE_GEN_TEXT_VISIBLE && type == MV_BARCODE_QR) {
			LOGW("QR code generation with visible text is not supported");
			return MEDIA_VISION_ERROR_INVALID_OPERATION;
		}

		/* set color value */
		error = mv_engine_config_get_string_attribute(engine_cfg, "MV_BARCODE_GENERATE_ATTR_COLOR_FRONT", &fgcolour);
		if (error != MEDIA_VISION_ERROR_NONE) {
			if (fgcolour) {
				delete [] fgcolour;
				fgcolour = NULL;
			}

			LOGW("mv_engine_config_get_string_attribute failed");
			return error;
		}

		error = mv_engine_config_get_string_attribute(engine_cfg, "MV_BARCODE_GENERATE_ATTR_COLOR_BACK", &bgcolour);
		if (error != MEDIA_VISION_ERROR_NONE) {
			if (bgcolour) {
				delete [] bgcolour;
				bgcolour = NULL;
			}

			if (fgcolour) {
				delete [] fgcolour;
				fgcolour = NULL;
			}

			LOGW("mv_engine_config_get_string_attribute failed");
			return error;
		}
	}

	/*
	The input colorspace is RGB but the generators' is BGR.
	Replace the value of R with that of B
	*/
	value = fgcolour[0];
	fgcolour[0] = fgcolour[4];
	fgcolour[4] = value;

	value = fgcolour[1];
	fgcolour[1] = fgcolour[5];
	fgcolour[5] = value;

	value = bgcolour[0];
	bgcolour[0] = bgcolour[4];
	bgcolour[4] = value;

	value = bgcolour[1];
	bgcolour[1] = bgcolour[5];
	bgcolour[5] = value;

	error = BarcodeGenerator::generateBarcodeToImage(
							std::string(image_path),
							convertImageFormat(image_format),
							image_width,
							image_height,
							messageStr,
							convertBarcodeType(type),
							convertEncodingMode(qr_enc_mode),
							convertECC(qr_ecc),
							qr_version,
							showText,
							fgcolour,
							bgcolour);

	if (fgcolour != NULL) {
		delete [] fgcolour;
		fgcolour = NULL;
	}

	if (bgcolour != NULL) {
		delete [] bgcolour;
		bgcolour = NULL;
	}

	if (error != BARCODE_ERROR_NONE) {
		LOGE("Barcode generation to the image file failed");
	} else {
		LOGI("Barcode has been generated to the image: "
			"Image size = %ui x %ui; Message = %s",
			image_width, image_height, messageStr.c_str());
	}

	return convertBarcodeError(error);
}
