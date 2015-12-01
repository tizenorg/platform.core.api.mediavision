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

#include "mv_mask_buffer.h"

#include "mv_common.h"
#include "mv_private.h"

#include <stdio.h>

int mv_get_mask_buffer(
	unsigned int buffer_width,
	unsigned int buffer_height,
	mv_point_s *polygon,
	unsigned int points_number,
	unsigned char **mask_buffer)
{
	if (buffer_width == 0u || buffer_height == 0u ||
			polygon == NULL|| points_number == 0u || mask_buffer == NULL) {
		LOGE("Wrong input parameter. Getting mask buffer failed.");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	const unsigned int buffer_size = buffer_width * buffer_height;

	*mask_buffer = (unsigned char*) malloc(sizeof(unsigned char) * buffer_size);

	unsigned int i = 0u;
	unsigned int j = 0u;
	unsigned int k = 0u;

	int max_x = polygon[0].x;
	int max_y = polygon[0].y;
	int min_x = polygon[0].x;
	int min_y = polygon[0].y;

	for (k = 1u; k < points_number; ++k) {
		if (polygon[k].x > max_x) {
			max_x = polygon[k].x;
		} else if (polygon[k].x < min_x) {
			min_x = polygon[k].x;
		}

		if (polygon[k].y > max_y) {
			max_y = polygon[k].y;
		} else if (polygon[k].y < min_y) {
			min_y = polygon[k].y;
		}
	}


	for (k = 0u; k < buffer_size; ++k) {
		bool inside_polygon = false;

		const int test_x = (int) k % buffer_width;
		const int test_y = (int) k / buffer_width;

		if (test_x > max_x || test_x < min_x || test_y > max_y || test_y < min_y) {
			(*mask_buffer)[k] = 0;
			continue;
		}

		for (i = 0u, j = points_number - 1; i < points_number; j = i++) {
			if (((polygon[i].y > test_y) != (polygon[j].y > test_y)) &&
					((float) test_x < (float) (polygon[j].x - polygon[i].x) *
					(test_y - polygon[i].y) /
					(polygon[j].y - polygon[i].y) +
					polygon[i].x)) {
				inside_polygon = !inside_polygon;
			}
		}
		inside_polygon ? ((*mask_buffer)[k] = 255) : ((*mask_buffer)[k] = 0);
	}

	return MEDIA_VISION_ERROR_NONE;
}
