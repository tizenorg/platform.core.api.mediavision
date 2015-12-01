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

#ifndef __MEDIA_VISION_EVENT_TRIGGER_MOVEMENT_DETECTION_H__
#define __MEDIA_VISION_EVENT_TRIGGER_MOVEMENT_DETECTION_H__

/**
 * @file  EventTriggerMovementDetection.h
 * @brief This file contains interface for movement detection events.
 */

#include "EventTrigger.h"

#include "EventResult.h"
#include "EventDefs.h"

#include <opencv/cv.h>

namespace mediavision {
namespace surveillance {

/**
 * @class EventResultMovementDetection
 * @brief This class contains movement detection event results.
 *
 * @since_tizen 3.0
 */
class EventResultMovementDetection : public EventResult {
public:
	/**
	 * @brief Gets result value.
	 *
	 * @since_tizen 3.0
	 * @param [in] valueName     The name of the value to be gotten
	 * @param [in, out] value    The pointer to variable which will be filled
	 *                           by result value
	 * @return @c 0 on success, otherwise a negative error value
	 */
	virtual int getResultValue(const char *valueName, void *value) const;

public:
	MVRectangles __movementRegions; /**< Regions where movements were detected */

	cv::Mat __grayImage; /** Current gray image (only for internal usage) */
};

/**
 * @class EventTriggerMovementDetection
 * @brief This class contains movement detection events.
 *
 * @since_tizen 3.0
 */
class EventTriggerMovementDetection : public EventTrigger {
public:
	/**
	 * @brief Default constructor.
	 *
	 * @since_tizen 3.0
	 * @param [in] eventTrigger      The event trigger to be register (NULL if internal)
	 * @param [in] triggerId         Unique event trigger identifier to be register
	 * @param [in] videoStreamId     Video stream identifier
	 * @param [in] callback          The callback to be called if event will be occured
	 * @param [in] user_data         The user data to be passed to the callback function
	 * @param [in] numberOfPoints    The number of ROI points
	 * @param [in] roi               The intput array with ROI points
	 */
	EventTriggerMovementDetection(
		mv_surveillance_event_trigger_h eventTrigger,
		long int triggerId,
		int videoStreamId,
		mv_surveillance_event_occurred_cb callback,
		void *userData,
		int numberOfPoints,
		mv_point_s *roi);

	/**
	 * @brief Default destructor.
	 *
	 * @since_tizen 3.0
	 */
	virtual ~EventTriggerMovementDetection();

	/**
	 * @brief Parses engine configuration.
	 *
	 * @since_tizen 3.0
	 * @param [in] engineConfig    The engine configuration to be parsed
	 * @return @c 0 on success, otherwise a negative error value
	 */
	virtual int parseEngineConfig(mv_engine_config_h engineConfig);

	/**
	 * @brief Pushes media source.
	 *
	 * @since_tizen 3.0
	 * @param [in] source        The media source to be parsed
	 * @param [in] graySource    The media source converted to gray scale
	 * @param [in] grayImage     The converted to gray scale source
	 * @return @c 0 on success, otherwise a negative error value
	 */
	virtual int pushSource(
					mv_source_h source,
					mv_source_h graySource,
					const cv::Mat& grayImage);

	/**
	 * @brief Gets event type.
	 *
	 * @since_tizen 3.0
	 * @return string with event type
	 */
	virtual std::string getEventType() const;

	/**
	 * @brief Comparison operator for equal case.
	 *
	 * @since_tizen 3.0
	 * @return true if event trigger is equal to other, false otherwise
	 */
	virtual bool operator==(const EventTriggerMovementDetection& other) const;

	/**
	 * @brief Comparison operator for not equal case.
	 *
	 * @since_tizen 3.0
	 * @return true if event trigger is not equal to other, false otherwise
	 */
	virtual bool operator!=(const EventTriggerMovementDetection& other) const;

private:
	static const cv::Mat __ERODE_KERNEL;

	static const cv::Mat __DILATE_KERNEL;

private:
	cv::Mat __previousImage;

	EventResultMovementDetection *__eventResult;

	int __diffThreshold;
};

} /* surveillance */
} /* mediavision */

#endif /* __MEDIA_VISION_EVENT_TRIGGER_MOVEMENT_DETECTION_H__ */
