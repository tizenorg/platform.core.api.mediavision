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

#ifndef __BARCODEOPTIONS_H__
#define __BARCODEOPTIONS_H__

/**
 * @file BarcodeOptions.h
 * @brief This file contains the Barcode options.
 */

namespace MediaVision {
namespace Barcode {
/**
 * @brief The Barcode type enumeration.
 *
 * @since_tizen 2.4
 */
enum BarcodeType {
	BARCODE_QR = 58,
	BARCODE_UPCA = 34,
	BARCODE_UPCE = 37,
	BARCODE_EAN8 = 13,
	BARCODE_EAN13 = BARCODE_EAN8,
	BARCODE_CODE39 = 8,
	BARCODE_CODE128 = 20,
	BARCODE_INTERLEAVE_2_5 = 3
};

/**
 * @brief The Barcode error correction level enumeration.
 *
 * @since_tizen 2.4
 * @remarks This is unavailable for 1D barcodes.
 */
enum BarcodeQRErrorCorrectionLevel {
	BARCODE_QR_ECC_UNAVAILABLE = 0,
	BARCODE_QR_ECC_LOW = 1,
	BARCODE_QR_ECC_MEDIUM = 2,
	BARCODE_QR_ECC_QUARTILE = 3,
	BARCODE_QR_ECC_HIGH = 4
};

/**
 * @brief The Barcode encoding mode enumeration.
 *
 * @since_tizen 2.4
 * @remarks This is unavailable for 1D barcodes.
 */
enum BarcodeQREncodingMode {
	BARCODE_QR_MODE_NUMERIC = 1,
	BARCODE_QR_MODE_ALPHANUMERIC = 1,
	BARCODE_QR_MODE_BYTE = 0,
	BARCODE_QR_MODE_UTF8 = 1,
	BARCODE_QR_MODE_KANJI = 3,
	BARCODE_QR_MODE_UNAVAILABLE
};

/**
 * @brief The Barcode image format enumeration.
 *
 * @since_tizen 2.4
 */
enum BarcodeImageFormat {
	BARCODE_IMAGE_JPG,
	BARCODE_IMAGE_PNG,
	BARCODE_IMAGE_BMP
};

/**
 * @brief The Barcode generation text option enumeration.
 *
 * @since_tizen 2.4
 */
enum BarcodeGenTextOpt {
	BARCODE_GEN_TEXT_INVISIBLE,
	BARCODE_GEN_TEXT_VISIBLE
};

/**
 * @brief The Barcode error enumeration.
 *
 * @since_tizen 2.4
 */
enum BarcodeError {
	BARCODE_ERROR_NONE = 0,
	BARCODE_WARNING_INVALID_OPTION = 2,
	BARCODE_ERROR_TOO_LONG = 5,
	BARCODE_ERROR_INVALID_DATA = 6,
	BARCODE_ERROR_INVALID_CHECK = 7,
	BARCODE_ERROR_INVALID_OPTION = 8,
	BARCODE_ERROR_ENCODING_PROBLEM = 9,
	BARCODE_ERROR_FILE_ACCESS = 10,
	BARCODE_ERROR_MEMORY = 11,
	BARCODE_ERROR_INVALID_PATH = 12,
};

} /* Barcode */
} /* MediaVision */

#endif /* __BARCODEOPTIONS_H__ */

