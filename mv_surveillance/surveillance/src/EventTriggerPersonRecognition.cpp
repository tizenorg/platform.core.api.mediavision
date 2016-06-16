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

#include "EventTriggerPersonRecognition.h"
#include "EventManager.h"

#include <mv_private.h>
#include <mv_face.h>
#include <mv_surveillance.h>

#include <sstream>

namespace mediavision {
namespace surveillance {

static const int MAX_VALUE_NAME_LENGHT = 255;

namespace {

template <typename T>
std::string numberToString(T Number)
{
	std::ostringstream ss;
	ss << Number;
	return ss.str();
}

} /* Anonymous namespace*/

int EventResultPersonRecognition::getResultValue(
	const char *valueName,
	void *value) const
{
	if (valueName == NULL) {
		LOGE("Invalid pointer for value name. Getting result value failed.");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	if (value == NULL) {
		LOGE("Invalid pointer for value. Getting result value failed.");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	const size_t numberOfPersons = __locations.size();

	if (strncmp(valueName,
				MV_SURVEILLANCE_PERSONS_RECOGNIZED_NUMBER,
				MAX_VALUE_NAME_LENGHT) == 0) {
		size_t *outNumberOfPersons = (size_t*) value;
		*outNumberOfPersons = numberOfPersons;
	} else if (strncmp(valueName,
						MV_SURVEILLANCE_PERSONS_RECOGNIZED_LOCATIONS,
						MAX_VALUE_NAME_LENGHT) == 0) {
		mv_rectangle_s *locations = (mv_rectangle_s*) value;

		for (size_t i = 0; i < numberOfPersons; ++i)
			locations[i] = __locations[i];
	} else if (strncmp(valueName,
						MV_SURVEILLANCE_PERSONS_RECOGNIZED_LABELS,
						MAX_VALUE_NAME_LENGHT) == 0) {
		int *labels = (int*) value;

		for (size_t i = 0; i < numberOfPersons; ++i)
			labels[i] = __faceLabels[i];
	} else if (strncmp(valueName,
						MV_SURVEILLANCE_PERSONS_RECOGNIZED_CONFIDENCES,
						MAX_VALUE_NAME_LENGHT) == 0) {
		double *confidences = (double*) value;

		for (size_t i = 0; i < numberOfPersons; ++i)
			confidences[i] = __confidences[i];
	} else {
		LOGE("This value name doesn't exist. Getting result value failed.");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	return MEDIA_VISION_ERROR_NONE;
}

EventTriggerPersonRecognition::EventTriggerPersonRecognition(
		mv_surveillance_event_trigger_h eventTrigger,
		long int triggerId,
		int videoStreamId,
		mv_surveillance_event_occurred_cb callback,
		void *userData,
		int numberOfPoints,
		mv_point_s *roi) : EventTrigger(eventTrigger,
										triggerId,
										videoStreamId,
										callback,
										userData,
										numberOfPoints,
										roi),
	__faceRecognitionModel(NULL),
	__lastFrame(NULL),
	__eventResult(new EventResultPersonRecognition())
{
	; /* NULL */
}

EventTriggerPersonRecognition::~EventTriggerPersonRecognition()
{
	if (NULL != __faceRecognitionModel) {
		const int err = mv_face_recognition_model_destroy(__faceRecognitionModel);
		if (MEDIA_VISION_ERROR_NONE != err)
			LOGE("Error while trying to delete face recognition model when "
					"event trigger had been destroyed. Error code: %i.", err);
	}

	delete __eventResult;
}

int EventTriggerPersonRecognition::parseEngineConfig(mv_engine_config_h engineConfig)
{
	if (engineConfig == NULL) {
		LOGE("Engine configuration is NULL. Parsing failed.");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	char *modelPath = NULL;

	int error = mv_engine_config_get_string_attribute(
			engineConfig,
			MV_SURVEILLANCE_FACE_RECOGNITION_MODEL_FILE_PATH,
			&modelPath);

	if (error != MEDIA_VISION_ERROR_NONE) {
		if (modelPath != NULL)
			delete[] modelPath;

		LOGE("Getting recognition model from engine configuration failed.");

		return error;
	}

	mv_face_recognition_model_h recognitionModel = NULL;

	error = mv_face_recognition_model_load(modelPath, &recognitionModel);

	if (error != MEDIA_VISION_ERROR_NONE) {
		LOGE("Loading recognition model from file %s failed.",
				modelPath);

		if (modelPath != NULL)
			delete[] modelPath;

		return error;
	}

	if (NULL != __faceRecognitionModel) {
		error = mv_face_recognition_model_destroy(__faceRecognitionModel);
		if (MEDIA_VISION_ERROR_NONE != error) {
			LOGE("Error while trying to delete old face recognition model when "
					"new model is trying to be loaded. Error code: %i.", error);
		}
	}

	__faceRecognitionModel = recognitionModel;

	if (NULL == __faceRecognitionModel) {
		LOGE("Failed to load face recognition model. Check %s attribute of the "
				"engine config.", MV_SURVEILLANCE_FACE_RECOGNITION_MODEL_FILE_PATH);

		if (modelPath != NULL) {
			free(modelPath);
			modelPath = NULL;
		}

		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	if (modelPath != NULL) {
		free(modelPath);
		modelPath = NULL;
	}

	return MEDIA_VISION_ERROR_NONE;
}

int EventTriggerPersonRecognition::pushSource(
		mv_source_h source,
		mv_source_h graySource,
		const cv::Mat& grayImage)
{
	if (source == NULL || graySource == NULL || grayImage.empty()) {
		LOGE("Media source is NULL. Pushing source failed.");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	__lastFrame = source;

	__eventResult->__locations.clear();
	__eventResult->__faceLabels.clear();
	__eventResult->__confidences.clear();

	unsigned char *data_buffer = NULL;
	unsigned int buffer_size = 0;
	mv_colorspace_e colorspace = MEDIA_VISION_COLORSPACE_INVALID;
	unsigned int width = 0;
	unsigned int height = 0;

	int error = mv_source_get_buffer(graySource, &data_buffer, &buffer_size);

	if (error != MEDIA_VISION_ERROR_NONE) {
		LOGE("Operation with media source failed with error %d.", error);
		return error;
	}

	error = mv_source_get_colorspace(graySource, &colorspace);

	if (error != MEDIA_VISION_ERROR_NONE) {
		LOGE("Operation with media source failed with error %d.", error);
		return error;
	}

	error = mv_source_get_width(graySource, &width);

	if (error != MEDIA_VISION_ERROR_NONE) {
		LOGE("Operation with media source failed with error %d.", error);
		return error;
	}

	error = mv_source_get_height(graySource, &height);

	if (error != MEDIA_VISION_ERROR_NONE) {
		LOGE("Operation with media source failed with error %d.", error);
		return error;
	}

	if (buffer_size != width * height) {
		/* Unexcepted behaviour */
		LOGE("Grayscale source interpretation failed.");
		return MEDIA_VISION_ERROR_INTERNAL;
	}

	mv_source_h sourceCopy = NULL;

	error = mv_create_source(&sourceCopy);

	if (error != MEDIA_VISION_ERROR_NONE) {
		LOGE("Operation with media source failed with error %d.", error);
		return error;
	}

	error = mv_source_fill_by_buffer(sourceCopy, data_buffer, buffer_size,
			width, height, colorspace);

	if (error != MEDIA_VISION_ERROR_NONE) {
		mv_destroy_source(sourceCopy);
		LOGE("Operation with media source failed with error %d.", error);
		return error;
	}

	error = mv_source_get_buffer(sourceCopy, &data_buffer, &buffer_size);

	if (error != MEDIA_VISION_ERROR_NONE) {
		mv_destroy_source(sourceCopy);
		LOGE("Operation with media source failed with error %d.", error);
		return error;
	}

	error = applyROIToImage(data_buffer, width, height);

	if (error != MEDIA_VISION_ERROR_NONE) {
		mv_destroy_source(sourceCopy);
		LOGE("Applying ROI failed with error %d.", error);
		return error;
	}

	error = mv_face_detect(sourceCopy, NULL, faceDetectedCB, this);

	if (MEDIA_VISION_ERROR_NONE != error) {
		mv_destroy_source(sourceCopy);
		LOGE("Errors were occurred during face detecting %i", error);
		return error;
	}

	error = mv_destroy_source(sourceCopy);

	if (error != MEDIA_VISION_ERROR_NONE) {
		LOGE("Operation with media source failed with error %d.", error);
		return error;
	}

	return error;
}

std::string EventTriggerPersonRecognition::getEventType() const
{
	return MV_SURVEILLANCE_EVENT_TYPE_PERSON_RECOGNIZED;
}

bool EventTriggerPersonRecognition::operator==(const EventTriggerPersonRecognition& other) const
{
	if (EventTrigger::operator !=(other))
		return false;

	/* TODO: compare private values if necessary */

	return true;
}

bool EventTriggerPersonRecognition::operator!=(const EventTriggerPersonRecognition& other) const
{
	return !(*this == other);
}

void EventTriggerPersonRecognition::setEventResults(
		mv_rectangle_s faceLocation,
		int faceLabel,
		double confidence)
{
	__eventResult->__locations.push_back(faceLocation);
	__eventResult->__faceLabels.push_back(faceLabel);
	__eventResult->__confidences.push_back(confidence);
}

void EventTriggerPersonRecognition::faceDetectedCB(
		mv_source_h source,
		mv_engine_config_h /*engine_cfg*/,
		mv_rectangle_s *faces_locations,
		int number_of_faces,
		void *user_data)
{
	if (NULL == user_data) {
		LOGE("Invalid user data passed");
		return;
	}

	EventTriggerPersonRecognition *recognitionTrigger =
		(EventTriggerPersonRecognition*)user_data;

	int location_idx = 0;
	for (; location_idx < number_of_faces; ++location_idx) {
		LOGI("Start surveillance face recognition");

		const int error = mv_face_recognize(
				source,
				recognitionTrigger->__faceRecognitionModel,
				NULL,
				&faces_locations[location_idx],
				faceRecognizedCB,
				recognitionTrigger);

		if (error != MEDIA_VISION_ERROR_NONE) {
			LOGW("Face recognition for one model failed. Continue");
			continue;
		}

		LOGI("Face has been successfully recognized");
	}
}

void EventTriggerPersonRecognition::faceRecognizedCB(
		mv_source_h source,
		mv_face_recognition_model_h /*recognition_model*/,
		mv_engine_config_h /*engine_cfg*/,
		mv_rectangle_s *face_location,
		const int *face_label,
		double confidence,
		void *user_data)
{
	if (face_location == NULL || face_label == NULL) {
		LOGI("Face wasn't recognized");
		return;
	}

	EventTriggerPersonRecognition *trigger =
		(EventTriggerPersonRecognition*) user_data;

	trigger->setEventResults(*face_location, *face_label, confidence);

	CallbackDataMapConstIter iter = trigger->__callbackDataMap.begin();

	for (; iter != trigger->__callbackDataMap.end(); ++iter) {
		mv_surveillance_event_occurred_cb callback = iter->second.callback;
		callback(
			iter->second.eventTrigger,
			trigger->__lastFrame,
			trigger->__videoStreamId,
			trigger->__eventResult,
			iter->second.userData);
	}
}

} /* surveillance */
} /* mediavision */
