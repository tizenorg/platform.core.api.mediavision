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

#ifndef __MEDIA_VISION_MV_ABSDIFF_H__
#define __MEDIA_VISION_MV_ABSDIFF_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Absolute difference between two buffers.
 * @details Works only with grayscale buffers.
 *
 * @since_tizen 3.0
 * @remarks If NEON is enabled (ENABLE_NEON flag), then @a width has to be
 *          multiple of 16
 * @param [in] src1      The first input buffer.
 * @param [in] src2      The second input buffer.
 * @param [in] width     The ROI width. Must be the multiple of 16.
 * @param [in] height    The ROI height.
 * @param [in] stride    The stride.
 * @param [out] dst      The output buffer.
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 */
int mv_absdiff(
		uint8_t *__restrict__ src1,
		uint8_t *__restrict__ src2,
		int width,
		int height,
		int stride,
		uint8_t *__restrict__ dst);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MEDIA_VISION_MV_ABSDIFF_H__ */

