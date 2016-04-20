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

#ifndef __TIZEN_MEDIAVISION_BARCODE_GENERATE_H__
#define __TIZEN_MEDIAVISION_BARCODE_GENERATE_H__

#include <mv_common.h>
#include <mv_barcode_type.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @file  mv_barcode_generate.h
 * @brief This file contains the Media Vision barcode generate API.
 */

/**
 * @addtogroup CAPI_MEDIA_VISION_BARCODE_MODULE
 * @{
 */

/**
 * @brief Define MV_BARCODE_GENERATE_ATTR_TEXT to set text attribute
          of the engine configuration
 *
 * @since_tizen @if MOBILE 2.4 @else 3.0 @endif
 * @remarks This attribute is only available for 1D barcode generation
 *
 * @see mv_barcode_generate_attr_text_e
 */
#define MV_BARCODE_GENERATE_ATTR_TEXT "MV_BARCODE_GENERATE_ATTR_TEXT" /**< Text:
                                                                    0-invisible,
                                                                    1-visible */

/**
 * @brief Defines MV_BARCODE_GENERATE_ATTR_COLOR_FRONT to set
		  Barcode's foreground color attribute of the engine configuration.
 *
 * @since_tizen 3.0
 * @remarks This attribute represents RGB color as a hex triplet with six digits.
 */
#define MV_BARCODE_GENERATE_ATTR_COLOR_FRONT "MV_BARCODE_GENERATE_ATTR_COLOR_FRONT"

/**
 * @brief Defines MV_BARCODE_GENERATE_ATTR_COLOR_BACK to set
		  Barcode's background color attribute of the engine configuration.
 *
 * @since_tizen 3.0
 * @remarks This attribute represents RGB color as a hex triplet with six digits.
 */
#define MV_BARCODE_GENERATE_ATTR_COLOR_BACK "MV_BARCODE_GENERATE_ATTR_COLOR_BACK"

/**
 * @brief Enumeration to text attribute
 *
 * @since_tizen @if MOBILE 2.4 @else 3.0 @endif
 */
typedef enum {
    MV_BARCODE_GENERATE_ATTR_TEXT_INVISIBLE,   /**< Invisible */
    MV_BARCODE_GENERATE_ATTR_TEXT_VISIBLE,     /**< Visible */
} mv_barcode_generate_attr_text_e;


/**
 * @brief Generates @ref mv_source_h with barcode image.
 * @details Pay attention that for EAN-8 and EAN-13 barcode types the barcode
 * type may be selected automatically and this selection depends on the input
 * message length. Also for QR codes the version may be selected as minimum
 * required to generate QR code with the input message length.
 *
 * @since_tizen @if MOBILE 2.4 @else 3.0 @endif
 * @remarks If the text attribute of engine configuration is set to
 *          MV_BARCODE_GENERATE_ATTR_TEXT_VISIBLE,
 *          MEDIA_VISION_ERROR_INVALID_OPERATION will be return
 *          when @a type is MV_BARCODE_QR
 *
 * @param [in]  engine_cfg      The handle to the configuration of the engine
 * @param [in]  message         The message to be encoded in the barcode
 * @param [in]  type            Type of the barcode to be generated
 * @param [in]  qr_enc_mode     Encoding mode for the message (only for QR codes;
 *                              for 1D barcodes set this parameter to
 *                              @a MV_BARCODE_QR_MODE_UNAVAILABLE)
 * @param [in]  qr_ecc          Error correction level (only for QR codes; for
 *                              1D barcodes set this parameter to
 *                              @a MV_BARCODE_QR_ECC_UNAVAILABLE)
 * @param [in]  qr_version      QR code version (for 1D barcodes set this
 *                              parameter to 0)
 * @param [in, out] image       The media source handle which will be used to
 *                              fill by the buffer with generated image
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_MSG_TOO_LONG Too long or short message
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 * @retval #MEDIA_VISION_ERROR_INVALID_DATA Invalid data
 * @retval #MEDIA_VISION_ERROR_INTERNAL Internal error
 * @retval #MEDIA_VISION_ERROR_INVALID_OPERATION Invalid operation
 *
 * @pre Create an engine configuration handle by calling
 *      @ref mv_create_engine_config(), otherwise use NULL
 *
 * @see mv_barcode_generate_image()
 */
int mv_barcode_generate_source(
        mv_engine_config_h engine_cfg,
        const char *message,
        mv_barcode_type_e type,
        mv_barcode_qr_mode_e qr_enc_mode,
        mv_barcode_qr_ecc_e qr_ecc,
        int qr_version,
        mv_source_h image);

/**
 * @brief Generates image file with barcode.
 * @details Pay attention that for EAN-8 and EAN-13 barcode types the barcode
 * type may be selected automatically and this selection depends on the input
 * message length. Also for QR codes the version may be selected as minimum
 * required to generate QR code with the input message length.
 *
 * @since_tizen @if MOBILE 2.4 @else 3.0 @endif
 * @remarks If the text attribute of engine configuration is set to
 *          MV_BARCODE_GENERATE_ATTR_TEXT_VISIBLE,
 *          MEDIA_VISION_ERROR_INVALID_OPERATION will be return
 *          when @a type is MV_BARCODE_QR
 *
 * @remarks The mediastorage privilege http://tizen.org/privilege/mediastorage is needed \n
            if @a image_path is relevant to media storage.\n
            The externalstorage privilege http://tizen.org/privilege/externalstorage is needed \n
            if @a image_path is relevant to external storage.
 * @param [in] engine_cfg      The handle to the configuration of the engine
 * @param [in] message         The message to be encoded in the barcode
 * @param [in] image_width     The width of the generated image
 * @param [in] image_height    The height of the generated image
 * @param [in] type            Type of the barcode to be generated
 * @param [in] qr_enc_mode     Encoding mode for the message (only for QR codes;
 *                             for 1D barcodes set this parameter to
 *                             @a MV_BARCODE_QR_MODE_UNAVAILABLE)
 * @param [in] qr_ecc          Error correction level (only for QR codes; for
 *                             1D barcodes set this parameter to
 *                             @a MV_BARCODE_QR_ECC_UNAVAILABLE)
 * @param [in] qr_version      QR code version (for 1D barcodes set this
 *                             parameter to 0)
 * @param [in] image_path      The path to the file that has to be generated
 * @param [in] image_format    The format of the output image
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_MSG_TOO_LONG Too long or short message
 * @retval #MEDIA_VISION_ERROR_PERMISSION_DENIED Permission denied
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 * @retval #MEDIA_VISION_ERROR_INVALID_DATA Invalid data
 * @retval #MEDIA_VISION_ERROR_INVALID_PATH Invalid path
 * @retval #MEDIA_VISION_ERROR_INTERNAL Internal error
 * @retval #MEDIA_VISION_ERROR_INVALID_OPERATION Invalid operation
 *
 * @pre Create an engine configuration handle by calling
 *      @ref mv_create_engine_config(), otherwise use NULL
 *
 * @see mv_barcode_generate_source()
 */
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
        mv_barcode_image_format_e image_format);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TIZEN_MEDIAVISION_BARCODE_GENERATE_H__ */
