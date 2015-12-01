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

#include "mv_apply_mask.h"

#include "mv_common.h"
#include "mv_private.h"

#ifdef ENABLE_NEON
#include <arm_neon.h>
#endif

int mv_apply_mask(
	uint8_t *src_buffer,
	uint8_t *__restrict mask,
	int width,
	int height,
	int stride,
	uint8_t *dst_buffer)
{
	if (src_buffer == NULL || mask == NULL || width <= 0 || height <= 0 ||
			stride <= 0 || dst_buffer == NULL) {
		LOGE("Wrong input parameter. Aplpying mask failed.");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	const int padding = stride - width;

#ifdef ENABLE_NEON
	const int batch_size = 16;
	const int batch_columns_count = width / batch_size;
#endif

	int row = 0;
	int column = 0;
	for (; row < height; ++row) {
#ifdef ENABLE_NEON
		for (column = 0; column < batch_columns_count; ++column) {
			uint8x16_t src_v = vld1q_u8(src_buffer);
			uint8x16_t mask_v = vld1q_u8(mask);

			uint8x16_t dst_v = vandq_u8(src_v, mask_v);

			vst1q_u8(dst_buffer, dst_v);

			dst_buffer += batch_size;
			src_buffer += batch_size;
			mask += batch_size;
		}
#else
		for (column = 0; column < width; ++column) {
			(*dst_buffer) = ((*src_buffer) & (*mask));
			++dst_buffer;
			++src_buffer;
			++mask;
		}
#endif
		dst_buffer += padding;
		src_buffer += padding;
		mask += padding;
	}

	return MEDIA_VISION_ERROR_NONE;
}
