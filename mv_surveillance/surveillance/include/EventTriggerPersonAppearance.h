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

#ifndef __MEDIA_VISION_EVENT_TRIGGER_PERSON_APPEARANCE_H__
#define __MEDIA_VISION_EVENT_TRIGGER_PERSON_APPEARANCE_H__

/**
 * @file  EventTriggerPersonAppearance.h
 * @brief This file contains interface for person appeared / disapeared events.
 */

#include "EventTrigger.h"

#include "EventResult.h"
#include "EventDefs.h"
#include "HoGDetector.h"

#include <opencv/cv.h>

namespace mediavision {
namespace surveillance {

/**
 * @class EventResultPersonAppearance
 * @brief This class contains person appeared / disapeared event results.
 *
 * @since_tizen 3.0
 */
class EventResultPersonAppearance : public EventResult {
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
	MVRectangles __appearedLocations; /**< Locations of persons which were appeared
                                         first time*/

	MVRectangles __trackedLocations; /**< Locations of persons which were tracked
                                        from previous frame*/

	MVRectangles __disappearedLocations; /**< Locations of persons which were
                                            disappeared */
};

/**
 * @class EventTriggerPersonAppearance
 * @brief This class contains person appeared / disapeared events.
 *
 * @since_tizen 3.0
 */
class EventTriggerPersonAppearance : public EventTrigger {
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
	EventTriggerPersonAppearance(
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
	virtual ~EventTriggerPersonAppearance();

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
	virtual bool operator==(const EventTriggerPersonAppearance& other) const;

	/**
	 * @brief Comparison operator for not equal case.
	 *
	 * @since_tizen 3.0
	 * @return true if event trigger is not equal to other, false otherwise
	 */
	virtual bool operator!=(const EventTriggerPersonAppearance& other) const;

private:
	static void movementDetectedCB(
		mv_surveillance_event_trigger_h event_trigger,
		mv_source_h source,
		int video_stream_id,
		mv_surveillance_result_h event_result,
		void *user_data);

private:

	void runCallbacks(mv_source_h source);

private:

	struct TrackedRectangle {
		cv::Rect rect;

		int framesCount;

		TrackedRectangle(cv::Rect _rect, int _framesCount)
		{
			rect = _rect;
			framesCount = _framesCount;
		}
	};

	typedef std::list<TrackedRectangle> TrackedRectangles;
	typedef TrackedRectangles::const_iterator TrackedRectanglesConstIter;
	typedef TrackedRectangles::iterator TrackedRectanglesIter;

private:
	int __skipFramesCount;

	int __frameCounter; /**< Counts frames on which detection has not be launched */

	long int __movementDetectedEventId;

	float __factorX;

	float __factorY;

	cv::Rect __rectToDetect;

	cv::Rect __rectToDetectPrevious;

	TrackedRectangles __trackedRects;

	CVRectangles __appearedRects;

	CVRectangles __disappearedRects;

	modifiedcv::HOGDescriptor __hogClassifier; /**< Classifier to be used for full body
												person detection */

	MVRectangles __detectedLocations;

	EventResultPersonAppearance *__eventResult;
};

} /* surveillance */
} /* mediavision */

#endif /* __MEDIA_VISION_EVENT_TRIGGER_PERSON_APPEARANCE_H__ */
