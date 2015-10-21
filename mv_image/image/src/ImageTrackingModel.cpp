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

#include "ImageTrackingModel.h"

#include <app_common.h>

#include "mv_private.h"
#include "mv_common.h"

#include <fstream>
#include <unistd.h>

namespace MediaVision {
namespace Image {
ImageTrackingModel::ImageTrackingModel() :
	m_recognitionObject(),
	m_lastLocation(0),
	m_state(Invalid),
	m_recognitionThread(0)
{
	pthread_mutex_init(&m_globalGuard, NULL);
	pthread_spin_init(&m_lastLocationGuard, PTHREAD_PROCESS_SHARED);
	pthread_spin_init(&m_stateGuard, PTHREAD_PROCESS_SHARED);
}

ImageTrackingModel::ImageTrackingModel(const ImageObject& recognitionObject) :
	m_recognitionObject(recognitionObject),
	m_lastLocation(0),
	m_state(Invalid),
	m_recognitionThread(0)
{
	if (!recognitionObject.isEmpty()) {
		m_state = Undetected;
	}
	pthread_mutex_init(&m_globalGuard, NULL);
	pthread_spin_init(&m_lastLocationGuard, PTHREAD_PROCESS_SHARED);
	pthread_spin_init(&m_stateGuard, PTHREAD_PROCESS_SHARED);
}

ImageTrackingModel::ImageTrackingModel(const ImageTrackingModel& copy) :
	m_recognitionThread(0)
{
	pthread_mutex_init(&m_globalGuard, NULL);
	pthread_spin_init(&m_lastLocationGuard, PTHREAD_PROCESS_SHARED);
	pthread_spin_init(&m_stateGuard, PTHREAD_PROCESS_SHARED);

	*this = copy;
}

ImageTrackingModel::~ImageTrackingModel()
{
	if (m_recognitionThread) {
		pthread_join(m_recognitionThread, NULL);
	}

	pthread_mutex_destroy(&m_globalGuard);
	pthread_spin_destroy(&m_lastLocationGuard);
	pthread_spin_destroy(&m_stateGuard);
}

void ImageTrackingModel::setTarget(const ImageObject& target)
{
	pthread_mutex_lock(&m_globalGuard);

	pthread_spin_lock(&m_stateGuard);
	m_state = target.isEmpty() ? Invalid : Undetected;
	pthread_spin_unlock(&m_stateGuard);

	pthread_spin_lock(&m_lastLocationGuard);
	m_lastLocation.clear();
	pthread_spin_unlock(&m_lastLocationGuard);

	LOGI("[%s] Target is set into tracking model.", __FUNCTION__);

	m_recognitionObject = target;

	pthread_mutex_unlock(&m_globalGuard);
}

void ImageTrackingModel::refresh(void)
{
	pthread_mutex_lock(&m_globalGuard);

	pthread_spin_lock(&m_stateGuard);
	m_state = m_recognitionObject.isEmpty() ? Invalid : Undetected;
	pthread_spin_unlock(&m_stateGuard);

	pthread_spin_lock(&m_lastLocationGuard);
	m_lastLocation.clear();
	pthread_spin_unlock(&m_lastLocationGuard);

	LOGI("[%s] Image tracking model is refreshed.", __FUNCTION__);

	pthread_mutex_unlock(&m_globalGuard);
}

bool ImageTrackingModel::isValid() const
{
	bool result = false;

	pthread_spin_lock(&m_stateGuard);
	result = (m_state != Invalid);
	pthread_spin_unlock(&m_stateGuard);

	return result;
}

ImageTrackingModel& ImageTrackingModel::operator=(const ImageTrackingModel& copy)
{
	if (this != &copy) {
		pthread_mutex_t *higherMutex = &m_globalGuard;
		pthread_mutex_t *lowerMutex = &copy.m_globalGuard;

		if (higherMutex < lowerMutex) {
			std::swap(higherMutex, lowerMutex);
		}

		pthread_mutex_lock(higherMutex);
		pthread_mutex_lock(lowerMutex);

		m_recognitionObject = copy.m_recognitionObject;

		pthread_spin_lock(&m_lastLocationGuard);
		m_lastLocation = copy.m_lastLocation;
		pthread_spin_unlock(&m_lastLocationGuard);

		if (copy.m_state == InProcess) {
			pthread_spin_lock(&m_stateGuard);
			m_state = m_recognitionObject.isEmpty() ? Invalid : Undetected;
			pthread_spin_unlock(&m_stateGuard);
		} else {
			pthread_spin_lock(&m_stateGuard);
			m_state = copy.m_state;
			pthread_spin_unlock(&m_stateGuard);
		}

		pthread_mutex_unlock(lowerMutex);
		pthread_mutex_unlock(higherMutex);
	}

	return *this;
}

int ImageTrackingModel::save(const char *fileName) const
{
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

	std::ofstream out;
	out.open(filePath.c_str());

	if (!out.is_open()) {
		LOGE("[%s] Can't create/open file.", __FUNCTION__);
		return MEDIA_VISION_ERROR_PERMISSION_DENIED;
	}

	out << (*this);

	out.close();
	LOGI("[%s] Image tracking model is saved.", __FUNCTION__);

	return MEDIA_VISION_ERROR_NONE;
}

int ImageTrackingModel::load(const char *fileName)
{
	/* find directory */
	std::string prefix_path = std::string(app_get_data_path());
	LOGD("prefix_path: %s", prefix_path.c_str());

	std::string filePath;
	filePath += prefix_path;
	filePath += fileName;

	if (access(filePath.c_str(), F_OK)) {
		LOGE("Can't load tracking model. Path[%s] doesn't existed.", filePath.c_str());

		return MEDIA_VISION_ERROR_INVALID_PATH;
	}

	std::ifstream in;
	in.open(filePath.c_str());

	if (!in.is_open()) {
		LOGE("[%s] Can't open file.", __FUNCTION__);
		return MEDIA_VISION_ERROR_PERMISSION_DENIED;
	}

	in >> (*this);

	if (!in.good()) {
		LOGE("[%s] Unexpected end of file.", __FUNCTION__);
		return MEDIA_VISION_ERROR_PERMISSION_DENIED;
	}

	in.close();
	LOGI("[%s] Image tracking model is loaded.", __FUNCTION__);

	return MEDIA_VISION_ERROR_NONE;
}

bool ImageTrackingModel::isDetected() const
{
	bool result = false;

	pthread_spin_lock(&m_stateGuard);
	result = (m_state == Tracked);
	pthread_spin_unlock(&m_stateGuard);

	return result;
}

std::vector<cv::Point2f> ImageTrackingModel::getLastlocation() const
{
	std::vector<cv::Point2f> result;

	pthread_spin_lock(&m_lastLocationGuard);
	result = m_lastLocation;
	pthread_spin_unlock(&m_lastLocationGuard);

	return result;
}

#define STATE_UNSEEN_IO_ID 0
#define STATE_VISIBLE_IO_ID 1

std::ostream& operator << (std::ostream& os, const ImageTrackingModel::State& state)
{
	if (ImageTrackingModel::Tracked == state) {
		os << STATE_VISIBLE_IO_ID;
	} else {
		os << STATE_UNSEEN_IO_ID;
	}

	return os;
}

std::istream& operator >> (std::istream& is, ImageTrackingModel::State& state)
{
	int stateId = -1;

	is >> stateId;

	if (STATE_VISIBLE_IO_ID == stateId) {
		state = ImageTrackingModel::Tracked;
	} else {
		state = ImageTrackingModel::Undetected;
	}

	return is;
}

#undef STATE_UNSEEN_IO_ID
#undef STATE_VISIBLE_IO_ID

std::ostream& operator << (std::ostream& os, const ImageTrackingModel& obj)
{
	os << std::setprecision(7);

	pthread_mutex_lock(&obj.m_globalGuard);

	os << obj.m_recognitionObject;

	os << obj.m_lastLocation.size();
	for (size_t pointNum = 0u; pointNum < obj.m_lastLocation.size(); ++pointNum) {
		os << ' ' << obj.m_lastLocation[pointNum].x << ' ' << obj.m_lastLocation[pointNum].y;
	}
	os << '\n';

	os << obj.m_state << '\n';

	pthread_mutex_unlock(&obj.m_globalGuard);

	return os;
}

std::istream& operator >> (std::istream& is, ImageTrackingModel& obj)
{
#define MEDIA_VISION_CHECK_IFSTREAM \
	if (!is.good()) { \
		return is; \
	}

	ImageTrackingModel temporal;

	is >> obj.m_recognitionObject;
	MEDIA_VISION_CHECK_IFSTREAM

	size_t lastLocationAmount = 0u;
	is >> lastLocationAmount;
	MEDIA_VISION_CHECK_IFSTREAM

	temporal.m_lastLocation.resize(lastLocationAmount);
	for (size_t pointNum = 0u; pointNum < lastLocationAmount; ++pointNum) {
		is >> temporal.m_lastLocation[pointNum].x;
		MEDIA_VISION_CHECK_IFSTREAM
		is >> temporal.m_lastLocation[pointNum].y;
		MEDIA_VISION_CHECK_IFSTREAM
	}

	is >> temporal.m_state;
	MEDIA_VISION_CHECK_IFSTREAM

	if (temporal.m_recognitionObject.isEmpty()) {
		temporal.m_state = ImageTrackingModel::Invalid;
	}

	obj = temporal;

	return is;
}

} /* Image */
} /* MediaVision */
