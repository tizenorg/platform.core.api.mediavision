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

#ifndef __TIZEN_MEDIAVISION_BARCODE_DETECT_OPEN_H__
#define __TIZEN_MEDIAVISION_BARCODE_DETECT_OPEN_H__

#include "mv_barcode_detect.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @file  mv_barcode_detect_open.h
 * @brief This file contains the Media Vision barcode detect module API for the
 *        open module.
 */

/**
 * @brief Detects barcode(s) on source and reads message from it.
 *
 * @since_tizen 2.4
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
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED_FORMAT Not supported format
 * @retval #MEDIA_VISION_ERROR_INTERNAL Internal error
 * @pre Create a source handle by calling @ref mv_create_source()
 * @pre Create an engine configuration handle by calling
 *      @ref mv_create_engine_config() if necessary, otherwise use NULL
 *
 * @see mv_barcode_detected_cb()
 */
int mv_barcode_detect_open(
        mv_source_h source,
        mv_engine_config_h engine_cfg,
        mv_rectangle_s roi,
        mv_barcode_detected_cb detect_cb,
        void *user_data);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TIZEN_MEDIAVISION_BARCODE_DETECT_OPEN_H__ */
