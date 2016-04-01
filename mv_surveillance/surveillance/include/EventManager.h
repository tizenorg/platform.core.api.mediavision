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

#ifndef __MEDIA_VISION_EVENT_MANAGER_H__
#define __MEDIA_VISION_EVENT_MANAGER_H__

/**
 * @file  EventManager.h
 * @brief This file contains functionality for event manager.
 */

#include "EventTrigger.h"
#include "EventDefs.h"

namespace mediavision {
namespace surveillance {

class EventManager;

/**
 * @class    EventManagerDestroyer
 * @brief    This class contains event manager destroyer functionality.
 *
 * @since_tizen 3.0
 */
class EventManagerDestroyer {
public:
	/**
	 * @brief Default destructor.
	 *
	 * @since_tizen 3.0
	 */
	~EventManagerDestroyer();

	/**
	 * @brief Initializes pointer to EventManager which will be destroyed.
	 *
	 * @since_tizen 3.0
	 * @param [in] pointer    The pointer to EventManager which will be destroyed
	 */
	void initialize(EventManager *pointer);

private:
	EventManager *__pInstance;
};

/**
 * @class    EventManager
 * @brief    This class contains event manager functionality.
 *
 * @since_tizen 3.0
 */

class EventManager {
public:
	/**
	 * @brief Gets EventManager instance.
	 *
	 * @since_tizen 3.0
	 */
	static EventManager& getInstance();

	/**
	 * @brief Registers event.
	 *
	 * @since_tizen 3.0
	 * @param [in] eventTrigger     The event trigger to be register (NULL if internal)
	 * @param [in] triggerId        Unique event trigger identifier to be register
	 * @param [in] eventType        Type of the event
	 * @param [in] videoStreamId    Video stream identificator
	 * @param [in] engineCfg        The engine configuration for event trigger
	 * @param [in] callback         The callback to be called if event will be occured
	 * @param [in] user_data        The user data to be passed to the callback function
	 * @param [in] numberOfPoints    The number of ROI points
	 * @param [in] roi               The intput array with ROI points
	 * @return @c 0 on success, otherwise a negative error value
	 */
	int registerEvent(
		mv_surveillance_event_trigger_h eventTrigger,
		long int triggerId,
		const char *eventType,
		int videoStreamId,
		mv_engine_config_h engineCfg,
		mv_surveillance_event_occurred_cb callback,
		void *user_data,
		int numberOfPoints,
		mv_point_s *roi);

    /**
     * @brief Unregisters event.
     *
     * @since_tizen 3.0
     * @param [in] triggerId       Unique event trigger identifier to be
     *                             unregister
     * @param [in] videoStreamId   Video stream identifier for which event
     *                             will be unregistered
     * @return @c 0 on success, otherwise a negative error value
     */
    int unregisterEvent(long int triggerId, int videoStreamId);

	/**
	 * @brief Pushes media source to run event triggers.
	 *
	 * @since_tizen 3.0
	 * @param [in] source           The media source to be pushed
	 * @param [in] videoStreamId    The video stream identificator for media source
	 * @return @c 0 on success, otherwise a negative error value
	 */
	int pushSource(mv_source_h source, int videoStreamId);

	/**
	 * @brief Gets supported event types.
	 *
	 * @since_tizen 3.0
	 * @param [out] eventTypes    The supported event types
	 * @return @c 0 on success, otherwise a negative error value
	 */
	static int getSupportedEventTypes(StringVector& eventTypes);

	/**
	 * @brief Gets all supported event result value names.
	 *
	 * @since_tizen 3.0
	 * @param [out] eventResValNames    The supported event result value names
	 * @return @c 0 on success, otherwise a negative error value
	 */
	static int getSupportedEventResultValueNames(StringVector& eventResValNames);

	/**
	 * @brief Gets supported event result value names for an event type.
	 *
	 * @since_tizen 3.0
	 * @param [in]  eventTypeName       The name of the event type to return
	 *                                  result value names for
	 * @param [out] eventResValNames    The supported event result value names
	 * @return @c 0 on success, otherwise a negative error value
	 */
	static int getSupportedEventResultValueNames(
		const std::string& eventTypeName,
		StringVector& eventResValNames);

private:
	EventManager();

	EventManager(const EventManager&);

	EventManager& operator=(EventManager&);

	~EventManager();

	static void setSupportedEventTypes();

    EventTriggersIter isTriggerExists(EventTrigger *trigger, int videoStreamId);

	friend class EventManagerDestroyer;

private:
	static EventManager *__pInstance;

	static EventManagerDestroyer Destroyer;

	static EventTypesMap SupportedEventTypes;

private:
	EventTriggersMap __eventTriggers;
};

} /* surveillance */
} /* mediavision */

#endif /* __MEDIA_VISION_EVENT_MANAGER_H__ */
