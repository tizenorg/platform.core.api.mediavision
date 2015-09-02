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
#include "mv_image.h"

#ifdef MEDIA_VISION_IMAGE_LICENSE_PORT

/* Include headers of licensed image module here. */
#include "mv_image_lic.h"

#else

/* Include headers of open image module here. */
#include "mv_image_open.h"

#endif /* MEDIA_VISION_IMAGE_LICENSE_PORT */

/**
 * @file  mv_image.c
 * @brief This file contains the porting layer for Media Vision image module.
 */

int mv_image_recognize(
        mv_source_h source,
        const mv_image_object_h *image_objects,
        int number_of_objects,
        mv_engine_config_h engine_cfg,
        mv_image_recognized_cb recognized_cb,
        void *user_data)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_image_check_system_info_feature_supported);
    MEDIA_VISION_INSTANCE_CHECK(source);
    MEDIA_VISION_NULL_ARG_CHECK(image_objects);
    int object_num = 0;
    for (; object_num < number_of_objects; ++object_num)
    {
        MEDIA_VISION_INSTANCE_CHECK(image_objects[object_num]);
    }
    MEDIA_VISION_NULL_ARG_CHECK(recognized_cb);

    MEDIA_VISION_FUNCTION_ENTER();

#ifdef MEDIA_VISION_IMAGE_LICENSE_PORT

    /* Use licensed image functionality here. */
    int ret = mv_image_recognize_lic(source, image_objects,
            number_of_objects, engine_cfg, recognized_cb, user_data);

#else

    /* Use open image functionality here. */
    int ret = mv_image_recognize_open(source, image_objects,
            number_of_objects, engine_cfg, recognized_cb, user_data);

#endif /* MEDIA_VISION_IMAGE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_image_track(
        mv_source_h source,
        mv_image_tracking_model_h image_tracking_model,
        mv_engine_config_h engine_cfg,
        mv_image_tracked_cb tracked_cb,
        void *user_data)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_image_check_system_info_feature_supported);
    MEDIA_VISION_INSTANCE_CHECK(source);
    MEDIA_VISION_INSTANCE_CHECK(image_tracking_model);
    MEDIA_VISION_NULL_ARG_CHECK(tracked_cb);

    MEDIA_VISION_FUNCTION_ENTER();

#ifdef MEDIA_VISION_IMAGE_LICENSE_PORT

    /* Use licensed image functionality here. */
    int ret = mv_image_track_lic(source, image_tracking_model, engine_cfg, tracked_cb, user_data);

#else

    /* Use open image functionality here. */
    int ret = mv_image_track_open(source, image_tracking_model, engine_cfg, tracked_cb, user_data);

#endif /* MEDIA_VISION_IMAGE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_image_object_create(
        mv_image_object_h *image_object)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_image_check_system_info_feature_supported);
    MEDIA_VISION_NULL_ARG_CHECK(image_object);
    MEDIA_VISION_FUNCTION_ENTER();

#ifdef MEDIA_VISION_IMAGE_LICENSE_PORT

    /* Use licensed image functionality here. */
    int ret = mv_image_object_create_lic(image_object);

#else

    /* Use open image functionality here. */
    int ret = mv_image_object_create_open(image_object);

#endif /* MEDIA_VISION_IMAGE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_image_object_destroy(
        mv_image_object_h image_object)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_image_check_system_info_feature_supported);
    MEDIA_VISION_INSTANCE_CHECK(image_object);
    MEDIA_VISION_FUNCTION_ENTER();

#ifdef MEDIA_VISION_IMAGE_LICENSE_PORT

    /* Use licensed image functionality here. */
    int ret = mv_image_object_destroy_lic(image_object);

#else

    /* Use open image functionality here. */
    int ret = mv_image_object_destroy_open(image_object);

#endif /* MEDIA_VISION_IMAGE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_image_object_fill(
        mv_image_object_h image_object,
        mv_engine_config_h engine_cfg,
        mv_source_h source,
        mv_rectangle_s *location)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_image_check_system_info_feature_supported);
    MEDIA_VISION_INSTANCE_CHECK(image_object);
    MEDIA_VISION_INSTANCE_CHECK(source);

    MEDIA_VISION_FUNCTION_ENTER();

#ifdef MEDIA_VISION_IMAGE_LICENSE_PORT

    /* Use licensed image functionality here. */
    int ret = mv_image_object_fill_lic(image_object, engine_cfg, source, location);

#else

    /* Use open image functionality here. */
    int ret = mv_image_object_fill_open(image_object, engine_cfg, source, location);

#endif /* MEDIA_VISION_IMAGE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_image_object_get_recognition_rate(
        mv_image_object_h image_object,
        double *recognition_rate)
{
	MEDIA_VISION_SUPPORT_CHECK(__mv_image_check_system_info_feature_supported);
    MEDIA_VISION_INSTANCE_CHECK(image_object);
    MEDIA_VISION_NULL_ARG_CHECK(recognition_rate);

    MEDIA_VISION_FUNCTION_ENTER();

#ifdef MEDIA_VISION_IMAGE_LICENSE_PORT

    /* Use licensed image functionality here. */
    int ret = mv_image_object_get_recognition_rate_lic(image_object, recognition_rate);

#else

    /* Use open image functionality here. */
    int ret = mv_image_object_get_recognition_rate_open(image_object, recognition_rate);

#endif /* MEDIA_VISION_IMAGE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_image_object_set_label(
        mv_image_object_h image_object,
        int label)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_image_check_system_info_feature_supported);
    MEDIA_VISION_INSTANCE_CHECK(image_object);

    MEDIA_VISION_FUNCTION_ENTER();

#ifdef MEDIA_VISION_IMAGE_LICENSE_PORT

    /* Use licensed image functionality here. */
    int ret = mv_image_object_set_label_lic(image_object, label);

#else

    /* Use open image functionality here. */
    int ret = mv_image_object_set_label_open(image_object, label);

#endif /* MEDIA_VISION_IMAGE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_image_object_get_label(
        mv_image_object_h image_object,
        int *label)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_image_check_system_info_feature_supported);
    MEDIA_VISION_INSTANCE_CHECK(image_object);
    MEDIA_VISION_NULL_ARG_CHECK(label);

    MEDIA_VISION_FUNCTION_ENTER();

#ifdef MEDIA_VISION_IMAGE_LICENSE_PORT

    /* Use licensed image functionality here. */
    int ret = mv_image_object_get_label_lic(image_object, label);

#else

    /* Use open image functionality here. */
    int ret = mv_image_object_get_label_open(image_object, label);

#endif /* MEDIA_VISION_IMAGE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_image_object_clone(
        mv_image_object_h src,
        mv_image_object_h *dst)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_image_check_system_info_feature_supported);
    MEDIA_VISION_INSTANCE_CHECK(src);
    MEDIA_VISION_NULL_ARG_CHECK(dst);

    MEDIA_VISION_FUNCTION_ENTER();

#ifdef MEDIA_VISION_IMAGE_LICENSE_PORT

    /* Use licensed image functionality here. */
    int ret = mv_image_object_clone_lic(src, dst);

#else

    /* Use open image functionality here. */
    int ret = mv_image_object_clone_open(src, dst);

#endif /* MEDIA_VISION_IMAGE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_image_object_save(
        const char *file_name, mv_image_object_h image_object)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_image_check_system_info_feature_supported);
    MEDIA_VISION_INSTANCE_CHECK(image_object);

    if (file_name == NULL)
    {
         LOGE("File name is NULL. The file name has to be specified");
         return MEDIA_VISION_ERROR_INVALID_PATH;
    }

    MEDIA_VISION_FUNCTION_ENTER();

#ifdef MEDIA_VISION_IMAGE_LICENSE_PORT

    /* Use licensed image functionality here. */
    int ret = mv_image_object_save_lic(file_name, image_object);

#else

    /* Use open image functionality here. */
    int ret = mv_image_object_save_open(file_name, image_object);

#endif /* MEDIA_VISION_IMAGE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_image_object_load(
        const char *file_name, mv_image_object_h *image_object)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_image_check_system_info_feature_supported);
    MEDIA_VISION_NULL_ARG_CHECK(image_object);

    if (file_name == NULL)
    {
         LOGE("file name is NULL. The file name has to be specified");
         return MEDIA_VISION_ERROR_INVALID_PATH;
    }

    MEDIA_VISION_FUNCTION_ENTER();

#ifdef MEDIA_VISION_IMAGE_LICENSE_PORT

    /* Use licensed image functionality here. */
    int ret = mv_image_object_load_lic(file_name, image_object);

#else

    /* Use open image functionality here. */
    int ret = mv_image_object_load_open(file_name, image_object);

#endif /* MEDIA_VISION_IMAGE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_image_tracking_model_create(
        mv_image_tracking_model_h *image_tracking_model)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_image_check_system_info_feature_supported);
    MEDIA_VISION_NULL_ARG_CHECK(image_tracking_model);

    MEDIA_VISION_FUNCTION_ENTER();

#ifdef MEDIA_VISION_IMAGE_LICENSE_PORT

    /* Use licensed image functionality here. */
    int ret = mv_image_tracking_model_create_lic(image_tracking_model);

#else

    /* Use open image functionality here. */
    int ret = mv_image_tracking_model_create_open(image_tracking_model);

#endif /* MEDIA_VISION_IMAGE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_image_tracking_model_set_target(
        mv_image_object_h image_object,
        mv_image_tracking_model_h image_tracking_model)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_image_check_system_info_feature_supported);
    MEDIA_VISION_INSTANCE_CHECK(image_tracking_model);
    MEDIA_VISION_INSTANCE_CHECK(image_object);

    MEDIA_VISION_FUNCTION_ENTER();

#ifdef MEDIA_VISION_IMAGE_LICENSE_PORT

    /* Use licensed image functionality here. */
    int ret = mv_image_tracking_model_set_target_lic(image_object, image_tracking_model);

#else

    /* Use open image functionality here. */
    int ret = mv_image_tracking_model_set_target_open(image_object, image_tracking_model);

#endif /* MEDIA_VISION_IMAGE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_image_tracking_model_destroy(
        mv_image_tracking_model_h image_tracking_model)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_image_check_system_info_feature_supported);
    MEDIA_VISION_INSTANCE_CHECK(image_tracking_model);

    MEDIA_VISION_FUNCTION_ENTER();

#ifdef MEDIA_VISION_IMAGE_LICENSE_PORT

    /* Use licensed image functionality here. */
    int ret = mv_image_tracking_model_destroy_lic(image_tracking_model);

#else

    /* Use open image functionality here. */
    int ret = mv_image_tracking_model_destroy_open(image_tracking_model);

#endif /* MEDIA_VISION_IMAGE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_image_tracking_model_refresh(
        mv_image_tracking_model_h image_tracking_model,
        mv_engine_config_h engine_cfg)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_image_check_system_info_feature_supported);
    MEDIA_VISION_INSTANCE_CHECK(image_tracking_model);

    MEDIA_VISION_FUNCTION_ENTER();

#ifdef MEDIA_VISION_IMAGE_LICENSE_PORT

    /* Use licensed image functionality here. */
    int ret = mv_image_tracking_model_refresh_lic(
                  image_tracking_model,
                  engine_cfg);

#else

    /* Use open image functionality here. */
    int ret = mv_image_tracking_model_refresh_open(
                  image_tracking_model,
                  engine_cfg);

#endif /* MEDIA_VISION_IMAGE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_image_tracking_model_clone(
        mv_image_tracking_model_h src,
        mv_image_tracking_model_h *dst)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_image_check_system_info_feature_supported);
    MEDIA_VISION_INSTANCE_CHECK(src);
    MEDIA_VISION_NULL_ARG_CHECK(dst);

    MEDIA_VISION_FUNCTION_ENTER();

#ifdef MEDIA_VISION_IMAGE_LICENSE_PORT

    /* Use licensed image functionality here. */
    int ret = mv_image_tracking_model_clone_lic(src, dst);

#else

    /* Use open image functionality here. */
    int ret = mv_image_tracking_model_clone_open(src, dst);

#endif /* MEDIA_VISION_IMAGE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_image_tracking_model_save(
        const char *file_name, mv_image_tracking_model_h image_tracking_model)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_image_check_system_info_feature_supported);
    MEDIA_VISION_INSTANCE_CHECK(image_tracking_model);

    if (file_name == NULL)
    {
         LOGE("File name is NULL. The file name has to be specified");
         return MEDIA_VISION_ERROR_INVALID_PATH;
    }

    MEDIA_VISION_FUNCTION_ENTER();

#ifdef MEDIA_VISION_IMAGE_LICENSE_PORT

    /* Use licensed image functionality here. */
    int ret = mv_image_tracking_model_save_lic(file_name, image_tracking_model);

#else

    /* Use open image functionality here. */
    int ret = mv_image_tracking_model_save_open(file_name, image_tracking_model);

#endif /* MEDIA_VISION_IMAGE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}

int mv_image_tracking_model_load(
        const char *file_name, mv_image_tracking_model_h *image_tracking_model)
{
    MEDIA_VISION_SUPPORT_CHECK(__mv_image_check_system_info_feature_supported);
    MEDIA_VISION_NULL_ARG_CHECK(image_tracking_model);

    if (file_name == NULL)
    {
         LOGE("File name is NULL. The file name has to be specified");
         return MEDIA_VISION_ERROR_INVALID_PATH;
    }

    MEDIA_VISION_FUNCTION_ENTER();

#ifdef MEDIA_VISION_IMAGE_LICENSE_PORT

    /* Use licensed image functionality here. */
    int ret = mv_image_tracking_model_load_lic(file_name, image_tracking_model);

#else

    /* Use open image functionality here. */
    int ret = mv_image_tracking_model_load_open(file_name, image_tracking_model);

#endif /* MEDIA_VISION_IMAGE_LICENSE_PORT */

    MEDIA_VISION_FUNCTION_LEAVE();
    return ret;
}
