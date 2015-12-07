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

#include "mv_image_open.h"

#include "mv_private.h"
#include "mv_common_c.h"

#include "ImageConfig.h"
#include "Features/FeatureExtractor.h"
#include "Features/ORBExtractorFactory.h"
#include "Recognition/ImageObject.h"
#include "Recognition/ImageRecognizer.h"
#include "Tracking/ImageTrackingModel.h"

#include <opencv/cv.h>

namespace {
class DefaultConfiguration {
public:
	static const DefaultConfiguration& getInstance();

	MediaVision::Image::FeaturesExtractingParams getObjectFeaturesExtractingParams() const;

	MediaVision::Image::FeaturesExtractingParams getSceneFeaturesExtractingParams() const;

	MediaVision::Image::RecognitionParams getRecognitionParams() const;

	MediaVision::Image::StabilizationParams getStabilizationParams() const;

	MediaVision::Image::TrackingParams getTrackingParams() const;

private:
	DefaultConfiguration();

private:
	static DefaultConfiguration instance;

	MediaVision::Image::FeaturesExtractingParams m_objectFeaturesExtractingParams;

	MediaVision::Image::FeaturesExtractingParams m_sceneFeaturesExtractingParams;

	MediaVision::Image::RecognitionParams m_recognitionParams;

	MediaVision::Image::StabilizationParams m_stabilizationParams;

	MediaVision::Image::TrackingParams m_trackingParams;
};

DefaultConfiguration DefaultConfiguration::instance;

DefaultConfiguration::DefaultConfiguration() :
		m_objectFeaturesExtractingParams(),
		m_sceneFeaturesExtractingParams(),
		m_recognitionParams(15, 0.33, 0.1),
		m_stabilizationParams(true, 3, 0.00006, 1.3, 2, 0.001),
		m_trackingParams()
{
	m_objectFeaturesExtractingParams.mKeypointType = MediaVision::Image::KT_ORB;
	m_objectFeaturesExtractingParams.mDescriptorType = MediaVision::Image::DT_ORB;
	m_objectFeaturesExtractingParams.ORB.mScaleFactor = 1.2;
	m_objectFeaturesExtractingParams.ORB.mMaximumFeaturesNumber = 1000;

	m_sceneFeaturesExtractingParams.mKeypointType = MediaVision::Image::KT_ORB;
	m_sceneFeaturesExtractingParams.mDescriptorType = MediaVision::Image::DT_ORB;
	m_sceneFeaturesExtractingParams.ORB.mScaleFactor = 1.2;
	m_sceneFeaturesExtractingParams.ORB.mMaximumFeaturesNumber = 5000;

	m_trackingParams.mFramesFeaturesExtractingParams = m_sceneFeaturesExtractingParams;
	m_trackingParams.mRecognitionParams = m_recognitionParams;
	m_trackingParams.mStabilizationParams = m_stabilizationParams;
	m_trackingParams.mExpectedOffset = 0.0;
}

const DefaultConfiguration& DefaultConfiguration::getInstance()
{
	return instance;
}

MediaVision::Image::FeaturesExtractingParams
DefaultConfiguration::getObjectFeaturesExtractingParams() const
{
	return m_objectFeaturesExtractingParams;
}

MediaVision::Image::FeaturesExtractingParams
DefaultConfiguration::getSceneFeaturesExtractingParams() const
{
	return m_sceneFeaturesExtractingParams;
}

MediaVision::Image::RecognitionParams
DefaultConfiguration::getRecognitionParams() const
{
	return m_recognitionParams;
}

MediaVision::Image::StabilizationParams
DefaultConfiguration::getStabilizationParams() const
{
	return m_stabilizationParams;
}

MediaVision::Image::TrackingParams
DefaultConfiguration::getTrackingParams() const
{
	return m_trackingParams;
}

void extractTargetFeaturesExtractingParams(
		mv_engine_config_h engine_cfg,
		MediaVision::Image::FeaturesExtractingParams& featuresExtractingParams)
{
	mv_engine_config_h working_cfg = NULL;

	if (NULL == engine_cfg) {
		mv_create_engine_config(&working_cfg);
	} else {
		working_cfg = engine_cfg;
	}

	featuresExtractingParams =
			DefaultConfiguration::getInstance().getObjectFeaturesExtractingParams();

	mv_engine_config_get_double_attribute_c(
			working_cfg,
			MV_IMAGE_RECOGNITION_OBJECT_SCALE_FACTOR,
			&featuresExtractingParams.ORB.mScaleFactor);

	mv_engine_config_get_int_attribute_c(
			working_cfg,
			MV_IMAGE_RECOGNITION_OBJECT_MAX_KEYPOINTS_NUM,
			&featuresExtractingParams.ORB.mMaximumFeaturesNumber);

	if (NULL == engine_cfg) {
		mv_destroy_engine_config(working_cfg);
	}
}

void extractSceneFeaturesExtractingParams(
		mv_engine_config_h engine_cfg,
		MediaVision::Image::FeaturesExtractingParams& featuresExtractingParams)
{
	mv_engine_config_h working_cfg = NULL;

	if (NULL == engine_cfg) {
		mv_create_engine_config(&working_cfg);
	} else {
		working_cfg = engine_cfg;
	}

	featuresExtractingParams =
			DefaultConfiguration::getInstance().getSceneFeaturesExtractingParams();

	mv_engine_config_get_double_attribute_c(
			working_cfg,
			MV_IMAGE_RECOGNITION_SCENE_SCALE_FACTOR,
			&featuresExtractingParams.ORB.mScaleFactor);

	mv_engine_config_get_int_attribute_c(
			working_cfg,
			MV_IMAGE_RECOGNITION_SCENE_MAX_KEYPOINTS_NUM,
			&featuresExtractingParams.ORB.mMaximumFeaturesNumber);

	if (NULL == engine_cfg) {
		mv_destroy_engine_config(working_cfg);
	}
}

void extractRecognitionParams(
		mv_engine_config_h engine_cfg,
		MediaVision::Image::RecognitionParams& recognitionParams)
{
	mv_engine_config_h working_cfg = NULL;

	if (NULL == engine_cfg) {
		mv_create_engine_config(&working_cfg);
	} else {
		working_cfg = engine_cfg;
	}

	recognitionParams =
			DefaultConfiguration::getInstance().getRecognitionParams();

	mv_engine_config_get_int_attribute_c(
			working_cfg,
			MV_IMAGE_RECOGNITION_MIN_MATCH_NUM,
			&recognitionParams.mMinMatchesNumber);

	mv_engine_config_get_double_attribute_c(
			working_cfg,
			MV_IMAGE_RECOGNITION_REQ_MATCH_PART,
			&recognitionParams.mRequiredMatchesPart);

	mv_engine_config_get_double_attribute_c(
			working_cfg,
			MV_IMAGE_RECOGNITION_TOLERANT_MATCH_PART_ERR,
			&recognitionParams.mTolerantMatchesPartError);

	if (NULL == engine_cfg) {
		mv_destroy_engine_config(working_cfg);
	}
}

void extractStabilizationParams(
		mv_engine_config_h engine_cfg,
		MediaVision::Image::StabilizationParams& stabilizationParams)
{
	mv_engine_config_h working_cfg = NULL;

	if (NULL == engine_cfg) {
		mv_create_engine_config(&working_cfg);
	} else {
		working_cfg = engine_cfg;
	}

	stabilizationParams =
			DefaultConfiguration::getInstance().getStabilizationParams();

	mv_engine_config_get_bool_attribute_c(
			working_cfg,
			MV_IMAGE_TRACKING_USE_STABLIZATION,
			&stabilizationParams.mIsEnabled);

	mv_engine_config_get_int_attribute_c(
			working_cfg,
			MV_IMAGE_TRACKING_HISTORY_AMOUNT,
			&stabilizationParams.mHistoryAmount);

	mv_engine_config_get_double_attribute_c(
			working_cfg,
			MV_IMAGE_TRACKING_STABLIZATION_TOLERANT_SHIFT,
			&stabilizationParams.mTolerantShift);

	mv_engine_config_get_double_attribute_c(
			working_cfg,
			MV_IMAGE_TRACKING_STABLIZATION_SPEED,
			&stabilizationParams.mStabilizationSpeed);

	mv_engine_config_get_double_attribute_c(
			working_cfg,
			MV_IMAGE_TRACKING_STABLIZATION_ACCELERATION,
			&stabilizationParams.mStabilizationAcceleration);

	if (NULL == engine_cfg) {
		mv_destroy_engine_config(working_cfg);
	}
}

void extractTrackingParams(
		mv_engine_config_h engine_cfg,
		MediaVision::Image::TrackingParams& trackingParams)
{
	mv_engine_config_h working_cfg = NULL;

	if (NULL == engine_cfg) {
		mv_create_engine_config(&working_cfg);
	} else {
		working_cfg = engine_cfg;
	}

	trackingParams =
			DefaultConfiguration::getInstance().getTrackingParams();

	extractSceneFeaturesExtractingParams(
			working_cfg,
			trackingParams.mFramesFeaturesExtractingParams);

	extractRecognitionParams(
			working_cfg,
			trackingParams.mRecognitionParams);

	extractStabilizationParams(
			working_cfg,
			trackingParams.mStabilizationParams);

	mv_engine_config_get_double_attribute_c(
			working_cfg,
			MV_IMAGE_TRACKING_EXPECTED_OFFSET,
			&trackingParams.mExpectedOffset);

	if (NULL == engine_cfg) {
		mv_destroy_engine_config(working_cfg);
	}
}

int convertSourceMV2GrayCV(mv_source_h mvSource, cv::Mat& cvSource)
{
	MEDIA_VISION_INSTANCE_CHECK(mvSource);

	int depth = CV_8U; // Default depth. 1 byte for channel.
	unsigned int channelsNumber = 0u;
	unsigned int width = 0u, height = 0u;
	unsigned int bufferSize = 0u;
	unsigned char *buffer = NULL;

	mv_colorspace_e colorspace = MEDIA_VISION_COLORSPACE_INVALID;

	MEDIA_VISION_ASSERT(mv_source_get_width(mvSource, &width),
			"Failed to get the width.");
	MEDIA_VISION_ASSERT(mv_source_get_height(mvSource, &height),
			"Failed to get the height.");
	MEDIA_VISION_ASSERT(mv_source_get_colorspace(mvSource, &colorspace),
			"Failed to get the colorspace.");
	MEDIA_VISION_ASSERT(mv_source_get_buffer(mvSource, &buffer, &bufferSize),
			"Failed to get the buffer size.");

	int conversionType = -1; /* Type of conversion from given colorspace to gray */
	switch(colorspace) {
	case MEDIA_VISION_COLORSPACE_INVALID:
		LOGE("Error: mv_source has invalid colorspace.");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	case MEDIA_VISION_COLORSPACE_Y800:
		channelsNumber = 1;
		/* Without convertion */
		break;
	case MEDIA_VISION_COLORSPACE_I420:
		channelsNumber = 1;
		height *= 1.5;
		conversionType = CV_YUV2GRAY_I420;
		break;
	case MEDIA_VISION_COLORSPACE_NV12:
		channelsNumber = 1;
		height *= 1.5;
		conversionType = CV_YUV2GRAY_NV12;
		break;
	case MEDIA_VISION_COLORSPACE_YV12:
		channelsNumber = 1;
		height *= 1.5;
		conversionType = CV_YUV2GRAY_YV12;
		break;
	case MEDIA_VISION_COLORSPACE_NV21:
		channelsNumber = 1;
		height *= 1.5;
		conversionType = CV_YUV2GRAY_NV21;
		break;
	case MEDIA_VISION_COLORSPACE_YUYV:
		channelsNumber = 2;
		conversionType = CV_YUV2GRAY_YUYV;
		break;
	case MEDIA_VISION_COLORSPACE_UYVY:
		channelsNumber = 2;
		conversionType = CV_YUV2GRAY_UYVY;
		break;
	case MEDIA_VISION_COLORSPACE_422P:
		channelsNumber = 2;
		conversionType = CV_YUV2GRAY_Y422;
		break;
	case MEDIA_VISION_COLORSPACE_RGB565:
		channelsNumber = 2;
		conversionType = CV_BGR5652GRAY;
		break;
	case MEDIA_VISION_COLORSPACE_RGB888:
		channelsNumber = 3;
		conversionType = CV_RGB2GRAY;
		break;
	case MEDIA_VISION_COLORSPACE_RGBA:
		channelsNumber = 4;
		conversionType = CV_RGBA2GRAY;
		break;
	default:
		LOGE("Error: mv_source has unsupported colorspace.");
		return MEDIA_VISION_ERROR_NOT_SUPPORTED_FORMAT;
	}

	if (conversionType == -1) {/* Without conversion */
		cvSource = cv::Mat(cv::Size(width, height),
					CV_MAKETYPE(depth, channelsNumber), buffer).clone();
	} else {/* With conversion */
		/* Class for representation the given image as cv::Mat before conversion */
		cv::Mat origin(cv::Size(width, height),
				CV_MAKETYPE(depth, channelsNumber), buffer);
				cv::cvtColor(origin, cvSource, conversionType);
	}

	return MEDIA_VISION_ERROR_NONE;
}

} /* anonymous namespace */

int mv_image_recognize_open(
		mv_source_h source,
		const mv_image_object_h *image_objects,
		int number_of_objects,
		mv_engine_config_h engine_cfg,
		mv_image_recognized_cb recognized_cb,
		void *user_data)
{
	MEDIA_VISION_INSTANCE_CHECK(source);
	MEDIA_VISION_NULL_ARG_CHECK(image_objects);
	for (int objectNum = 0; objectNum < number_of_objects; ++objectNum) {
		MEDIA_VISION_INSTANCE_CHECK(image_objects[objectNum]);
	}
	MEDIA_VISION_NULL_ARG_CHECK(recognized_cb);

	cv::Mat scene;
	MEDIA_VISION_ASSERT(
			convertSourceMV2GrayCV(source, scene),
			"Failed to convert mv_source.");

	int ret = MEDIA_VISION_ERROR_NONE;

	MediaVision::Image::FeaturesExtractingParams featuresExtractingParams;
	extractSceneFeaturesExtractingParams(engine_cfg, featuresExtractingParams);

	MediaVision::Image::ImageObject sceneImageObject(scene, featuresExtractingParams);

	MediaVision::Image::RecognitionParams recognitionParams;
	extractRecognitionParams(engine_cfg, recognitionParams);

	MediaVision::Image::ImageRecognizer recognizer(sceneImageObject);

	mv_quadrangle_s *resultLocations[number_of_objects];

	for (int objectNum = 0; objectNum < number_of_objects; ++objectNum) {
		std::vector<cv::Point2f> resultContour;
		bool isRecognized = recognizer.recognize(
						*((MediaVision::Image::ImageObject*)image_objects[objectNum]),
						recognitionParams, resultContour);
		if (isRecognized && (resultContour.size() ==
			MediaVision::Image::NumberOfQuadrangleCorners)) {
			resultLocations[objectNum] = new mv_quadrangle_s;
			if (resultLocations[objectNum] == NULL) {
				ret = MEDIA_VISION_ERROR_OUT_OF_MEMORY;
				goto ErrorExit;
			}

			for (size_t pointNum = 0u;
				pointNum < MediaVision::Image::NumberOfQuadrangleCorners;
				++pointNum) {
				resultLocations[objectNum]->points[pointNum].x =
							resultContour[pointNum].x;
				resultLocations[objectNum]->points[pointNum].y =
							resultContour[pointNum].y;
			}
		} else {
			resultLocations[objectNum] = NULL;
		}
	}

	recognized_cb(
			source,
			engine_cfg,
			image_objects,
			resultLocations,
			number_of_objects,
			user_data);

ErrorExit:

	for (int objectNum = 0; objectNum < number_of_objects; ++objectNum) {
		if (resultLocations[objectNum] != NULL) {
			delete resultLocations[objectNum];
			resultLocations[objectNum] = NULL;
		}
	}

	return ret;
}

int mv_image_track_open(
		mv_source_h source,
		mv_image_tracking_model_h image_tracking_model,
		mv_engine_config_h engine_cfg,
		mv_image_tracked_cb tracked_cb,
		void *user_data)
{
	MEDIA_VISION_INSTANCE_CHECK(source);
	MEDIA_VISION_INSTANCE_CHECK(image_tracking_model);
	MEDIA_VISION_NULL_ARG_CHECK(tracked_cb);

	if (!((MediaVision::Image::ImageTrackingModel*)image_tracking_model)->isValid()) {
		LOGE("[%s] Image tracking model is invalid.", __FUNCTION__);
		return MEDIA_VISION_ERROR_INVALID_DATA;
	}

	MediaVision::Image::TrackingParams trackingParams;
	extractTrackingParams(engine_cfg, trackingParams);

	cv::Mat frame;
	MEDIA_VISION_ASSERT(
			convertSourceMV2GrayCV(source, frame),
			"Failed to convert mv_source.");

	MediaVision::Image::ImageTrackingModel *trackingModel =
			(MediaVision::Image::ImageTrackingModel*)image_tracking_model;

	std::vector<cv::Point> resultContour;
	const bool isTracked = trackingModel->track(frame, resultContour);

	if (isTracked &&
		MediaVision::Image::NumberOfQuadrangleCorners == resultContour.size()) {
		mv_quadrangle_s result;
		for (size_t pointNum = 0u;
			pointNum < MediaVision::Image::NumberOfQuadrangleCorners;
			++pointNum) {
			result.points[pointNum].x = resultContour[pointNum].x;
			result.points[pointNum].y = resultContour[pointNum].y;
		}
		tracked_cb(source, image_tracking_model, engine_cfg, &result, user_data);
	} else {
		tracked_cb(source, image_tracking_model, engine_cfg, NULL, user_data);
	}

	return MEDIA_VISION_ERROR_NONE;
}

int mv_image_object_create_open(
		mv_image_object_h *image_object)
{
	MEDIA_VISION_NULL_ARG_CHECK(image_object);

	(*image_object) = (mv_image_object_h)new (std::nothrow)MediaVision::Image::ImageObject();
	if (*image_object == NULL) {
		return MEDIA_VISION_ERROR_OUT_OF_MEMORY;
	}

	return MEDIA_VISION_ERROR_NONE;
}

int mv_image_object_destroy_open(
		mv_image_object_h image_object)
{
	MEDIA_VISION_INSTANCE_CHECK(image_object);

	delete (MediaVision::Image::ImageObject*)image_object;

	return MEDIA_VISION_ERROR_NONE;
}

int mv_image_object_fill_open(
		mv_image_object_h image_object,
		mv_engine_config_h engine_cfg,
		mv_source_h source,
		mv_rectangle_s *location)
{
	MEDIA_VISION_INSTANCE_CHECK(image_object);
	MEDIA_VISION_INSTANCE_CHECK(source);

	cv::Mat image;
	MEDIA_VISION_ASSERT(
			convertSourceMV2GrayCV(source, image),
			"Failed to convert mv_source.");

	std::vector<cv::Point2f> roi;
	if (NULL != location) {
		roi.resize(4);

		roi[0].x = location->point.x;
		roi[0].y = location->point.y;

		roi[1].x = roi[0].x + location->width;
		roi[1].y = roi[0].y;

		roi[2].x = roi[1].x;
		roi[2].y = roi[1].y + location->height;

		roi[3].x = roi[0].x;
		roi[3].y = roi[2].y;
	}

	MediaVision::Image::FeaturesExtractingParams featuresExtractingParams;
	extractTargetFeaturesExtractingParams(engine_cfg, featuresExtractingParams);

	static_cast<MediaVision::Image::ImageObject*>(image_object)->fill(
			image,
			featuresExtractingParams,
			roi);

	return MEDIA_VISION_ERROR_NONE;
}

int mv_image_object_get_recognition_rate_open(
		mv_image_object_h image_object,
		double *recognition_rate)
{
	MEDIA_VISION_INSTANCE_CHECK(image_object);
	MEDIA_VISION_NULL_ARG_CHECK(recognition_rate);

	(*recognition_rate) =
		((MediaVision::Image::ImageObject*)image_object)->getRecognitionRate();

	return MEDIA_VISION_ERROR_NONE;
}

int mv_image_object_set_label_open(
		mv_image_object_h image_object,
		int label)
{
	MEDIA_VISION_INSTANCE_CHECK(image_object);

	((MediaVision::Image::ImageObject*)image_object)->setLabel(label);

	return MEDIA_VISION_ERROR_NONE;
}
int mv_image_object_get_label_open(
		mv_image_object_h image_object,
		int *label)
{
	MEDIA_VISION_INSTANCE_CHECK(image_object);
	MEDIA_VISION_NULL_ARG_CHECK(label);

	if (!((MediaVision::Image::ImageObject*)image_object)->getLabel(*label)) {
		LOGW("[%s] Image object haven't a label.", __FUNCTION__);
		return MEDIA_VISION_ERROR_NO_DATA;
	}

	return MEDIA_VISION_ERROR_NONE;
}

int mv_image_object_clone_open(
		mv_image_object_h src,
		mv_image_object_h *dst)
{
	MEDIA_VISION_INSTANCE_CHECK(src);
	MEDIA_VISION_NULL_ARG_CHECK(dst);

	(*dst) = (mv_image_object_h)new (std::nothrow)MediaVision::Image::ImageObject();
	if (*dst == NULL) {
		return MEDIA_VISION_ERROR_OUT_OF_MEMORY;
	}

	*(MediaVision::Image::ImageObject*)(*dst) =
			*(MediaVision::Image::ImageObject*)src;

	return MEDIA_VISION_ERROR_NONE;
}

int mv_image_object_save_open(
		const char *file_name, mv_image_object_h image_object)
{
	MEDIA_VISION_INSTANCE_CHECK(image_object);

	if (file_name == NULL) {
		LOGE("File name is NULL. The file name has to be specified");
		return MEDIA_VISION_ERROR_INVALID_PATH;
	}

	int ret = ((MediaVision::Image::ImageObject*)image_object)->save(file_name);
	if (ret != MEDIA_VISION_ERROR_NONE) {
		LOGE("Fail to save image object.");
		return ret;
	}

	return MEDIA_VISION_ERROR_NONE;
}

int mv_image_object_load_open(
		const char *file_name, mv_image_object_h *image_object)
{
	MEDIA_VISION_NULL_ARG_CHECK(image_object);

	if (file_name == NULL) {
		LOGE("File name is NULL. The file name has to be specified");
		return MEDIA_VISION_ERROR_INVALID_PATH;
	}

	(*image_object) = (mv_image_object_h)new (std::nothrow)MediaVision::Image::ImageObject();
	if (*image_object == NULL) {
		return MEDIA_VISION_ERROR_OUT_OF_MEMORY;
	}

	int ret = ((MediaVision::Image::ImageObject*)(*image_object))->load(file_name);
	if (ret != MEDIA_VISION_ERROR_NONE) {
		LOGE("Fail to save image object.");
		return ret;
	}

	return MEDIA_VISION_ERROR_NONE;
}

int mv_image_tracking_model_create_open(
		mv_image_tracking_model_h *image_tracking_model)
{
	MEDIA_VISION_NULL_ARG_CHECK(image_tracking_model);

	(*image_tracking_model) = (mv_image_tracking_model_h)
			new (std::nothrow)MediaVision::Image::ImageTrackingModel();
	if (*image_tracking_model == NULL) {
		return MEDIA_VISION_ERROR_OUT_OF_MEMORY;
	}

	return MEDIA_VISION_ERROR_NONE;
}

int mv_image_tracking_model_set_target_open(
		mv_image_object_h image_object,
		mv_image_tracking_model_h image_tracking_model)
{
	MEDIA_VISION_INSTANCE_CHECK(image_tracking_model);
	MEDIA_VISION_INSTANCE_CHECK(image_object);

	if (((MediaVision::Image::ImageObject*)image_object)->isEmpty()) {
		LOGE("[%s] Target is empty and can't be set as target of tracking"
				"model.", __FUNCTION__);
		return MEDIA_VISION_ERROR_INVALID_DATA;
	}

	((MediaVision::Image::ImageTrackingModel*)image_tracking_model)->setTarget(
			*(MediaVision::Image::ImageObject*)image_object);

	return MEDIA_VISION_ERROR_NONE;
}

int mv_image_tracking_model_destroy_open(
		mv_image_tracking_model_h image_tracking_model)
{
	MEDIA_VISION_INSTANCE_CHECK(image_tracking_model);

	delete (MediaVision::Image::ImageTrackingModel*)image_tracking_model;

	return MEDIA_VISION_ERROR_NONE;
}

int mv_image_tracking_model_refresh_open(
		mv_image_tracking_model_h image_tracking_model,
		mv_engine_config_h /*engine_cfg*/)
{
	MEDIA_VISION_INSTANCE_CHECK(image_tracking_model);

	if (!((MediaVision::Image::ImageTrackingModel*)image_tracking_model)->isValid()) {
		LOGE("[%s] Image tracking model is invalid.", __FUNCTION__);
		return MEDIA_VISION_ERROR_INVALID_DATA;
	}

	((MediaVision::Image::ImageTrackingModel*)image_tracking_model)->refresh();

	return MEDIA_VISION_ERROR_NONE;
}

int mv_image_tracking_model_clone_open(
		mv_image_tracking_model_h src,
		mv_image_tracking_model_h *dst)
{
	MEDIA_VISION_INSTANCE_CHECK(src);
	MEDIA_VISION_NULL_ARG_CHECK(dst);

	(*dst) = (mv_image_tracking_model_h)new (std::nothrow)MediaVision::Image::ImageTrackingModel();
	if (*dst == NULL) {
		return MEDIA_VISION_ERROR_OUT_OF_MEMORY;
	}

	*(MediaVision::Image::ImageTrackingModel*)(*dst) = *(MediaVision::Image::ImageTrackingModel*)src;

	LOGD("Image tracking model has been successfully cloned");
	return MEDIA_VISION_ERROR_NONE;
}

int mv_image_tracking_model_save_open(
		const char *file_name, mv_image_tracking_model_h image_tracking_model)
{
	MEDIA_VISION_INSTANCE_CHECK(image_tracking_model);

	if (file_name == NULL) {
		LOGE("File name is NULL. The file name has to be specified");
		return MEDIA_VISION_ERROR_INVALID_PATH;
	}

	int ret = ((MediaVision::Image::ImageTrackingModel*)image_tracking_model)->save(file_name);
	if (ret != MEDIA_VISION_ERROR_NONE) {
		LOGE("Failed to save image tracking model");
		return ret;
	}

	LOGD("Image tracking model has been successfully saved");
	return MEDIA_VISION_ERROR_NONE;
}

int mv_image_tracking_model_load_open(
		const char *file_name, mv_image_tracking_model_h *image_tracking_model)
{
	MEDIA_VISION_NULL_ARG_CHECK(image_tracking_model);

	if (file_name == NULL) {
		LOGE("File path is NULL. The file name has to be specified");
		return MEDIA_VISION_ERROR_INVALID_PATH;
	}

	(*image_tracking_model) = (mv_image_tracking_model_h)
			new (std::nothrow)MediaVision::Image::ImageTrackingModel();

	if (*image_tracking_model == NULL) {
		return MEDIA_VISION_ERROR_OUT_OF_MEMORY;
	}

	int ret = ((MediaVision::Image::ImageTrackingModel*)(*image_tracking_model))->load(file_name);
	if (ret != MEDIA_VISION_ERROR_NONE) {
		LOGE("Failed to load image tracking model");
		return ret;
	}

	LOGD("Image tracking model has been successfully loaded");
	return MEDIA_VISION_ERROR_NONE;
}
