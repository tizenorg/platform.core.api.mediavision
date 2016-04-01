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

#ifndef __TIZEN_MEDIAVISION_SURVEILLANCE_PRIVATE_H__
#define __TIZEN_MEDIAVISION_SURVEILLANCE_PRIVATE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "mv_common.h"

/**
 * @brief Event trigger structure.
 * @details See supported event types and their descriptions in
 *          @ref CAPI_MEDIA_VISION_SURVEILLANCE_EVENT_TYPES documentation
 *          section.
 *          Also the list of supported events can be obtained using
 *          @ref mv_surveillance_foreach_supported_event_type() function
 *
 * @since_tizen 3.0
 */
typedef struct {
	unsigned int trigger_id;     /**< Unique event trigger identifier */
	const char *event_type;      /**< Type of the event */
	int number_of_roi_points;    /**< Number of ROI (Region of interest) points */
	mv_point_s *roi;             /**< ROI points array */
} mv_surveillance_event_trigger_s;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TIZEN_MEDIAVISION_SURVEILLANCE_PRIVATE_H__ */
