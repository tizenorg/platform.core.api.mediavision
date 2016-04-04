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

#ifndef __MEDIA_VISION_EVENT_TRIGGER_H__
#define __MEDIA_VISION_EVENT_TRIGGER_H__

/**
 * @file  EventTrigger.h
 * @brief This file contains interface for event trigger.
 */

#include "EventDefs.h"

#include <mv_surveillance.h>
#include <mv_surveillance_private.h>

#include <string>
#include <map>
#include <list>

namespace mediavision {
namespace surveillance {

/**
 * @class    EventTrigger
 * @brief    This class contains event trigger interface.
 *
 * @since_tizen 3.0
 */
class EXPORT_API EventTrigger {
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
	EventTrigger(
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
	virtual ~EventTrigger();

	/**
	 * @brief Parses engine configuration.
	 *
	 * @since_tizen 3.0
	 * @param [in] engineConfig    The engine configuration to be parsed
	 * @return @c 0 on success, otherwise a negative error value
	 */
	virtual int parseEngineConfig(mv_engine_config_h engineConfig) = 0;

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
					const cv::Mat& grayImage) = 0;

	/**
	 * @brief Gets event type.
	 *
	 * @since_tizen 3.0
	 * @return string with event type
	 */
	virtual std::string getEventType() const = 0;

	/**
	 * @brief Gets video stream identifier of event trigger.
	 *
	 * @since_tizen 3.0
	 * @return video stream identifier
	 */
	int getVideoStreamId() const;

	/**
	 * @brief Checks if callback with the identifier is subscribed.
	 *
	 * @since_tizen 3.0
	 * @return true if suscribed, false otherwise
	 */
	bool isCallbackSubscribed(long int triggerId) const;

	/**
	 * @brief Subscibes callback with unique identifier.
	 *
	 * @since_tizen 3.0
	 * @param [in] eventTrigger      The event trigger to be register (NULL if internal)
	 * @param [in] triggerId         Unique event trigger identifier to be subscribed
	 * @param [in] callback          The callback to be called if event will be occured
	 * @param [in] user_data         The user data to be passed to the callback function
	 * @param [in] numberOfPoints    The number of ROI points
	 * @param [in] roi               The intput array with ROI points
	 * @return @c true on success, false otherwise
	 */
	bool subscribeCallback(
			mv_surveillance_event_trigger_h eventTrigger,
			long int triggerId,
			mv_surveillance_event_occurred_cb callback,
			void *userData,
			int numberOfPoints,
			mv_point_s *roi);

	/**
	 * @brief Unsubscibes callback with unique identifier.
	 *
	 * @since_tizen 3.0
	 * @param [in] triggerId    Unique event trigger identifier to be unsubscribed
	 * @return @c true on success, false otherwise
	 */
	bool unsubscribeCallback(long int triggerId);

	/**
	 * @brief Checks if there are no subscribed callbacks.
	 *
	 * @since_tizen 3.0
	 * @return @c true at least one callback is subscribed, false otherwise
	 */
	bool isCallbacksEmpty() const;

	/**
	 * @brief Applies ROI (Region Of Interest) to input image.
	 *
	 * @since_tizen 3.0
	 * @param [in, out] image     The input image where ROI will be applied
	 * @param [in] imageWidth     The input image width
	 * @param [in] imageHeight    The input image height
	 * @param [in] scalePoints    True if ROI points must be scaled, false oterwise
	 * @param [in] scaleX         The scale for X ROI point coordinate
	 * @param [in] scaleY         The scale for Y ROI point coordinate
	 * @return @c true on success, false otherwise
	 */
	int applyROIToImage(
			unsigned char *image,
			int imageWidth,
			int imageHeight,
			bool scalePoints = false,
			int scaleX = 1,
			int scaleY = 1);

	/**
	 * @brief Comparison operator for equal case.
	 *
	 * @since_tizen 3.0
	 * @return true if event trigger is equal to other, false otherwise
	 */
	virtual bool operator==(const EventTrigger& other) const;

	/**
	 * @brief Comparison operator for not equal case.
	 *
	 * @since_tizen 3.0
	 * @return true if event trigger is not equal to other, false otherwise
	 */
	virtual bool operator!=(const EventTrigger& other) const;

protected:
	struct CallbackData {
		mv_surveillance_event_trigger_h eventTrigger;

		mv_surveillance_event_occurred_cb callback;

		void *userData;
	};

	typedef std::map<long int, CallbackData> CallbackDataMap;
	typedef CallbackDataMap::const_iterator CallbackDataMapConstIter;
	typedef CallbackDataMap::iterator CallbackDataMapIter;

	typedef std::pair<long int, CallbackData> CallbackDataPair;

protected:
	static long int InternalTriggersCounter;

protected:
	int __videoStreamId;

	MVPoints __roi;

	CallbackDataMap __callbackDataMap;
};

typedef std::list<EventTrigger*> EventTriggers;
typedef std::map<int, EventTriggers> EventTriggersMap;
typedef EventTriggers::const_iterator EventTriggersConstIter;
typedef EventTriggers::iterator EventTriggersIter;

} /* surveillance */
} /* mediavision */

#endif /* __MEDIA_VISION_EVENT_TRIGGER_H__ */
