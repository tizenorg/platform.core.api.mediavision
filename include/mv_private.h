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

#ifndef __TIZEN_MEDIA_VISION_PRIVATE_H__
#define __TIZEN_MEDIA_VISION_PRIVATE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdbool.h>
#include <dlog.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "TIZEN_MEDIA_VISION"

#define MEDIA_VISION_FUNCTION_ENTER() \
        LOGI("[%s] <ENTER>", __FUNCTION__)

#define MEDIA_VISION_FUNCTION_LEAVE() \
        LOGI("[%s] <LEAVE>", __FUNCTION__)

#define MEDIA_VISION_ASSERT(function, msg) \
        do \
        { \
            int error_code = function; \
            if(error_code != MEDIA_VISION_ERROR_NONE) \
            { \
                LOGE("%s(0x%08x)", msg, error_code); \
                return error_code; \
            } \
        } \
        while(0)

#define MEDIA_VISION_CHECK_CONDITION(condition,error,msg) \
        do \
        { \
            if(!(condition)) \
            { \
                LOGE("[%s] %s(0x%08x)", __FUNCTION__, msg, error); \
                return error; \
            } \
        } \
        while(0)

#define MEDIA_VISION_INSTANCE_CHECK(arg) \
        MEDIA_VISION_CHECK_CONDITION(arg != NULL, \
                                     MEDIA_VISION_ERROR_INVALID_PARAMETER, \
                                     "MEDIA_VISION_ERROR_INVALID_PARAMETER")

#define MEDIA_VISION_NULL_ARG_CHECK(arg) \
        MEDIA_VISION_CHECK_CONDITION(arg != NULL, \
                                     MEDIA_VISION_ERROR_INVALID_PARAMETER, \
                                     "MEDIA_VISION_ERROR_INVALID_PARAMETER")

#define MEDIA_VISION_SUPPORT_CHECK(arg) \
        MEDIA_VISION_CHECK_CONDITION(arg != false, MEDIA_VISION_ERROR_NOT_SUPPORTED, \
                                     "MEDIA_VISION_ERROR_NOT_SUPPORTED")

bool __mv_check_system_info_feature_supported();
bool __mv_barcode_detect_check_system_info_feature_supported();
bool __mv_barcode_generate_check_system_info_feature_supported();
bool __mv_face_check_system_info_feature_supported();
bool __mv_image_check_system_info_feature_supported();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TIZEN_MEDIA_VISION_PRIVATE_H__ */
