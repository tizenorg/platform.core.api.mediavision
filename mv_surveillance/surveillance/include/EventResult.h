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

#ifndef __MEDIA_VISION_EVENT_RESULT_H__
#define __MEDIA_VISION_EVENT_RESULT_H__

/**
 * @file  EventTrigger.h
 * @brief This file contains interface for event trigger.
 */

namespace mediavision {
namespace surveillance {

/**
 * @class    EventResult
 * @brief    This class contains event result interface.
 *
 * @since_tizen 3.0
 */
class EXPORT_API EventResult {
public:
	/**
	 * @brief Default destructor.
	 *
	 * @since_tizen 3.0
	 */
	virtual ~EventResult() {}

	/**
	 * @brief Gets result value.
	 *
	 * @since_tizen 3.0
	 * @param [in] valueName     The name of the value to be gotten
	 * @param [in, out] value    The pointer to variable which will be filled
	 *                           by result value
	 * @return @c 0 on success, otherwise a negative error value
	 */
	virtual int getResultValue(const char *valueName, void *value) const = 0;
};


} /* surveillance */
} /* mediavision */

#endif /* _MEDIA_VISION__EVENT_RESULT_H__ */
