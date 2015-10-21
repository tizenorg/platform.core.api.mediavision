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

#ifndef __BARCODEGENERATOR_H__
#define __BARCODEGENERATOR_H__

#include "BarcodeOptions.h"

#include <string>

/**
 * @file BarcodeGenerator.h
 * @brief This file contains the BarcodeGenerator class.
 */

namespace MediaVision {
namespace Barcode {
/**
 * @brief This class implements barcode generation.
 * @details 1D Barcodes and 2D QR codes are supported.
 *
 * @since_tizen 2.4
 */
class BarcodeGenerator {
public:
	/**
	 * @brief This method generates Barcodes image according to options.
	 *
	 * @since_tizen 2.4
	 * @param [in] imageFileName    Image file name which will be generated
	 * @param [in] imageFormat      Image file format which will be generated
	 * @param [in] imageWidth       Image file width which will be generated
	 * @param [in] imageHeight      Image file height which will be generated
	 * @param [in] message          Input message to be encoded
	 * @param [in] type             Barcode type (1D barcode or 2D QR code)
	 * @param [in] encodingMode     Encoding mode (for QR codes only)
	 * @param [in] correctionLevel  Error correction level (for QR codes only)
	 * @param [in] qrVersion        QR code version (1 ~ 40, 0 for 1D barcodes)
	 * @param [in] showText         Show text or not
	 * @return BARCODE_ERROR_NONE from BarcodeError which is 0 if success,
	 *         BarcodeError value otherwise
	 */
	static int generateBarcodeToImage(
		const std::string& imageFileName,
		BarcodeImageFormat imageFormat,
		const int imageWidth,
		const int imageHeight,
		const std::string& message,
		BarcodeType type,
		BarcodeQREncodingMode encodingMode = BARCODE_QR_MODE_UNAVAILABLE,
		BarcodeQRErrorCorrectionLevel correctionLevel = BARCODE_QR_ECC_UNAVAILABLE,
		int qrVersion = 0,
		int showText = 0);

	/**
	 * @brief This method generates Barcodes image buffer according to options.
	 *
	 * @since_tizen 2.4
	 * @param [out] imageBuffer        Image buffer with image to be generated
	 * @param [out] imageWidth         Image buffer width which will be generated
	 * @param [out] imageHeight        Image buffer height which will be generated
	 * @param [out] imageChannels      Image buffer channels number which will be generated
	 * @param [in] message             Input message to be encoded
	 * @param [in] type                Barcode type (1D barcode or 2D QR code)
	 * @param [in] encodingMode        Encoding mode (for QR codes only)
	 * @param [in] correctionLevel     Error correction level (for QR codes only)
	 * @param [in] qrVersion           QR code version (1 ~ 40, 0 for 1D barcodes)
	 * @param [in] showText            Show text or not
	 * @return BARCODE_ERROR_NONE from BarcodeError which is 0 if success,
	 *         BarcodeError value otherwise
	 */
	static int generateBarcodeToBuffer(
		unsigned char **imageBuffer,
		unsigned int *imageWidth,
		unsigned int *imageHeight,
		unsigned int *imageChannels,
		const std::string& message,
		BarcodeType type,
		BarcodeQREncodingMode encodingMode = BARCODE_QR_MODE_UNAVAILABLE,
		BarcodeQRErrorCorrectionLevel correctionLevel = BARCODE_QR_ECC_UNAVAILABLE,
		int qrVersion = 0,
		int showText = 0);
};

} /* Barcode */
} /* MediaVision */

#endif /* __BARCODEGENERATOR_H__ */

