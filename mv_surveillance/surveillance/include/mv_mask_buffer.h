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

#ifndef __MEDIA_VISION_MV_MASK_BUFFER_H__
#define __MEDIA_VISION_MV_MASK_BUFFER_H__

#include "mv_common.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Gets mask buffer from buffer with known size.
 * @details Mask buffer values: 0 ouside polygon and 255 inside polygon.
 *
 * @since_tizen 3.0
 * @param [in] buffer_width     The buffer width
 * @param [in] buffer_height    The buffer height
 * @param [in] polygon          The array with polygon
 * @param [in] points_number    The size of array with polygon
 * @param [out] mask_buffer     The output mask buffer. mask_buffer size is
 *                              the same as the buffer size in media source
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @post Free memory for mask_buffer.
 */
int mv_get_mask_buffer(
		unsigned int buffer_width,
		unsigned int buffer_height,
		mv_point_s *polygon,
		unsigned int points_number,
		unsigned char **mask_buffer);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MEDIA_VISION_MV_MASK_BUFFER_H__ */

