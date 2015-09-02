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

#include "mv_face_lic.h"

/******************/
/* Face detection */
/******************/

int mv_face_detect_lic(
        mv_source_h source,
        mv_engine_config_h engine_cfg,
        mv_face_detected_cb detected_cb,
        void *user_data)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}


/********************/
/* Face recognition */
/********************/

int mv_face_recognize_lic(
        mv_source_h source,
        mv_face_recognition_model_h recognition_model,
        mv_engine_config_h engine_cfg,
        mv_rectangle_s *face_location,
        mv_face_recognized_cb recognized_cb,
        void *user_data)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}


/*****************/
/* Face tracking */
/*****************/

int mv_face_track_lic(
        mv_source_h source,
        mv_face_tracking_model_h tracking_model,
        mv_engine_config_h engine_cfg,
        mv_face_tracked_cb tracked_cb,
        bool do_learn,
        void *user_data)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}


/********************************/
/* Recognition of eye condition */
/********************************/

int mv_face_eye_condition_recognize_lic(
        mv_source_h source,
        mv_engine_config_h engine_cfg,
        mv_rectangle_s face_location,
        mv_face_eye_condition_recognized_cb eye_condition_recognized_cb,
        void *user_data)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}


/************************************/
/* Recognition of facial expression */
/************************************/

int mv_face_facial_expression_recognize_lic(
        mv_source_h source,
        mv_engine_config_h engine_cfg,
        mv_rectangle_s face_location,
        mv_face_facial_expression_recognized_cb expression_recognized_cb,
        void *user_data)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}


/******************************/
/* Recognition model behavior */
/******************************/

int mv_face_recognition_model_create_lic(
        mv_face_recognition_model_h *recognition_model)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

int mv_face_recognition_model_destroy_lic(
        mv_face_recognition_model_h recognition_model)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

int mv_face_recognition_model_clone_lic(
        mv_face_recognition_model_h src,
        mv_face_recognition_model_h *dst)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

int mv_face_recognition_model_save_lic(
        const char *file_name,
        mv_face_recognition_model_h recognition_model)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

int mv_face_recognition_model_load_lic(
        const char *file_name,
        mv_face_recognition_model_h recognition_model_h)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

int mv_face_recognition_model_add_lic(
        const mv_source_h source,
        mv_face_recognition_model_h recognition_model,
        const mv_rectangle_s *example_location,
        int face_label)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

int mv_face_recognition_model_reset_lic(
        mv_face_recognition_model_h recognition_model,
        const int *face_label)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

int mv_face_recognition_model_learn_lic(
        mv_engine_config_h engine_cfg,
        mv_face_recognition_model_h recognition_model)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

int mv_face_recognition_model_query_labels_lic(
        mv_face_recognition_model_h recognition_model,
        int **labels,
        unsigned int *number_of_labels)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}


/***************************/
/* Tracking model behavior */
/***************************/

int mv_face_tracking_model_create_lic(
        mv_face_tracking_model_h *tracking_model)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

int mv_face_tracking_model_destroy_lic(
        mv_face_tracking_model_h tracking_model)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

int mv_face_tracking_model_prepare_lic(
        mv_face_tracking_model_h tracking_model,
        mv_engine_config_h engine_cfg,
        mv_source_h source,
        mv_quadrangle_s *location)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

int mv_face_tracking_model_clone_lic(
        mv_face_tracking_model_h src,
        mv_face_tracking_model_h *dst)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

int mv_face_tracking_model_save_lic(
        const char *file_name,
        mv_face_tracking_model_h tracking_model)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

int mv_face_tracking_model_load_lic(
        const char *file_name,
        mv_face_tracking_model_h tracking_model)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}
