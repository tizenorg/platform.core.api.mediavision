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

#include "mv_image_lic.h"

int mv_image_recognize_lic(
        mv_source_h source,
        const mv_image_object_h *image_objects,
        int number_of_objects,
        mv_engine_config_h engine_cfg,
        mv_image_recognized_cb recognized_cb,
        void *user_data)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

int mv_image_track_lic(
        mv_source_h source,
        mv_image_tracking_model_h image_tracking_model,
        mv_engine_config_h engine_cfg,
        mv_image_tracked_cb tracked_cb,
        void *user_data)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

int mv_image_object_create_lic(
        mv_image_object_h *image_object)

{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}


int mv_image_object_destroy_lic(
        mv_image_object_h image_object)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

int mv_image_object_fill_lic(
        mv_image_object_h image_object,
        mv_engine_config_h engine_cfg,
        mv_source_h source,
        mv_rectangle_s *location)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

int mv_image_object_get_recognition_rate_lic(
        mv_image_object_h image_object,
        double *recognition_rate)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

int mv_image_object_set_label_lic(
        mv_image_object_h image_object,
        int label)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

int mv_image_object_get_label_lic(
        mv_image_object_h image_object,
        int *label)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

int mv_image_object_clone_lic(
        mv_image_object_h src,
        mv_image_object_h *dst)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

int mv_image_object_save_lic(
        mv_image_object_h image_object,
        const char *file_name)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

int mv_image_object_load_lic(
        mv_image_object_h image_object,
        const char *file_name)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

int mv_image_tracking_model_create_lic(
        mv_image_tracking_model_h *image_tracking_model)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

int mv_image_tracking_model_set_target_lic(
        mv_image_object_h image_object,
        mv_image_tracking_model_h image_tracking_model)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

int mv_image_tracking_model_destroy_lic(
        mv_image_tracking_model_h image_tracking_model)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

int mv_image_tracking_model_refresh_lic(
        mv_image_tracking_model_h image_tracking_model,
        mv_engine_config_h engine_cfg)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

int mv_image_tracking_model_clone_lic(
        mv_image_tracking_model_h src,
        mv_image_tracking_model_h *dst)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

int mv_image_tracking_model_save_lic(
        mv_image_tracking_model_h image_tracking_model,
        const char *file_name)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

int mv_image_tracking_model_load_lic(
        mv_image_tracking_model_h image_tracking_model,
        const char *file_name)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}
