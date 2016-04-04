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

#ifndef __MEDIA_VISION_MV_APPLY_MASK_H__
#define __MEDIA_VISION_MV_APPLY_MASK_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Applies a binary mask to the input buffer.
 * @details Works only with grayscale buffers.
 *
 * @since_tizen 3.0
 * @remarks If NEON is enabled (ENABLE_NEON flag), then @a width has to be
 *          multiple of 16
 * @param [in] src_buffer     The source buffer.
 * @param [in] mask           The mask, which should contain only values of 0 or 255.
 * @param [in] width          The image width.
 * @param [in] height         The image height.
 * @param [in] stride         The stride.
 * @param [out] dst_buffer    The destination buffer.
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 */
EXPORT_API int mv_apply_mask(
		uint8_t *src_buffer,
		uint8_t *__restrict mask,
		int width,
		int height,
		int stride,
		uint8_t *dst_buffer);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MEDIA_VISION_MV_APPLY_MASK_H__ */

