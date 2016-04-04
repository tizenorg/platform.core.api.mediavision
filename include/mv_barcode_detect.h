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

#ifndef __TIZEN_MEDIAVISION_BARCODE_DETECT_H__
#define __TIZEN_MEDIAVISION_BARCODE_DETECT_H__

#include <mv_common.h>
#include <mv_barcode_type.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @file  mv_barcode_detect.h
 * @brief This file contains the Media Vision barcode detect API.
 */

/**
 * @addtogroup CAPI_MEDIA_VISION_BARCODE_MODULE
 * @{
 */

/**
 * @brief Define MV_BARCODE_DETECT_ATTR_TARGET to set target attribute of the engine configuration
 *
 * @since_tizen @if MOBILE 2.4 @else 3.0 @endif
 *
 * @see mv_barcode_detect_attr_target_e
 */
#define MV_BARCODE_DETECT_ATTR_TARGET "MV_BARCODE_DETECT_ATTR_TARGET"  /**< Target: 0-all, 1-1D, 2-2D*/

/**
 * @brief Enumeration to target attribute
 *
 * @since_tizen @if MOBILE 2.4 @else 3.0 @endif
 */
typedef enum {
	MV_BARCODE_DETECT_ATTR_TARGET_ALL,          /**< 1D and 2D */
	MV_BARCODE_DETECT_ATTR_TARGET_1D_BARCODE,   /**< 1D barcode only */
	MV_BARCODE_DETECT_ATTR_TARGET_2D_BARCODE,   /**< 2D barcode only */
} mv_barcode_detect_attr_target_e;


/**
 * @brief Called when barcode detection is completed.
 * @details If no barcode is detected then the method will be called, barcodes
 *          and states will be equal to NULL, and @a number_of_barcodes - 0.
 *
 * @since_tizen @if MOBILE 2.4 @else 3.0 @endif
 * @param [in] source               The handle to the media source
 * @param [in] engine_cfg           The handle to the configuration of the engine
 * @param [in] barcode_locations    The quadrangle locations of detected barcodes
 * @param [in] messages             The decoded messages of barcodes
 * @param [in] types                The types of detected barcodes
 * @param [in] number_of_barcodes   The number of detected barcodes
 * @param [in] user_data            The user data passed from
 *                                  the mv_barcode_detect() function
 *
 * @pre mv_barcode_detect() invokes this callback
 *
 * @see mv_barcode_detect()
 */
typedef void (*mv_barcode_detected_cb)(
        mv_source_h source,
        mv_engine_config_h engine_cfg,
        const mv_quadrangle_s *barcode_locations,
        const char* messages[],
        const mv_barcode_type_e *types,
        int number_of_barcodes,
        void *user_data);

/**
 * @brief Detects barcode(s) on source and reads message from it.
 *
 * @since_tizen @if MOBILE 2.4 @else 3.0 @endif
 * @param [in] source            The media source handle
 * @param [in] engine_cfg        The handle to the configuration of the engine
 * @param [in] roi               Region of interest - rectangular area on the
 *                               @a source which will be used for barcode detection
 *                               Note that @a roi should be inside area on the
 *                               @a source.
 * @param [in] detect_cb         The callback for result handling
 * @param [in] user_data    The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED_FORMAT Not supported format
 * @retval #MEDIA_VISION_ERROR_INTERNAL Internal error
 *
 * @pre Create a source handle by calling @ref mv_create_source()
 * @pre Create an engine configuration handle by calling
 *      @ref mv_create_engine_config(), otherwise use NULL
 *
 * @see mv_barcode_detected_cb()
 */
EXPORT_API int mv_barcode_detect(
        mv_source_h source,
        mv_engine_config_h engine_cfg,
        mv_rectangle_s roi,
        mv_barcode_detected_cb detect_cb,
        void *user_data);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TIZEN_MEDIAVISION_BARCODE_DETECT_H__ */
