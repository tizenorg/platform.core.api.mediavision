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

#ifndef __MEDIA_VISION_EVENT_TRIGGER_PERSON_RECOGNITION_H__
#define __MEDIA_VISION_EVENT_TRIGGER_PERSON_RECOGNITION_H__

/**
 * @file  EventTriggerPersonRecognition.h
 * @brief This file contains interface for person recognized events.
 */

#include <mv_face.h>

#include "EventTrigger.h"

#include "EventResult.h"
#include "EventDefs.h"

#include "EventTriggerPersonAppearance.h"

namespace mediavision {
namespace surveillance {

/**
 * @class EventResultPersonRecogniton
 * @brief This class contains person recognized event results.
 *
 * @since_tizen 3.0
 */
class EXPORT_API EventResultPersonRecognition : public EventResult {
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
	MVRectangles __locations; /**< Persons locations */

	IntVector __faceLabels; /**< Persons face lables */

	DoubleVector __confidences; /**< Persons face recognition confidences */
};

/**
 * @class EventTriggerPersonRecognition
 * @brief This class contains person recognized events.
 *
 * @since_tizen 3.0
 */
class EXPORT_API EventTriggerPersonRecognition : public EventTrigger {
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
	EventTriggerPersonRecognition(
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
	virtual ~EventTriggerPersonRecognition();

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
	virtual bool operator==(const EventTriggerPersonRecognition& other) const;

	/**
	 * @brief Comparison operator for not equal case.
	 *
	 * @since_tizen 3.0
	 * @return true if event trigger is not equal to other, false otherwise
	 */
	virtual bool operator!=(const EventTriggerPersonRecognition& other) const;

	/**
	 * @brief Sets event results.
	 *
	 * @since_tizen 3.0
	 * @param [in] faceLocation    The location of the face recognized on @a source.
	 * @param [in] faceLabel       The label that identifies face which was
	 *                             recognized in the @a source.
	 * @param [in] confidence      The confidence of the @a recognition_model
	 *                             that face has been recognized correctly
	 *                             (value from 0.0 to 1.0).
	 * @return @c 0 on success, otherwise a negative error value
	 */
	void setEventResults(
		mv_rectangle_s faceLocation,
		int faceLabel,
		double confidence);

private:
	mv_face_recognition_model_h __faceRecognitionModel;

	mv_source_h __lastFrame;

	EventResultPersonRecognition *__eventResult;

private:
	static void faceDetectedCB(
					mv_source_h source,
					mv_engine_config_h engine_cfg,
					mv_rectangle_s *faces_locations,
					int number_of_faces,
					void *user_data);

	static void faceRecognizedCB(
					mv_source_h source,
					mv_face_recognition_model_h recognition_model,
					mv_engine_config_h engine_cfg,
					mv_rectangle_s *face_location,
					const int *face_label,
					double confidence,
					void *user_data);
};

} /* surveillance */
} /* mediaVision */

#endif /* __MEDIA_VISION_EVENT_TRIGGER_PERSON_RECOGNITION_H__ */
