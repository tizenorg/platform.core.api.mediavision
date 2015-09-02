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

#ifndef __TIZEN_MEDIAVISION_BARCODE_TYPE_H__
#define __TIZEN_MEDIAVISION_BARCODE_TYPE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @file  mv_barcode_type.h
 * @brief This file contains enumerations required by barcode detect/generate API.
 */

/**
 * @addtogroup CAPI_MEDIA_VISION_BARCODE_MODULE
 * @{
 */

/**
 * @brief Enumeration for supported barcode types.
 * @details QR codes (versions 1 to 40) and set of 1D barcodes are supported
 *
 * @since_tizen 2.4
 */
typedef enum
{
    MV_BARCODE_QR,          /**< 2D barcode - Quick Response code */
    MV_BARCODE_UPC_A,       /**< 1D barcode - Universal Product Code with 12-digit */
    MV_BARCODE_UPC_E,       /**< 1D barcode - Universal Product Code with 6-digit */
    MV_BARCODE_EAN_8,       /**< 1D barcode - International Article Number with 8-digit */
    MV_BARCODE_EAN_13,      /**< 1D barcode - International Article Number with 13-digit */
    MV_BARCODE_CODE128,     /**< 1D barcode - Code 128 */
    MV_BARCODE_CODE39,      /**< 1D barcode - Code 39 */
    MV_BARCODE_I2_5,        /**< 1D barcode - Interleaved Two of Five */
    MV_BARCODE_UNDEFINED    /**< Undefined */
} mv_barcode_type_e;

/**
 * @brief Enumeration for supported QR code error correction level.
 *
 * @since_tizen 2.4
 * @remarks This is unavailable for 1D barcodes
 */
typedef enum
{
    MV_BARCODE_QR_ECC_LOW,           /**< Recovery up to  7% losses */
    MV_BARCODE_QR_ECC_MEDIUM,        /**< Recovery up to 15% losses */
    MV_BARCODE_QR_ECC_QUARTILE,      /**< Recovery up to 25% losses */
    MV_BARCODE_QR_ECC_HIGH,          /**< Recovery up to 30% losses */
    MV_BARCODE_QR_ECC_UNAVAILABLE    /**< Unavailable  */
} mv_barcode_qr_ecc_e;

/**
 * @brief Enumeration for supported QR code encoding mode.
 *
 * @since_tizen 2.4
 * @remarks This is unavailable for 1D barcodes
 */
typedef enum
{
    MV_BARCODE_QR_MODE_NUMERIC,         /**< Numeric digits */
    MV_BARCODE_QR_MODE_ALPHANUMERIC,    /**< Alphanumeric characters */
    MV_BARCODE_QR_MODE_BYTE,            /**< Raw 8-bit bytes */
    MV_BARCODE_QR_MODE_UTF8,            /**< UTF-8 character encoding */
    MV_BARCODE_QR_MODE_UNAVAILABLE      /**< Unavailable */
} mv_barcode_qr_mode_e;

/**
 * @brief Enumeration for supported image formats for the barcode generating.
 *
 * @since_tizen 2.4
 */
typedef enum
{
    MV_BARCODE_IMAGE_FORMAT_UNAVAILABLE = -1, /** Unavailable image format */
    MV_BARCODE_IMAGE_FORMAT_BMP,    /**< BMP image format */
    MV_BARCODE_IMAGE_FORMAT_JPG,    /**< JPEG image format */
    MV_BARCODE_IMAGE_FORMAT_PNG,    /**< PNG image format */
    MV_BARCODE_IMAGE_FORMAT_NUM,    /**< The number of supported image format */
} mv_barcode_image_format_e;

/**
 * @}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TIZEN_MEDIAVISION_BARCODE_TYPE_H__ */
