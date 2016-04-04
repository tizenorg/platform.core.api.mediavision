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

#ifndef __MEDIA_VISION_SURVEILLANCE_HELPER_H__
#define __MEDIA_VISION_SURVEILLANCE_HELPER_H__

/**
 * @file  SurveillanceHelper.h
 * @brief This file contains interface for surveillance helper.
 */

#include <mv_common.h>

#include <opencv/cv.h>

namespace mediavision {
namespace surveillance {

/**
 * @class    SurveillanceHelper
 * @brief    This class contains surveillance helper interface (common class for
 *           surveillance module).
 *
 * @since_tizen 3.0
 */
class EXPORT_API SurveillanceHelper {
public:
	/**
	 * @brief Converts mediavision source to cv::Mat in gray scale.
	 *
	 * @since_tizen 3.0
	 * @param [in] mvSource     The input media source handle
	 * @param [out] cvSource    The outut matrix with gray scaled image
	 * @return @c 0 on success, otherwise a negative error value
	 */
	static int convertSourceMV2GrayCV(mv_source_h mvSource, cv::Mat& cvSource);

#ifdef ENABLE_NEON
	/**
	 * @brief Converts mediavision source to cv::Mat in gray scale with NEON.
	 * @details Works only with RGB color space
	 *
	 * @since_tizen 3.0
	 * @param [in] mvSource     The input media source handle
	 * @param [out] cvSource    The outut matrix with gray scaled image
	 * @return @c 0 on success, otherwise a negative error value
	 */
	static int convertSourceMVRGB2GrayCVNeon(mv_source_h mvSource, cv::Mat& cvSource);
#endif
};


} /* surveillance */
} /* mediavision */

#endif /* __MEDIA_VISION_SURVEILLANCE_HELPER_H__ */
