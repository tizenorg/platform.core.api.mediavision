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

#include "mv_barcode_detect_open.h"

#include "Barcode.h"
#include "BarcodeUtils.h"

#include <mv_private.h>

#include <zbar.h>

using namespace MediaVision::Barcode;

int mv_barcode_detect_open(
	mv_source_h source,
	mv_engine_config_h engine_cfg,
	mv_rectangle_s roi,
	mv_barcode_detected_cb detect_cb,
	void *user_data)
{
	if (!source || !detect_cb) {
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	zbar::Image image;
	int err = convertSourceMV2Zbar(source, image);
	if (err != MEDIA_VISION_ERROR_NONE) {
		LOGW("convertSourceMV2Zbar failed");
		return err;
	}

	zbar::Image greyImage = image.convert("Y800");
	if (!greyImage.get_data()) {
		LOGE("fail to image convert by zbar");
		return MEDIA_VISION_ERROR_INVALID_OPERATION;
	}

	greyImage.set_crop(roi.point.x, roi.point.y, roi.width, roi.height);
	zbar::ImageScanner scanner;

	int target_val;
	err = mv_engine_config_get_int_attribute(engine_cfg, "MV_BARCODE_DETECT_ATTR_TARGET", &target_val);
	if (err != MEDIA_VISION_ERROR_NONE) {
		LOGW("mv_engine_config_get_int_attribute failed");
		return err;
	}

	/**
	 * 0 - linear barcodes and QR codes
	 * 1 - only linear barcodes
	 * 2 - only QR codes
	 */
	switch (target_val) {
	case 0:
		scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);
		break;
	case 1:
		scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 0);
		scanner.set_config(zbar::ZBAR_UPCA, zbar::ZBAR_CFG_ENABLE, 1);
		scanner.set_config(zbar::ZBAR_UPCE, zbar::ZBAR_CFG_ENABLE, 1);
		scanner.set_config(zbar::ZBAR_EAN8, zbar::ZBAR_CFG_ENABLE, 1);
		scanner.set_config(zbar::ZBAR_EAN13, zbar::ZBAR_CFG_ENABLE, 1);
		scanner.set_config(zbar::ZBAR_CODE128, zbar::ZBAR_CFG_ENABLE, 1);
		scanner.set_config(zbar::ZBAR_CODE39, zbar::ZBAR_CFG_ENABLE, 1);
		scanner.set_config(zbar::ZBAR_I25, zbar::ZBAR_CFG_ENABLE, 1);
		break;
	case 2:
		scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 0);
		scanner.set_config(zbar::ZBAR_QRCODE, zbar::ZBAR_CFG_ENABLE, 1);
		break;
	default:
		LOGW("Unavailabe target value %d", target_val);
	}

	int numberOfBarcodes = scanner.scan(greyImage);
	LOGI("ZBar scanner has found %i barcodes on the mv_source_h", numberOfBarcodes);
	mv_quadrangle_s *barcodeLocations = NULL;
	mv_barcode_type_e *types = NULL;

	if (numberOfBarcodes == 0) {
		LOGI("Call the detect callback for 0 detected barcodes");
		detect_cb(source, engine_cfg, barcodeLocations, NULL, types, numberOfBarcodes, user_data);
		return MEDIA_VISION_ERROR_NONE;
	} else if (numberOfBarcodes < 0) {
		LOGW("Incorrect number of barcodes (%i), detection is terminated", numberOfBarcodes);
		return MEDIA_VISION_ERROR_INTERNAL;
	}

	const char **messagesArray = new const char*[numberOfBarcodes];
	barcodeLocations = new mv_quadrangle_s[numberOfBarcodes];
	types = new mv_barcode_type_e[numberOfBarcodes];

	int i = 0;
	/* extract results and prepare them for callback passing */
	for (zbar::SymbolIterator symbol = greyImage.symbol_begin();
		symbol != greyImage.symbol_end();
		++symbol, ++i) {
		Barcode curBarcode(*symbol);

		size_t messageLength = curBarcode.getMessage().size();
		char *curMessage = new char[messageLength + 1];
		curBarcode.getMessage().copy(curMessage, messageLength);
		curMessage[messageLength] = '\0';
		messagesArray[i] = curMessage;

		types[i] = curBarcode.getType();

		int err = curBarcode.calculateLocation(barcodeLocations[i]);
		if (err != MEDIA_VISION_ERROR_NONE) {
			LOGW("Can't determine location for barcode, detection is terminated");
			for (int j = 0; j <= i; ++j) {
				delete[] messagesArray[j];
			}
			delete[] messagesArray;
			delete[] barcodeLocations;
			delete[] types;
			return err;
		}
	}

	LOGI("Call the detect callback for %i detected barcodes", numberOfBarcodes);
	detect_cb(source, engine_cfg, barcodeLocations, messagesArray, types, numberOfBarcodes, user_data);

	LOGI("Clean the memory from barcodes messages, locations and types");
	for (int j = 0; j < numberOfBarcodes; ++j) {
		delete[] messagesArray[j];
	}
	delete[] messagesArray;
	delete[] barcodeLocations;
	delete[] types;

	return MEDIA_VISION_ERROR_NONE;
}
