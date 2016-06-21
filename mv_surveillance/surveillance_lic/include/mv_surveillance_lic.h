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

#ifndef __TIZEN_MEDIAVISION_SURVEILLANCE_LIC_H__
#define __TIZEN_MEDIAVISION_SURVEILLANCE_LIC_H__

#include <mv_surveillance.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @file  mv_surveillance_lic.h
 * @brief This file contains the Media Vision surveillance API
 */

/**
 * @brief Allows to subscribe to the event and start calling @a callback
 *        each time when the @a source is pushed using
 *        @ref mv_surveillance_push_source_lic() and event is detected.
 *
 * @since_tizen 3.0
 * @remarks To stop handling triggering use
 *          @ref mv_surveillance_unsubscribe_event_trigger_lic().
 * @param [in] event_trigger    The event trigger activating calls of the
 *                              @a callback function
 * @param [in] engine_cfg       The engine configuration of the event
 * @param [in] callback         Callback to be called each time when event
 *                              occurrence is detected
 * @param [in] user_data        The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @post @ref mv_surveillance_event_occurred_cb
 *
 * @see mv_surveillance_event_trigger_s
 * @see mv_surveillance_unsubscribe_event_trigger_lic()
 */
int mv_surveillance_subscribe_event_trigger_lic(
        mv_surveillance_event_trigger_h event_trigger,
        mv_engine_config_h engine_cfg,
        mv_surveillance_event_occurred_cb callback,
        void *user_data);

/**
 * @brief Allows to unsubscribe from the event and stop calling @a callback.
 *
 * @since_tizen 3.0
 * @remarks To start handling trigger activation use
            @ref mv_surveillance_subscribe_event_trigger_lic().
 * @param [in] event_trigger    The event trigger for which subscription will be
 *                              stopped
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @post @ref mv_surveillance_event_occurred_cb
 *
 * @see mv_surveillance_event_trigger_s
 * @see mv_surveillance_subscribe_event_trigger_lic()
 */
int mv_surveillance_unsubscribe_event_trigger_lic(
        mv_surveillance_event_trigger_h event_trigger);

/**
 * @brief Allows to push source to the event trigger and start calling @a callback.
 *
 * @since_tizen 3.0
 * @param [in] source             The handle to the media source
 * @param [in] video_stream_id    The video stream, wthich will be updated
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @see mv_surveillance_event_trigger_s
 * @see mv_surveillance_event_occurred_cb
 * @see mv_surveillance_subscribe_event_trigger_lic()
 * @see mv_surveillance_unsubscribe_event_trigger_lic()
 */
int mv_surveillance_push_source_lic(
        mv_source_h source,
        int video_stream_id);

/**
 * @brief Starts traversing through list of supported event types.
 *
 * @since_tizen 3.0
 * @remarks Supported event types and their descriptions can be found in
 *          @ref CAPI_MEDIA_VISION_SURVEILLANCE_EVENT_TYPES documentation
 *          section
 * @param [in] callback     The callback function to be called for each
 *                          supported event type
 * @param [in] user_data    The user data to be passed to the @a callback
 *                          function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @see mv_surveillance_event_type_cb
 * @see mv_surveillance_foreach_event_result_value_name_lic()
 */
int mv_surveillance_foreach_event_type_lic(
        mv_surveillance_event_type_cb callback,
        void *user_data);

/**
 * @brief Starts traversing through list of supported event result value names.
 *
 * @since_tizen 3.0
 * @remarks Supported event types, event result value names and their
 *          descriptions can be found in
 *          @ref CAPI_MEDIA_VISION_SURVEILLANCE_EVENT_TYPES documentation
 *          section
 * @param [in] event_type    The name of the event type for which result value
 *                           names will be passed to the @a callback. Can be
 *                           set @c NULL. If set @c NULL then all supported
 *                           event result value names will be traversed
 * @param [in] callback      The callback function to be called for each
 *                           supported event result value name
 * @param [in] user_data     The user data to be passed to the @a callback
 *                           function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @see mv_surveillance_event_result_value_name_cb
 * @see mv_surveillance_foreach_event_type_lic()
 * @see mv_surveillance_get_result_value_lic()
 */
int mv_surveillance_foreach_event_result_value_name_lic(
        const char *event_type,
        mv_surveillance_event_result_value_name_cb callback,
        void *user_data);

/**
 * @brief Gets result value.
 * @details See the output values names in the event types descriptions located
 *          in /usr/share/capi-media-vision/surveillance-event-types.txt.
 *
 * @since_tizen 3.0
 * @param [in] result           The handle to the event result
 * @param [in] value_name       The name of the value to be gotten
 * @param [in, out] value       The pointer to variable which will be filled
 *                              by result value
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Memory for value has to be allocated
 *
 * @see mv_surveillance_event_trigger_s
 * @see mv_surveillance_event_occurred_cb
 * @see mv_surveillance_subscribe_event_trigger_lic()
 * @see mv_surveillance_unsubscribe_event_trigger_lic()
 * @see mv_surveillance_query_events_lic()
 */
int mv_surveillance_get_result_value_lic(
        mv_surveillance_result_h result,
        const char *value_name,
        void *value);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TIZEN_MEDIAVISION_SURVEILLANCE_LIC_H__ */
