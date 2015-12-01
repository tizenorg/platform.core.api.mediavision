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

#include "mv_surveillance_lic.h"

int mv_surveillance_subscribe_event_trigger_lic(
        mv_surveillance_event_trigger_h event_trigger,
        mv_engine_config_h engine_cfg,
        mv_surveillance_event_occurred_cb callback,
        void *user_data)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}


int mv_surveillance_unsubscribe_event_trigger_lic(
        mv_surveillance_event_trigger_h event_trigger)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

int mv_surveillance_push_source_lic(
        mv_source_h source,
        int video_stream_id)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

int mv_surveillance_foreach_event_type_lic(
        mv_surveillance_event_type_cb callback,
        void *user_data)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

int mv_surveillance_foreach_event_result_value_name_lic(
        const char *event_type,
        mv_surveillance_event_result_value_name_cb callback,
        void *user_data)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

int mv_surveillance_get_result_value_lic(
        mv_surveillance_result_h result,
        const char *value_name,
        void *value)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}
