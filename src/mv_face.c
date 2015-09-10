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

#include "mv_private.h"
#include "mv_face.h"

#ifdef MEDIA_VISION_FACE_LICENSE_PORT

/* Include headers of licensed face module here. */
#include "mv_face_lic.h"

#else

/* Include headers of open face module here. */
#include "mv_face_open.h"

#endif /* MEDIA_VISION_FACE_LICENSE_PORT */

/**
 * @file  mv_face.c
 * @brief This file contains the porting layer for Media Vision face module.
 */

static const int check_source_roi_quadrangle(mv_quadrangle_s *roi, mv_source_h source)
{
    int ret = MEDIA_VISION_ERROR_NONE;

    if (roi)
    {
        int src_w = 0;
        int src_h = 0;

        ret = mv_source_get_width(source, &src_w);
        if (ret != MEDIA_VISION_ERROR_NONE)
        {
            LOGE("mv_source_get_width fail");
            return ret;
        }

        ret = mv_source_get_height(source, &src_h);
        if (ret != MEDIA_VISION_ERROR_NONE)
        {
            LOGE("mv_source_get_height fail");
            return ret;
        }

        int idx = 0;
        while (idx < 4)
        {
            if (roi->points[idx].x < 0 || roi->points[idx].y < 0 ||
                roi->points[idx].x > src_w || roi->points[idx].y > src_h)
            {
               LOGE("roi is out of area on source");
               return MEDIA_VISION_ERROR_INVALID_PARAMETER;
            }
            ++idx;
        }
    }

    return ret;
}

static const int check_source_roi(mv_rectangle_s *roi, mv_source_h source)
{
    int ret = MEDIA_VISION_ERROR_NONE;

    if (roi)
    {
        int src_w = 0;
        int src_h = 0;

        ret = mv_source_get_width(source, &src_w);
        if (ret != MEDIA_VISION_ERROR_NONE)
        {
            LOGE("mv_source_get_width fail");
            return ret;
        }

        ret = mv_source_get_height(source, &src_h);
        if (ret != MEDIA_VISION_ERROR_NONE)
        {
            LOGE("mv_source_get_height fail");
            return ret;
        }

        if (roi->width <= 0 || roi->height <= 0)
        {
            LOGE("roi has negative width or height");
            return MEDIA_VISION_ERROR_INVALID_PARAMETER;
        }

        if (roi->point.x < 0 || roi->point.y < 0 ||
           (roi->point.x + roi->width) > src_w ||
           (roi->point.y + roi->height) > src_h)
        {
           LOGE("roi is out of area on source");
           return MEDIA_VISION_ERROR_INVALID_PARAMETER;
        }
    }

    return ret;
}

int mv_face_detect(
        mv_source_h source,
        mv_engine_config_h engine_cfg,
        mv_face_detected_cb detected_cb,
        void *user_data)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_face_check_system_info_feature_supported());
    MEDIA_VISION_INSTANCE_CHECK(source);
    MEDIA_VISION_NULL_ARG_CHECK(detected_cb);

    MEDIA_VISION_FUNCTION_ENTER();

    int ret = MEDIA_VISION_ERROR_NONE;

    #ifdef MEDIA_VISION_FACE_LICENSE_PORT

    ret = mv_face_detect_lic(source, engine_cfg, detected_cb, user_data);

    #else

    ret = mv_face_detect_open(source, engine_cfg, detected_cb, user_data);

    #endif /* MEDIA_VISION_FACE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_face_recognize(
        mv_source_h source,
        mv_face_recognition_model_h recognition_model,
        mv_engine_config_h engine_cfg,
        mv_rectangle_s *face_location,
        mv_face_recognized_cb recognized_cb,
        void *user_data)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_face_check_system_info_feature_supported());
    MEDIA_VISION_INSTANCE_CHECK(source);
    MEDIA_VISION_INSTANCE_CHECK(recognition_model);
    MEDIA_VISION_NULL_ARG_CHECK(recognized_cb);

    MEDIA_VISION_FUNCTION_ENTER();

    int ret = check_source_roi(face_location, source);
    if (MEDIA_VISION_ERROR_NONE != ret)
    {
        LOGE("Errors occured when check source and ROI");
        return ret;
    }

    #ifdef MEDIA_VISION_FACE_LICENSE_PORT

    ret = mv_face_recognize_lic(
                  source,
                  recognition_model,
                  engine_cfg,
                  face_location,
                  recognized_cb,
                  user_data);

    #else

    ret = mv_face_recognize_open(
                  source,
                  recognition_model,
                  engine_cfg,
                  face_location,
                  recognized_cb,
                  user_data);

    #endif /* MEDIA_VISION_FACE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_face_track(
        mv_source_h source,
        mv_face_tracking_model_h tracking_model,
        mv_engine_config_h engine_cfg,
        mv_face_tracked_cb tracked_cb,
        bool do_learn,
        void *user_data)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_face_check_system_info_feature_supported());
    MEDIA_VISION_INSTANCE_CHECK(source);
    MEDIA_VISION_INSTANCE_CHECK(tracking_model);
    MEDIA_VISION_NULL_ARG_CHECK(tracked_cb);

    MEDIA_VISION_FUNCTION_ENTER();

    int ret = MEDIA_VISION_ERROR_NONE;

    #ifdef MEDIA_VISION_FACE_LICENSE_PORT

    ret = mv_face_track_lic(
            source,
            tracking_model,
            engine_cfg,
            tracked_cb,
            do_learn,
            user_data);

    #else

    ret = mv_face_track_open(
            source,
            tracking_model,
            engine_cfg,
            tracked_cb,
            do_learn,
            user_data);

    #endif /* MEDIA_VISION_FACE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_face_eye_condition_recognize(
        mv_source_h source,
        mv_engine_config_h engine_cfg,
        mv_rectangle_s face_location,
        mv_face_eye_condition_recognized_cb eye_condition_recognized_cb,
        void *user_data)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_face_check_system_info_feature_supported());
    MEDIA_VISION_INSTANCE_CHECK(source);
    MEDIA_VISION_NULL_ARG_CHECK(eye_condition_recognized_cb);

    MEDIA_VISION_FUNCTION_ENTER();

    int ret = MEDIA_VISION_ERROR_NONE;

    #ifdef MEDIA_VISION_FACE_LICENSE_PORT

    ret = mv_face_eye_condition_recognize_lic(
              source,
              engine_cfg,
              face_location,
              eye_condition_recognized_cb,
              user_data);

    #else

    ret = mv_face_eye_condition_recognize_open(
              source,
              engine_cfg,
              face_location,
              eye_condition_recognized_cb,
              user_data);

    #endif /* MEDIA_VISION_FACE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_face_facial_expression_recognize(
        mv_source_h source,
        mv_engine_config_h engine_cfg,
        mv_rectangle_s face_location,
        mv_face_facial_expression_recognized_cb expression_recognized_cb,
        void *user_data)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_face_check_system_info_feature_supported());
    MEDIA_VISION_INSTANCE_CHECK(source);
    MEDIA_VISION_NULL_ARG_CHECK(expression_recognized_cb);

    MEDIA_VISION_FUNCTION_ENTER();

    int ret = check_source_roi(&face_location, source);
    if (MEDIA_VISION_ERROR_NONE != ret)
    {
        LOGE("Errors occurred when check source and ROI");
        return ret;
    }

    #ifdef MEDIA_VISION_FACE_LICENSE_PORT

    ret = mv_face_facial_expression_recognize_lic(
                  source,
                  engine_cfg,
                  face_location,
                  expression_recognized_cb,
                  user_data);

    #else

    ret = mv_face_facial_expression_recognize_open(
                  source,
                  engine_cfg,
                  face_location,
                  expression_recognized_cb,
                  user_data);

    #endif /* MEDIA_VISION_FACE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_face_recognition_model_create(
        mv_face_recognition_model_h *recognition_model)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_face_check_system_info_feature_supported());
    MEDIA_VISION_NULL_ARG_CHECK(recognition_model);

    MEDIA_VISION_FUNCTION_ENTER();

    int ret = MEDIA_VISION_ERROR_NONE;

    #ifdef MEDIA_VISION_FACE_LICENSE_PORT

    ret = mv_face_recognition_model_create_lic(recognition_model);

    #else

    ret = mv_face_recognition_model_create_open(recognition_model);

    #endif /* MEDIA_VISION_FACE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_face_recognition_model_destroy(
        mv_face_recognition_model_h recognition_model)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_face_check_system_info_feature_supported());
    MEDIA_VISION_INSTANCE_CHECK(recognition_model);

    MEDIA_VISION_FUNCTION_ENTER();

    int ret = MEDIA_VISION_ERROR_NONE;

    #ifdef MEDIA_VISION_FACE_LICENSE_PORT

    ret = mv_face_recognition_model_destroy_lic(recognition_model);

    #else

    ret = mv_face_recognition_model_destroy_open(recognition_model);

    #endif /* MEDIA_VISION_FACE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_face_recognition_model_clone(
        mv_face_recognition_model_h src,
        mv_face_recognition_model_h *dst)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_face_check_system_info_feature_supported());
    MEDIA_VISION_INSTANCE_CHECK(src);
    MEDIA_VISION_NULL_ARG_CHECK(dst);

    MEDIA_VISION_FUNCTION_ENTER();

    int ret = MEDIA_VISION_ERROR_NONE;

    #ifdef MEDIA_VISION_FACE_LICENSE_PORT

    ret = mv_face_recognition_model_clone_lic(src, dst);

    #else

    ret = mv_face_recognition_model_clone_open(src, dst);

    #endif /* MEDIA_VISION_FACE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_face_recognition_model_save(
        const char *file_name,
        mv_face_recognition_model_h recognition_model)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_face_check_system_info_feature_supported());
    MEDIA_VISION_INSTANCE_CHECK(recognition_model);

    if (file_name == NULL)
    {
         LOGE("File name is NULL. The file name has to be specified");
         return MEDIA_VISION_ERROR_INVALID_PATH;
    }

    MEDIA_VISION_FUNCTION_ENTER();

    int ret = MEDIA_VISION_ERROR_NONE;

    #ifdef MEDIA_VISION_FACE_LICENSE_PORT

    ret = mv_face_recognition_model_save_lic(
                  file_name,
                  recognition_model);

    #else

    ret = mv_face_recognition_model_save_open(
                  file_name,
                  recognition_model);

    #endif /* MEDIA_VISION_FACE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_face_recognition_model_load(
        const char *file_name,
        mv_face_recognition_model_h *recognition_model)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_face_check_system_info_feature_supported());
    MEDIA_VISION_NULL_ARG_CHECK(recognition_model);

    if (file_name == NULL)
    {
         LOGE("File name is NULL. The file name has to be specified");
         return MEDIA_VISION_ERROR_INVALID_PATH;
    }

    MEDIA_VISION_FUNCTION_ENTER();

    int ret = MEDIA_VISION_ERROR_NONE;

    #ifdef MEDIA_VISION_FACE_LICENSE_PORT

    ret = mv_face_recognition_model_load_lic(
                  file_name,
                  recognition_model);

    #else

    ret = mv_face_recognition_model_load_open(
                  file_name,
                  recognition_model);

    #endif /* MEDIA_VISION_FACE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_face_recognition_model_add(
        const mv_source_h source,
        mv_face_recognition_model_h recognition_model,
        const mv_rectangle_s *example_location,
        int face_label)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_face_check_system_info_feature_supported());
    MEDIA_VISION_INSTANCE_CHECK(source);
    MEDIA_VISION_INSTANCE_CHECK(recognition_model);

    MEDIA_VISION_FUNCTION_ENTER();

    int ret = check_source_roi(example_location, source);
    if (MEDIA_VISION_ERROR_NONE != ret)
    {
        LOGE("Errors occured when check source and ROI");
        return ret;
    }

    #ifdef MEDIA_VISION_FACE_LICENSE_PORT

    ret = mv_face_recognition_model_add_lic(
                  source,
                  recognition_model,
                  example_location,
                  face_label);
    #else

    ret = mv_face_recognition_model_add_open(
                  source,
                  recognition_model,
                  example_location,
                  face_label);

    #endif /* MEDIA_VISION_FACE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_face_recognition_model_reset(
        mv_face_recognition_model_h recognition_model,
        int *face_label)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_face_check_system_info_feature_supported());
    MEDIA_VISION_INSTANCE_CHECK(recognition_model);

    MEDIA_VISION_FUNCTION_ENTER();

    int ret = MEDIA_VISION_ERROR_NONE;

    #ifdef MEDIA_VISION_FACE_LICENSE_PORT

    ret = mv_face_recognition_model_reset_lic(
                  recognition_model,
                  face_label);

    #else

    ret = mv_face_recognition_model_reset_open(
                  recognition_model,
                  face_label);

    #endif /* MEDIA_VISION_FACE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_face_recognition_model_learn(
        mv_engine_config_h engine_cfg,
        mv_face_recognition_model_h recognition_model)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_face_check_system_info_feature_supported());
    MEDIA_VISION_INSTANCE_CHECK(recognition_model);

    MEDIA_VISION_FUNCTION_ENTER();

    int ret = MEDIA_VISION_ERROR_NONE;

    #ifdef MEDIA_VISION_FACE_LICENSE_PORT

    ret = mv_face_recognition_model_learn_lic(engine_cfg, recognition_model);

    #else

    ret = mv_face_recognition_model_learn_open(engine_cfg, recognition_model);

    #endif /* MEDIA_VISION_FACE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_face_recognition_model_query_labels(
        mv_face_recognition_model_h recognition_model,
        int **labels,
        unsigned int *number_of_labels)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_face_check_system_info_feature_supported());
    MEDIA_VISION_INSTANCE_CHECK(recognition_model);
    MEDIA_VISION_NULL_ARG_CHECK(labels);
    MEDIA_VISION_NULL_ARG_CHECK(number_of_labels);

    MEDIA_VISION_FUNCTION_ENTER();

    int ret = MEDIA_VISION_ERROR_NONE;

    #ifdef MEDIA_VISION_FACE_LICENSE_PORT

    ret = mv_face_recognition_model_query_labels_lic(recognition_model, labels, number_of_labels);

    #else

    ret = mv_face_recognition_model_query_labels_open(recognition_model, labels, number_of_labels);

    #endif /* MEDIA_VISION_FACE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_face_tracking_model_create(
        mv_face_tracking_model_h *tracking_model)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_face_check_system_info_feature_supported());
    MEDIA_VISION_NULL_ARG_CHECK(tracking_model);

    MEDIA_VISION_FUNCTION_ENTER();

    int ret = MEDIA_VISION_ERROR_NONE;

    #ifdef MEDIA_VISION_FACE_LICENSE_PORT

    ret = mv_face_tracking_model_create_lic(tracking_model);

    #else

    ret = mv_face_tracking_model_create_open(tracking_model);

    #endif /* MEDIA_VISION_FACE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_face_tracking_model_destroy(
        mv_face_tracking_model_h tracking_model)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_face_check_system_info_feature_supported());
    MEDIA_VISION_INSTANCE_CHECK(tracking_model);

    MEDIA_VISION_FUNCTION_ENTER();

    int ret = MEDIA_VISION_ERROR_NONE;

    #ifdef MEDIA_VISION_FACE_LICENSE_PORT

    ret = mv_face_tracking_model_destroy_lic(tracking_model);

    #else

    ret = mv_face_tracking_model_destroy_open(tracking_model);

    #endif /* MEDIA_VISION_FACE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_face_tracking_model_prepare(
        mv_face_tracking_model_h tracking_model,
        mv_engine_config_h engine_cfg,
        mv_source_h source,
        mv_quadrangle_s *location)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_face_check_system_info_feature_supported());
    MEDIA_VISION_INSTANCE_CHECK(tracking_model);
    MEDIA_VISION_INSTANCE_CHECK(source);

    MEDIA_VISION_FUNCTION_ENTER();

    int ret = check_source_roi_quadrangle(location, source);
    if (MEDIA_VISION_ERROR_NONE != ret)
    {
        LOGE("Errors occured when check source and tracking start location");
        return ret;
    }

    #ifdef MEDIA_VISION_FACE_LICENSE_PORT

    ret = mv_face_tracking_model_prepare_lic(
              tracking_model, engine_cfg, source, location);

    #else

    ret = mv_face_tracking_model_prepare_open(
              tracking_model, engine_cfg, source, location);

    #endif /* MEDIA_VISION_FACE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_face_tracking_model_clone(
        mv_face_tracking_model_h src,
        mv_face_tracking_model_h *dst)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_face_check_system_info_feature_supported());
    MEDIA_VISION_INSTANCE_CHECK(src);
    MEDIA_VISION_NULL_ARG_CHECK(dst);

    MEDIA_VISION_FUNCTION_ENTER();

    int ret = MEDIA_VISION_ERROR_NONE;

    #ifdef MEDIA_VISION_FACE_LICENSE_PORT

    ret = mv_face_tracking_model_clone_lic(src, dst);

    #else

    ret = mv_face_tracking_model_clone_open(src, dst);

    #endif /* MEDIA_VISION_FACE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_face_tracking_model_save(
        const char *file_name,
        mv_face_tracking_model_h tracking_model)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_face_check_system_info_feature_supported());
    MEDIA_VISION_INSTANCE_CHECK(tracking_model);

    if (file_name == NULL)
    {
         LOGE("File name is NULL. The file name has to be specified");
         return MEDIA_VISION_ERROR_INVALID_PATH;
    }

    MEDIA_VISION_FUNCTION_ENTER();

    int ret = MEDIA_VISION_ERROR_NONE;

    #ifdef MEDIA_VISION_FACE_LICENSE_PORT

    ret = mv_face_tracking_model_save_lic(
              file_name,
              tracking_model);

    #else

    ret = mv_face_tracking_model_save_open(
              file_name,
              tracking_model);

    #endif /* MEDIA_VISION_FACE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_face_tracking_model_load(
        const char *file_name,
        mv_face_tracking_model_h *tracking_model)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_face_check_system_info_feature_supported());
    MEDIA_VISION_NULL_ARG_CHECK(tracking_model);

    if (file_name == NULL)
    {
         LOGE("File name is NULL. The file name has to be specifiled");
         return MEDIA_VISION_ERROR_INVALID_PATH;
    }

    MEDIA_VISION_FUNCTION_ENTER();

    int ret = MEDIA_VISION_ERROR_NONE;

    #ifdef MEDIA_VISION_FACE_LICENSE_PORT

    ret = mv_face_tracking_model_load_lic(
              file_name,
              tracking_model);

    #else

    ret = mv_face_tracking_model_load_open(
              file_name,
              tracking_model);

    #endif /* MEDIA_VISION_FACE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}
