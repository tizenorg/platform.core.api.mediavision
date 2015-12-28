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

#include "mv_face_open.h"

#include "FaceDetector.h"
#include "FaceUtil.h"
#include "FaceRecognitionModel.h"
#include "FaceTrackingModel.h"
#include "FaceEyeCondition.h"
#include "FaceExpressionRecognizer.h"

#include "mv_private.h"

#include <vector>
#include <set>
#include <cstring>

using namespace ::MediaVision::Face;

static const RecognitionParams defaultRecognitionParams = RecognitionParams();

static void extractRecognitionParams(
		mv_engine_config_h engine_cfg,
		RecognitionParams& recognitionParams)
{
	mv_engine_config_h working_cfg = NULL;

	if (NULL == engine_cfg) {
		mv_create_engine_config(&working_cfg);
	} else {
		working_cfg = engine_cfg;
	}

	int algType = 0;
	mv_engine_config_get_int_attribute_c(
			working_cfg,
			"MV_FACE_RECOGNITION_MODEL_TYPE",
			&algType);

	if (0 < algType && 4 > algType) {
		recognitionParams.mRecognitionAlgType =
								(FaceRecognitionModelType)algType;
		} else {
			recognitionParams.mRecognitionAlgType =
								defaultRecognitionParams.mRecognitionAlgType;
		}

		if (NULL == engine_cfg) {
			mv_destroy_engine_config(working_cfg);
		}
}

inline void convertRectCV2MV(const cv::Rect& src, mv_rectangle_s& dst)
{
	dst.point.x = src.x;
	dst.point.y = src.y;
	dst.width = src.width;
	dst.height = src.height;
}

int mv_face_detect_open(
		mv_source_h source,
		mv_engine_config_h engine_cfg,
		mv_face_detected_cb detected_cb,
		void *user_data)
{
	cv::Mat image;

	int error = convertSourceMV2GrayCV(source, image);
	if (error != MEDIA_VISION_ERROR_NONE) {
		LOGE("Convertion mv_source_h to gray failed");
		return error;
	}

	char *haarcascadeFilepath;
	error = mv_engine_config_get_string_attribute_c(
					engine_cfg,
					"MV_FACE_DETECTION_MODEL_FILE_PATH",
					&haarcascadeFilepath);

	/* default path */
	std::string haarcascadeFilePathStr =
			"/usr/share/OpenCV/haarcascades/haarcascade_frontalface_alt2.xml";

	if (error == MEDIA_VISION_ERROR_NONE) {
		LOGI("Haarcascade file was set as default");
		haarcascadeFilePathStr = std::string(haarcascadeFilepath);

		delete[] haarcascadeFilepath;
	} else {
		LOGE("Error occurred during face detection haarcascade file receiving."
				" (%i)", error);
	}

	static FaceDetector faceDetector;

	if (!faceDetector.loadHaarcascade(haarcascadeFilePathStr)) {
		LOGE("Loading Haarcascade failed");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	cv::Rect roi(-1, -1, -1, -1);
	error = mv_engine_config_get_int_attribute_c(
				engine_cfg,
				MV_FACE_DETECTION_ROI_X,
				&roi.x);
	if (error != MEDIA_VISION_ERROR_NONE) {
		LOGE("Error occurred during face detection roi (x) receiving."
				" (%i)", error);
	}

	error = mv_engine_config_get_int_attribute_c(
				engine_cfg,
				MV_FACE_DETECTION_ROI_Y,
				&roi.y);
	if (error != MEDIA_VISION_ERROR_NONE) {
		LOGE("Error occurred during face detection roi (y) receiving."
				" (%i)", error);
}

	error = mv_engine_config_get_int_attribute_c(
				engine_cfg,
				MV_FACE_DETECTION_ROI_WIDTH,
				&roi.width);
	if (error != MEDIA_VISION_ERROR_NONE) {
		LOGE("Error occurred during face detection roi (width) receiving."
				" (%i)", error);
	}

	error = mv_engine_config_get_int_attribute_c(
				engine_cfg,
				MV_FACE_DETECTION_ROI_HEIGHT,
				&roi.height);
	if (error != MEDIA_VISION_ERROR_NONE) {
		LOGE("Error occurred during face detection roi (height) receiving."
				" (%i)", error);
	}

	cv::Size minSize(-1, -1);
	error = mv_engine_config_get_int_attribute_c(
				engine_cfg,
				MV_FACE_DETECTION_MIN_SIZE_WIDTH,
				&minSize.width);
	if (error != MEDIA_VISION_ERROR_NONE) {
		LOGE("Error occurred during face detection minimum width receiving."
				" (%i)", error);
	}

	error = mv_engine_config_get_int_attribute_c(
				engine_cfg,
				MV_FACE_DETECTION_MIN_SIZE_HEIGHT,
				&minSize.height);
	if (error != MEDIA_VISION_ERROR_NONE) {
		LOGE("Error occurred during face detection minimum height receiving."
				" (%i)", error);
	}

	std::vector<cv::Rect> faceLocations;
	if (!faceDetector.detectFaces(image, roi, minSize, faceLocations)) {
		LOGE("Face detection in OpenCV failed");
		return MEDIA_VISION_ERROR_INVALID_OPERATION;
	}

	static const int StartMaxResultsNumber = 50;
	static std::vector<mv_rectangle_s> results(StartMaxResultsNumber);

	const int numberOfResults = faceLocations.size();
	if (numberOfResults > StartMaxResultsNumber) {
		results.resize(numberOfResults);
	}

	for (int rectNum = 0; rectNum < numberOfResults; ++rectNum) {
		convertRectCV2MV(faceLocations[rectNum], results[rectNum]);
	}

	LOGI("Call the detect callback for %i detected faces", numberOfResults);
	detected_cb(source, engine_cfg, results.data(), numberOfResults, user_data);

	return MEDIA_VISION_ERROR_NONE;
}

int mv_face_recognize_open(
		mv_source_h source,
		mv_face_recognition_model_h recognition_model,
		mv_engine_config_h engine_cfg,
		mv_rectangle_s *face_location,
		mv_face_recognized_cb recognized_cb,
		void *user_data)
{
	if (!source) {
		LOGE("Can't recognize for the NULL Media Vision source handle");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	if (!recognized_cb) {
		LOGE("Recognition failed. Can't output recognition results without "
			"callback function");
			return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	if (!recognition_model) {
			LOGE("Can't recognize for the NULL Media Vision Face recognition model");
			return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	FaceRecognitionModel *pRecModel = static_cast<FaceRecognitionModel*>(recognition_model);

	if (!pRecModel) {
		LOGE("Face recognition failed. Incorrect Media Vision Face recognition model handle is used");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	cv::Mat grayImage;
	int ret = convertSourceMV2GrayCV(source, grayImage);

	if (MEDIA_VISION_ERROR_NONE != ret) {
		LOGE("Convertion mv_source_h to gray failed");
		return ret;
	}

	cv::Mat image;
	if (NULL == face_location) {
		image = grayImage;
	} else {
		cv::Rect_<int> roi;
		roi.x = face_location->point.x;
		roi.y = face_location->point.y;
		roi.width = face_location->width;
		roi.height = face_location->height;
		image = grayImage(roi);
	}

	FaceRecognitionResults results;

	LOGD("Face recognition is started");

	ret = pRecModel->recognize(image, results);

	if (MEDIA_VISION_ERROR_NONE != ret) {
		LOGE("Error occurred during the recognition. Failed");
		return ret;
	}

	if (!results.mIsRecognized) {
		recognized_cb(
		source,
		recognition_model,
		engine_cfg,
		NULL,
		NULL,
		0.0,
		user_data);
	} else {
		mv_rectangle_s location;
		location.point.x = results.mFaceLocation.x;
		location.point.y = results.mFaceLocation.y;
		location.width = results.mFaceLocation.width;
		location.height = results.mFaceLocation.height;

		if (face_location != NULL) {
			location.point.x += face_location->point.x;
			location.point.y += face_location->point.y;
		}

		recognized_cb(
				source,
				recognition_model,
				engine_cfg,
				&location,
				&(results.mFaceLabel),
				results.mConfidence,
				user_data);
	}

	LOGD("Face recognition is finished");

	return ret;
}

int mv_face_track_open(
		mv_source_h source,
		mv_face_tracking_model_h tracking_model,
		mv_engine_config_h engine_cfg,
		mv_face_tracked_cb tracked_cb,
		bool /*do_learn*/,
		void *user_data)
{
	if (!source) {
		LOGE("Can't track for the NULL Media Vision source handle");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	if (!tracked_cb) {
		LOGE("Tracking failed. Can't output tracking results without "
				"callback function");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	if (!tracking_model) {
		LOGE("Can't track for the NULL Media Vision Face tracking model");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	FaceTrackingModel *pTrackModel =
			static_cast<FaceTrackingModel*>(tracking_model);

	if (!pTrackModel) {
		LOGE("Face tracking failed. "
			"Incorrect Media Vision Face tracking model handle is used");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	cv::Mat grayImage;
	int ret = convertSourceMV2GrayCV(source, grayImage);

	if (MEDIA_VISION_ERROR_NONE != ret) {
		LOGE("Convertion mv_source_h to gray failed");
		return ret;
	}

	FaceTrackingResults results;
	ret = pTrackModel->track(grayImage, results);

	if (MEDIA_VISION_ERROR_NONE != ret) {
		LOGE("Tracking can't be performed. "
			"Check that tracking model is prepared when tracking starts");
		return ret;
	}

	if (results.mIsTracked) {
		mv_quadrangle_s predictedLocation;
		predictedLocation.points[0].x = results.mFaceLocation.x;
		predictedLocation.points[0].y = results.mFaceLocation.y;
		predictedLocation.points[1].x =
				results.mFaceLocation.x + results.mFaceLocation.width;
		predictedLocation.points[1].y = results.mFaceLocation.y;
		predictedLocation.points[2].x =
				results.mFaceLocation.x + results.mFaceLocation.width;
		predictedLocation.points[2].y =
				results.mFaceLocation.y + results.mFaceLocation.height;
		predictedLocation.points[3].x = results.mFaceLocation.x;
		predictedLocation.points[3].y =
				results.mFaceLocation.y + results.mFaceLocation.height;
		tracked_cb(
				source,
				tracking_model,
				engine_cfg,
				&predictedLocation,
				results.mConfidence,
				user_data);
	} else {
		tracked_cb(
				source,
				tracking_model,
				engine_cfg,
				NULL,
				results.mConfidence,
				user_data);
	}

	return ret;
}

int mv_face_eye_condition_recognize_open(
		mv_source_h source,
		mv_engine_config_h engine_cfg,
		mv_rectangle_s face_location,
		mv_face_eye_condition_recognized_cb eye_condition_recognized_cb,
		void *user_data)
{
	cv::Mat image;

	int error = convertSourceMV2GrayCV(source, image);
	if (error != MEDIA_VISION_ERROR_NONE) {
		LOGE("Convertion mv_source_h to gray failed");
		return error;
	}

	mv_face_eye_condition_e eye_condition;
	error = FaceEyeCondition::recognizeEyeCondition(
									image,
									face_location,
									&eye_condition);

	if (error != MEDIA_VISION_ERROR_NONE) {
		LOGE("eye contition recognition failed");
		return error;
	}

	eye_condition_recognized_cb(
				source,
				engine_cfg,
				face_location,
				eye_condition,
				user_data);

	return MEDIA_VISION_ERROR_NONE;
}

int mv_face_facial_expression_recognize_open(
		mv_source_h source,
		mv_engine_config_h engine_cfg,
		mv_rectangle_s face_location,
		mv_face_facial_expression_recognized_cb expression_recognized_cb,
		void *user_data)
{
	cv::Mat image;

	int error = convertSourceMV2GrayCV(source, image);
	if (error != MEDIA_VISION_ERROR_NONE) {
		LOGE("Convertion mv_source_h to gray failed");
		return error;
	}

	mv_face_facial_expression_e expression;
	error = FaceExpressionRecognizer::recognizeFaceExpression(
											image, face_location, &expression);

	if (error != MEDIA_VISION_ERROR_NONE) {
		LOGE("eye contition recognition failed");
		return error;
	}

	expression_recognized_cb(
				source,
				engine_cfg,
				face_location,
				expression,
				user_data);

	return MEDIA_VISION_ERROR_NONE;
}

int mv_face_recognition_model_create_open(
		mv_face_recognition_model_h *recognition_model)
{
	if (recognition_model == NULL) {
		LOGE("Recognition model can't be created because handle pointer is NULL");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	(*recognition_model) =
			static_cast<mv_face_recognition_model_h>(new (std::nothrow)FaceRecognitionModel());

	if (*recognition_model == NULL) {
		LOGE("Failed to create media vision recognition model");
		return MEDIA_VISION_ERROR_OUT_OF_MEMORY;
	}

	LOGD("Recognition model [%p] has been created", *recognition_model);

	return MEDIA_VISION_ERROR_NONE;
}

int mv_face_recognition_model_destroy_open(
		mv_face_recognition_model_h recognition_model)
{
	if (!recognition_model) {
		LOGE("Recognition model can't be destroyed because handle is NULL");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	LOGD("Destroying media vision recognition model [%p]", recognition_model);
	delete static_cast<FaceRecognitionModel*>(recognition_model);
	LOGD("Media vision recognition model has been destroyed");

	return MEDIA_VISION_ERROR_NONE;
}

int mv_face_recognition_model_clone_open(
		mv_face_recognition_model_h src,
		mv_face_recognition_model_h *dst)
{
	if (!src || !dst) {
		LOGE("Can't clone recognition model. Both source and destination"
			"recognition model handles has to be not NULL");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	(*dst) = static_cast<mv_face_recognition_model_h>(new (std::nothrow)FaceRecognitionModel());

	if (*dst == NULL) {
		LOGE("Failed to create media vision recognition model");
		return MEDIA_VISION_ERROR_OUT_OF_MEMORY;
	}

	LOGD("Recognition model [%p] has been created", *dst);

	const FaceRecognitionModel *pSrcModel = static_cast<FaceRecognitionModel*>(src);
	FaceRecognitionModel *pDstModel = static_cast<FaceRecognitionModel*>(*dst);

	*pDstModel = *pSrcModel;

	LOGD("Media vision recognition model has been cloned");
	return MEDIA_VISION_ERROR_NONE;
}

int mv_face_recognition_model_save_open(
		const char *file_name,
		mv_face_recognition_model_h recognition_model)
{
	if (!recognition_model) {
		LOGE("Can't save recognition model to the file. Handle has to be not NULL");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	if (NULL == file_name) {
		LOGE("Can't save recognition model to the file. File name has to be specified");
		return MEDIA_VISION_ERROR_INVALID_PATH;
	}

	FaceRecognitionModel *pRecModel = static_cast<FaceRecognitionModel*>(recognition_model);
	const int ret = pRecModel->save(std::string(file_name));

	if (MEDIA_VISION_ERROR_NONE != ret) {
		LOGE("Error occurred when save recognition model to the file");
		return ret;
	}

	LOGD("Media vision recognition model has been saved to the file [%s]", file_name);
	return ret;
}

int mv_face_recognition_model_load_open(
		const char *file_name,
		mv_face_recognition_model_h *recognition_model)
{
	if (!recognition_model) {
		LOGE("Can't load recognition model from the file. "
			"Handle has to be not NULL");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	if (NULL == file_name) {
		LOGE("Can't load recognition model from the file. "
			"File name has to be specified");
		return MEDIA_VISION_ERROR_INVALID_PATH;
	}

	(*recognition_model) =
		static_cast<mv_face_recognition_model_h>(new (std::nothrow)FaceRecognitionModel());

	if (*recognition_model == NULL) {
		LOGE("Failed to create media vision recognition model");
		return MEDIA_VISION_ERROR_OUT_OF_MEMORY;
	}

	FaceRecognitionModel *pRecModel =
			static_cast<FaceRecognitionModel*>(*recognition_model);

	if (!pRecModel) {
		LOGE("Loading of the face recognition model from file failed. "
			"Incorrect Media Vision Face recognition model handle is used");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	const int ret = pRecModel->load(std::string(file_name));

	if (MEDIA_VISION_ERROR_NONE != ret) {
		LOGE("Error occurred when loading recognition model to the file");
		return ret;
	}

	LOGD("Media vision recognition model has been loaded from the file [%s]", file_name);
	return ret;
}

int mv_face_recognition_model_add_open(
		const mv_source_h source,
		mv_face_recognition_model_h recognition_model,
		const mv_rectangle_s *example_location,
		int face_label)
{
	if (!source) {
		LOGE("Can't add face image example for recognition model. "
			"Media Vision source handle has to be not NULL");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	if (!recognition_model) {
		LOGE("Can't add face image example for recognition model. "
			"Model handle has to be not NULL");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	FaceRecognitionModel *pRecModel =
			static_cast<FaceRecognitionModel*>(recognition_model);

	if (!pRecModel) {
		LOGE("Add face image example to the model failed. "
			"Incorrect Media Vision Face recognition model handle is used");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	cv::Mat image;
	int ret = convertSourceMV2GrayCV(source, image);
	if (MEDIA_VISION_ERROR_NONE != ret) {
		LOGE("Convertion mv_source_h to gray failed");
		return ret;
	}

	if (!example_location) {
		ret = pRecModel->addFaceExample(image, face_label);
	} else {
		cv::Rect_<int> roi;
		roi.x = example_location->point.x;
		roi.y = example_location->point.y;
		roi.width = example_location->width;
		roi.height = example_location->height;
		ret = pRecModel->addFaceExample(image(roi).clone(), face_label);
	}

	if (MEDIA_VISION_ERROR_NONE != ret) {
		LOGE("Error occurred when adding face image example to the recognition model");
		return ret;
	}

	LOGD("The face image example labeled %i has been added "
		"to the Media Vision recognition model", face_label);
	return ret;
}

int mv_face_recognition_model_reset_open(
		mv_face_recognition_model_h recognition_model,
		const int *face_label)
{
	if (!recognition_model) {
		LOGE("Can't reset positive examples for NULL recognition model");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	FaceRecognitionModel *pRecModel =
			static_cast<FaceRecognitionModel*>(recognition_model);

	if (!pRecModel) {
		LOGE("Loading of the face recognition model from file failed. "
			"Incorrect Media Vision Face recognition model handle is used");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	int ret = (NULL != face_label ?
				pRecModel->resetFaceExamples(*face_label) :
				pRecModel->resetFaceExamples());

	if (MEDIA_VISION_ERROR_NONE != ret) {
		LOGE("Error occurred when reset positive examples of the recognition model");
		return ret;
	}

	LOGD("The positive examples has been removed from recognition model");
	return ret;
}

int mv_face_recognition_model_learn_open(
		mv_engine_config_h engine_cfg,
		mv_face_recognition_model_h recognition_model)
{
	if (!recognition_model) {
		LOGE("Can't learn recognition model. Model handle has to be not NULL");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	FaceRecognitionModel *pRecModel =
			static_cast<FaceRecognitionModel*>(recognition_model);

	if (!pRecModel) {
		LOGE("Learning of the face recognition model failed. "
		"Incorrect Media Vision Face recognition model handle is used");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	RecognitionParams recognitionParams;
	extractRecognitionParams(engine_cfg, recognitionParams);
	FaceRecognitionModelConfig learnConfig;
	learnConfig.mModelType = recognitionParams.mRecognitionAlgType;

	const int ret = pRecModel->learn(learnConfig);

	if (MEDIA_VISION_ERROR_NONE != ret) {
		LOGE("Error occurred when learn face recognition model");
		return ret;
	}

	LOGD("Face recognition model has been learned");
	return ret;
}

int mv_face_recognition_model_query_labels_open(
		mv_face_recognition_model_h recognition_model,
		int **labels,
		unsigned int *number_of_labels)
{
	if (!recognition_model) {
		LOGE("Can't get list of labels for NULL recognition model");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	if (NULL == labels || NULL == number_of_labels) {
		LOGE("Can't get list of labels. labels and number_of_labels out "
		"parameters both has to be not NULL.");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	FaceRecognitionModel *pRecModel =
				static_cast<FaceRecognitionModel*>(recognition_model);

	if (!pRecModel) {
		LOGE("Learning of the face recognition model failed. "
			"Incorrect Media Vision Face recognition model handle is used");
		 return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	const std::set<int>& learnedLabels = pRecModel->getFaceLabels();
	*number_of_labels = learnedLabels.size();
	(*labels) = (int*)malloc(sizeof(int) * (*number_of_labels));

	std::set<int>::const_iterator it = learnedLabels.begin();
	int i = 0;
	for (; it != learnedLabels.end(); ++it) {
		(*labels)[i] = *it;
		++i;
	}

	LOGD("List of the labels learned by the recognition model has been retrieved");
	return MEDIA_VISION_ERROR_NONE;
}

int mv_face_tracking_model_create_open(
		mv_face_tracking_model_h *tracking_model)
{
	if (tracking_model == NULL) {
		LOGE("Tracking model can't be created because handle pointer is NULL");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	(*tracking_model) =
			static_cast<mv_face_tracking_model_h>(new (std::nothrow)FaceTrackingModel());

	if (*tracking_model == NULL) {
		LOGE("Failed to create media vision tracking model");
		return MEDIA_VISION_ERROR_OUT_OF_MEMORY;
	}

	LOGD("Tracking model [%p] has been created", *tracking_model);

	return MEDIA_VISION_ERROR_NONE;
}

int mv_face_tracking_model_destroy_open(
		mv_face_tracking_model_h tracking_model)
{
	if (!tracking_model) {
		LOGE("Tracking model can't be destroyed because handle is NULL");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	LOGD("Destroying media vision tracking model [%p]", tracking_model);
	delete static_cast<FaceTrackingModel*>(tracking_model);
	LOGD("Media vision tracking model has been destroyed");

	return MEDIA_VISION_ERROR_NONE;
}

int mv_face_tracking_model_prepare_open(
		mv_face_tracking_model_h tracking_model,
		mv_engine_config_h /*engine_cfg*/,
		mv_source_h source,
		mv_quadrangle_s *location)
{
	if (!tracking_model) {
		LOGE("Can't prepare tracking model. Handle has to be not NULL");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	if (!source) {
		LOGE("Can't prepare tracking model. "
			"Media Vision source handle has to be not NULL");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	FaceTrackingModel *pTrackModel =
			static_cast<FaceTrackingModel*>(tracking_model);

	if (!pTrackModel) {
		LOGE("Preparation of the face tracking model failed. "
			"Incorrect Media Vision Face tracking model handle is used");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	cv::Mat image;
	int ret = convertSourceMV2GrayCV(source, image);
	if (MEDIA_VISION_ERROR_NONE != ret) {
		LOGE("Convertion mv_source_h to gray failed");
		return ret;
	}

	cv::Rect_<double> roi;
	if (!location) {
		ret = pTrackModel->prepare(image);
	} else {
		int minX = image.cols;
		int minY = image.rows;
		int maxX = 0.0;
		int maxY = 0.0;
		for (unsigned i = 0; i < 4; ++i) {
			minX = minX > location->points[i].x ? location->points[i].x : minX;
			minY = minY > location->points[i].y ? location->points[i].y : minY;
			maxX = maxX < location->points[i].x ? location->points[i].x : maxX;
			maxY = maxY < location->points[i].y ? location->points[i].y : maxY;
		}

		roi.x = minX;
		roi.y = minY;
		roi.width = maxX - minX;
		roi.height = maxY - minY;
		ret = pTrackModel->prepare(image, roi);
	}

	if (MEDIA_VISION_ERROR_NONE != ret) {
		LOGE("Error occurred when prepare face tracking model");
		return ret;
	}

	LOGD("Face tracking model has been prepared");

	return ret;
}

int mv_face_tracking_model_clone_open(
		mv_face_tracking_model_h src,
		mv_face_tracking_model_h *dst)
{
	if (!src || !dst) {
		LOGE("Can't clone tracking model. Both source and destination"
			"tracking model handles has to be not NULL");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	(*dst) = static_cast<mv_face_tracking_model_h>(new (std::nothrow)FaceTrackingModel());

	if (*dst == NULL) {
		LOGE("Failed to create media vision tracking model");
		return MEDIA_VISION_ERROR_OUT_OF_MEMORY;
	}

	LOGD("Tracking model [%p] has been created", *dst);

	const FaceTrackingModel *pSrcModel = static_cast<FaceTrackingModel*>(src);
	FaceTrackingModel *pDstModel = static_cast<FaceTrackingModel*>(*dst);

	*pDstModel = *pSrcModel;

	LOGD("Media vision tracking model has been cloned");

	return MEDIA_VISION_ERROR_NONE;
}

int mv_face_tracking_model_save_open(
		const char *file_name,
		mv_face_tracking_model_h tracking_model)
{
	if (!tracking_model) {
		LOGE("Can't save tracking model to the file. "
			"Handle has to be not NULL");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	if (NULL == file_name) {
		LOGE("Can't save tracking model to the file. "
			"File name has to be specified");
		return MEDIA_VISION_ERROR_INVALID_PATH;
	}

	FaceTrackingModel *pTrackModel = static_cast<FaceTrackingModel*>(tracking_model);

	if (!pTrackModel) {
		LOGE("Saving of the face tracking model to file failed. "
			"Incorrect Media Vision Face tracking model handle is used");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	const int ret = pTrackModel->save(std::string(file_name));

	if (MEDIA_VISION_ERROR_NONE != ret) {
		LOGE("Error occurred when save tracking model to the file");
		return ret;
	}

	LOGD("Media vision tracking model has been saved to the file [%s]", file_name);

	return ret;
}

int mv_face_tracking_model_load_open(
		const char *file_name,
		mv_face_tracking_model_h *tracking_model)
{
	if (!tracking_model) {
		LOGE("Can't load tracking model from the file. "
			"Handle has to be not NULL");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	if (NULL == file_name) {
		LOGE("Can't load tracking model from the file. "
			"File name has to be specified");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	(*tracking_model) =
		static_cast<mv_face_tracking_model_h>(new (std::nothrow)FaceTrackingModel());

	if (*tracking_model == NULL) {
		LOGE("Failed to create media vision tracking model");
		return MEDIA_VISION_ERROR_OUT_OF_MEMORY;
	}

	FaceTrackingModel *pTrackModel =
			static_cast<FaceTrackingModel*>(*tracking_model);

	if (!pTrackModel) {
		LOGE("Loading of the face tracking model from file failed. "
			"Incorrect Media Vision Face tracking model handle is used");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	const int ret = pTrackModel->load(std::string(file_name));

	if (MEDIA_VISION_ERROR_NONE != ret) {
		LOGE("Error occurred when save recognition model to the file");
		return ret;
	}

	LOGD("Media vision recognition model has been loaded from the file [%s]", file_name);

	return ret;
}
