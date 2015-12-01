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

#include "mv_absdiff.h"

#include "mv_common.h"
#include "mv_private.h"

#ifdef ENABLE_NEON
#include <arm_neon.h>
#endif

int mv_absdiff(
		uint8_t *__restrict__ src1,
		uint8_t *__restrict__ src2,
		int width,
		int height,
		int stride,
		uint8_t *__restrict__ dst)
{
	if (src1 == NULL || src2 == NULL || width <= 0 || height <= 0 ||
			stride <= 0 || dst == NULL) {
		LOGE("Wrong input parameter. Aplpying mask failed.");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	int column = 0;
	int row = 0;
	const int padding = stride - width;

#ifdef ENABLE_NEON
	const int batch_size = 16;
	const int batch_columns_count = width / batch_size;
#endif

	for (; row < height; ++row) {
#ifdef ENABLE_NEON
		for (column = 0; column < batch_columns_count; ++column) {
			uint8x16_t gray1 = vld1q_u8 (src1);
			uint8x16_t gray2 = vld1q_u8 (src2);

			uint8x16_t dst_temp = vabdq_u8(gray1, gray2);

			vst1q_u8 (dst, dst_temp);

			src1 += batch_size;
			src2 += batch_size;
			dst += batch_size;
		}
#else
		for (column = 0; column < width; ++column) {
			uint8_t gray1 = *src1;
			uint8_t gray2 = *src2;

			(*dst) = abs((*src1) - (*src2));

			++src1;
			++src2;
			++dst;
		}
#endif
		src1 += padding;
		src2 += padding;
		dst += padding;
	}

	return MEDIA_VISION_ERROR_NONE;
}
