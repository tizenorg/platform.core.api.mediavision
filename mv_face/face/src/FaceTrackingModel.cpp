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

#include "FaceTrackingModel.h"

#include <app_common.h>

#include "mv_private.h"
#include "mv_common.h"

#include <unistd.h>

namespace MediaVision {
namespace Face {
FaceTrackingResults::FaceTrackingResults() :
	mIsTracked(false),
	mConfidence(0.f)
{
	; /* NULL */
}

FaceTrackingModel::FaceTrackingModel() :
	m_canTrack(false),
	m_tracker(new cv::TrackerMedianFlow())
{
	; /* NULL */
}

FaceTrackingModel::FaceTrackingModel(const FaceTrackingModel& origin) :
	m_canTrack(origin.m_canTrack),
	m_tracker(new cv::TrackerMedianFlow())
{
	if (!origin.m_tracker.empty()) {
		origin.m_tracker->copyTo(*(m_tracker.obj));
	}
}

FaceTrackingModel& FaceTrackingModel::operator=(const FaceTrackingModel& copy)
{
	if (this != &copy) {
		m_canTrack = copy.m_canTrack;
		m_tracker = cv::Ptr<cv::TrackerMedianFlow>(new cv::TrackerMedianFlow());
		if (!copy.m_tracker.empty()) {
			copy.m_tracker->copyTo(*(m_tracker.obj));
		}
	}

	return *this;
}

FaceTrackingModel::~FaceTrackingModel()
{
	; /* NULL */
}

int FaceTrackingModel::save(const std::string& fileName)
{
	if (m_tracker.empty()) {
		LOGE("Can't save tracking model. No tracking algorithm is used");
		return MEDIA_VISION_ERROR_INVALID_OPERATION;
	}

	std::string prefix_path = std::string(app_get_data_path());
	LOGD("prefix_path: %s", prefix_path.c_str());

	std::string filePath;
	filePath += prefix_path;
filePath += fileName;

	/* check the directory is available */
	std::string prefix_path_check = filePath.substr(0, filePath.find_last_of('/'));
	if (access(prefix_path_check.c_str(), F_OK)) {
		LOGE("Can't save tracking model. Path[%s] doesn't existed.", prefix_path_check.c_str());

		return MEDIA_VISION_ERROR_INVALID_PATH;
	}

	cv::FileStorage storage(filePath, cv::FileStorage::WRITE);
	if (!storage.isOpened()) {
		LOGE("Can't save tracking model. Write to file permission denied.");
		return MEDIA_VISION_ERROR_PERMISSION_DENIED;
	}

	LOGD("Storing tracking model to the file started.");

	storage << "canTrack" << (m_canTrack ? 1 : 0);
	m_tracker->write(storage);

	LOGD("Storing tracking model to the file finished.");

	storage.release();

	return MEDIA_VISION_ERROR_NONE;
}

int FaceTrackingModel::load(const std::string& fileName)
{
	/* find directory */
	std::string prefix_path = std::string(app_get_data_path());
	LOGD("prefix_path: %s", prefix_path.c_str());

	std::string filePath;
	filePath += prefix_path;
	filePath += fileName;

	if (access(filePath.c_str(), F_OK)) {
		LOGE("Can't load face tracking model. File[%s] doesn't exist.", filePath.c_str());

		return MEDIA_VISION_ERROR_INVALID_PATH;
	}

	cv::FileStorage storage(filePath, cv::FileStorage::READ);
	if (!storage.isOpened()) {
		LOGE("Can't load tracking model. Read from file permission denied.");
		return MEDIA_VISION_ERROR_PERMISSION_DENIED;
	}

	int canTrack = 0;
	storage["canTrack"] >> canTrack;
	m_canTrack = (0 != canTrack);
	m_tracker->read(storage);

	LOGD("Loading tracking model from file.");

	storage.release();

	return MEDIA_VISION_ERROR_NONE;
}

int FaceTrackingModel::prepare(const cv::Mat& image)
{
	if (m_tracker.empty()) {
		LOGE("Failed to prepare tracking model. No tracking algorithm "
			"is available.");
			return MEDIA_VISION_ERROR_INVALID_OPERATION;
	}

	cv::Rect_<float> lastBoundingBox;
	if (!m_tracker->isInited()) {
		lastBoundingBox.x = 0;
		lastBoundingBox.y = 0;
		lastBoundingBox.width = image.cols;
		lastBoundingBox.height = image.rows;
	} else {
		lastBoundingBox = m_tracker->getLastBoundingBox();
	}

	return prepare(image, lastBoundingBox);
}

int FaceTrackingModel::prepare(
		const cv::Mat& image,
		const cv::Rect_<float>& boundingBox)
{
	if (m_tracker.empty()) {
		LOGE("Failed to prepare tracking model. No tracking algorithm "
			"is available.");
		return MEDIA_VISION_ERROR_INVALID_OPERATION;
	}

	if (!m_tracker->init(image, boundingBox)) {
		LOGE("Failed to prepare tracking model.");
		return MEDIA_VISION_ERROR_INVALID_OPERATION;
	}

	m_canTrack = true;
	return MEDIA_VISION_ERROR_NONE;
}

int FaceTrackingModel::track(const cv::Mat& image, FaceTrackingResults& results)
{
	if (!m_tracker.empty() && m_canTrack) {
		results.mIsTracked = m_tracker->update(image, results.mFaceLocation);
		results.mConfidence = m_tracker->getLastConfidence();
	} else {
		LOGE("Attempt to track face with not prepared model");
		return MEDIA_VISION_ERROR_INVALID_OPERATION;
	}

	return MEDIA_VISION_ERROR_NONE;
}

} /* Face */
} /* MediaVision */
