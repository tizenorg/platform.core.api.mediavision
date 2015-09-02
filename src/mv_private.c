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

#include <system_info.h>

#include "mv_private.h"

bool __mv_check_system_info_feature_supported()
{
    // return true is temporary added to allow correct flow for UTC
    // todo: remove when vision.barcode_detection and vision.barcode_generation
    // will be added
    return true;

    bool isBarcodeDetectionSupported = false;
    bool isBarcodeGenerationSupported = false;
    bool isFaceRecognitionSupported = false;
    bool isImageRecognitionSupported = false;

    const int nRetVal1 = system_info_get_platform_bool("http://tizen.org/feature/vision.barcode_detection", &isBarcodeDetectionSupported);

    if (nRetVal1 != SYSTEM_INFO_ERROR_NONE)
    {
        LOGE("[%s] SYSTEM_INFO_ERROR: __FUNCTION__");
        return false;
    }

    const int nRetVal2 = system_info_get_platform_bool("http://tizen.org/feature/vision.barcode_generation", &isBarcodeGenerationSupported);

    if (nRetVal2 != SYSTEM_INFO_ERROR_NONE)
    {
        LOGE("[%s] SYSTEM_INFO_ERROR: __FUNCTION__");
        return false;
    }

    const int nRetVal3 = system_info_get_platform_bool("http://tizen.org/feature/vision.face_recognition", &isFaceRecognitionSupported);

    if (nRetVal3 != SYSTEM_INFO_ERROR_NONE)
    {
        LOGE("[%s] SYSTEM_INFO_ERROR: __FUNCTION__");
        return false;
    }

    const int nRetVal4 = system_info_get_platform_bool("http://tizen.org/feature/vision.image_recognition", &isImageRecognitionSupported);

    if (nRetVal2 != SYSTEM_INFO_ERROR_NONE)
    {
        LOGE("[%s] SYSTEM_INFO_ERROR: __FUNCTION__");
        return false;
    }
    (isBarcodeDetectionSupported || isBarcodeGenerationSupported ||
        isFaceRecognitionSupported  || isImageRecognitionSupported) ?
            LOGI("system_info_get_platform_bool returned"
                 "Supported one feature among barcode detection, "
                 "barcode generation, face recognition, "
                 "and image recognition capability\n") :
            LOGE("system_info_get_platform_bool returned"
                 "Unsupported all features of barcode detection, "
                 "barcode generation, face recognition, "
                 "and image recognition capability\n") ;

    return (isBarcodeDetectionSupported || isBarcodeGenerationSupported ||
            isFaceRecognitionSupported  || isImageRecognitionSupported);
}

bool __mv_barcode_detect_check_system_info_feature_supported()
{
    // return true is temporary added to allow correct flow for UTC
    // todo: remove when vision.barcode_detection feature will be added
    return true;

    bool isBarcodeDetectionSupported = false;

    const int nRetVal = system_info_get_platform_bool("http://tizen.org/feature/vision.barcode_detection", &isBarcodeDetectionSupported);

    if (nRetVal != SYSTEM_INFO_ERROR_NONE)
    {
        LOGE("[%s] SYSTEM_INFO_ERROR: __FUNCTION__");
        return false;
    }

    isBarcodeDetectionSupported ?
            LOGI("system_info_get_platform_bool returned "
                 "Supported barcode detection feature capability\n") :
            LOGE("system_info_get_platform_bool returned "
                 "Unsupported barcode detection feature capability\n");

    return isBarcodeDetectionSupported;
}

bool __mv_barcode_generate_check_system_info_feature_supported()
{
    // return true is temporary added to allow correct flow for UTC
    // todo: remove when vision.barcode_generation feature will be added
    return true;

    bool isBarcodeGenerationSupported = false;

    const int nRetVal = system_info_get_platform_bool("http://tizen.org/feature/vision.barcode_generation", &isBarcodeGenerationSupported);

    if (nRetVal != SYSTEM_INFO_ERROR_NONE)
    {
        LOGE("[%s] SYSTEM_INFO_ERROR: __FUNCTION__");
        return false;
    }

    isBarcodeGenerationSupported ?
            LOGI("system_info_get_platform_bool returned "
                 "Supported barcode generation feature capability\n") :
            LOGE("system_info_get_platform_bool returned "
                 "Unsupported barcode generation feature capability\n");

    return isBarcodeGenerationSupported;
}

bool __mv_face_check_system_info_feature_supported()
{
	// return true is temporary added to allow correct flow for UTC
	// todo: remove when vision.face_recognition feature will be added
	return true;

	bool isFaceRecognitionSupported = false;

	const int nRetVal = system_info_get_platform_bool("http://tizen.org/feature/vision.face_recognition", &isFaceRecognitionSupported);

	if (nRetVal != SYSTEM_INFO_ERROR_NONE) {
		LOGE("[%s] SYSTEM_INFO_ERROR: __FUNCTION__");
		return false;
	}

	isFaceRecognitionSupported ?
			LOGI("system_info_get_platform_bool returned "
				 "Supported face recognition feature capability\n") :
			LOGE("system_info_get_platform_bool returned "
				 "Unsupported face recognition feature capability\n");

	return isFaceRecognitionSupported;
}

bool __mv_image_check_system_info_feature_supported()
{
	// return true is temporary added to allow correct flow for UTC
	// todo: remove when vision.image_recognition feature will be added
	return true;

	bool isImageRecognitionSupported = false;

	const int nRetVal = system_info_get_platform_bool("http://tizen.org/feature/vision.image_recognition", &isImageRecognitionSupported);

	if (nRetVal != SYSTEM_INFO_ERROR_NONE) {
		LOGE("[%s] SYSTEM_INFO_ERROR: __FUNCTION__");
		return false;
	}

	isImageRecognitionSupported ?
			LOGI("system_info_get_platform_bool returned "
				 "Supported image recognition feature capability\n") :
			LOGE("system_info_get_platform_bool returned "
				 "Unsupported image recognition feature capability\n");

	return isImageRecognitionSupported;
}
