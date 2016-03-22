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

#ifndef __TIZEN_MEDIAVISION_BARCODE_GENERATE_OPEN_H__
#define __TIZEN_MEDIAVISION_BARCODE_GENERATE_OPEN_H__

#include "mv_barcode_generate.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @file  mv_barcode_generate_open.h
 * @brief This file contains the Media Vision barcode generate module API for
 *        the open module.
 */

/**
 * @brief Generates @ref mv_source_h with barcode image.
 *
 * @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
 * @param [in]  engine_cfg      The handle to the configuration of the engine
 * @param [in]  message         The message to be encoded in the barcode
 * @param [in]  type            Type of the barcode to be generated
 * @param [in]  qr_enc_mode     Encoding mode for the message (only for QR codes;
 *                              for 1D barcodes set this parameter to
 *                              @ref MV_BARCODE_QR_MODE_UNAVAILABLE)
 * @param [in]  qr_ecc          Error correction level (only for QR codes; for
 *                              1D barcodes set this parameter to
 *                              @ref MV_BARCODE_QR_ECC_UNAVAILABLE)
 * @param [in]  qr_version      QR code version (for 1D barcodes set this
 *                              parameter to 0)
 * @param [in, out] image       The media source handle which will be used to
 *                              fill by the buffer with generated image
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_MSG_TOO_LONG Too long or short message
 * @retval #MEDIA_VISION_ERROR_PERMISSION_DENIED Permission denied
 *
 * @see mv_barcode_generate_image_open()
 */
int mv_barcode_generate_source_open(mv_engine_config_h engine_cfg,
		const char *message,
		mv_barcode_type_e type,
		mv_barcode_qr_mode_e qr_enc_mode,
		mv_barcode_qr_ecc_e qr_ecc,
		int qr_version,
		mv_source_h image);

/**
 * @brief Generates image file with barcode.
 *
 * @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
 * @param [in] engine_cfg      The handle to the configuration of the engine
 * @param [in] message         The message to be encoded in the barcode
 * @param [in] image_width     The width of the generated image
 * @param [in] image_height    The height of the generated image
 * @param [in] type            Type of the barcode to be generated
 * @param [in] qr_enc_mode     Encoding mode for the message (only for QR codes;
 *                             for 1D barcodes set this parameter to
 *                             @ref MV_BARCODE_QR_MODE_UNAVAILABLE)
 * @param [in] qr_ecc          Error correction level (only for QR codes; for
 *                             1D barcodes set this parameter to
 *                             @ref MV_BARCODE_QR_ECC_UNAVAILABLE)
 * @param [in] qr_version      QR code version (for 1D barcodes set this
 *                             parameter to 0)
 * @param [in] image_path      The path to the file that has to be generated
 * @param [in] image_format    The format of the output image
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_MSG_TOO_LONG Too long or short message
 * @retval #MEDIA_VISION_ERROR_PERMISSION_DENIED Permission denied
 * @retval #MEDIA_VISION_ERROR_INVALID_PATH Invalid path
 *
 * @see mv_barcode_generate_source_open()
 */
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
		mv_barcode_image_format_e image_format);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TIZEN_MEDIAVISION_BARCODE_GENERATE_OPEN_H__ */
