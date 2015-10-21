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

#include "ImageTracker.h"

#include "ImageMathUtil.h"
#include "ImageRecognizer.h"
#include "ImageTrackingModel.h"
#include "ImageContourStabilizator.h"

#include "mv_private.h"

#include <pthread.h>

namespace MediaVision {
namespace Image {
ImageTracker::ImageTracker(const TrackingParams& trackingParams) :
	m_trackingParams(trackingParams)
{
	; /* NULL */
}

void ImageTracker::track(const cv::Mat& frame, ImageTrackingModel& target)
{
	ImageTrackingModel::State currentState = ImageTrackingModel::Undetected;

	while (pthread_mutex_trylock(&target.m_globalGuard) != 0) {
		pthread_spin_lock(&target.m_stateGuard);
		currentState = target.m_state;
		pthread_spin_unlock(&target.m_stateGuard);

		if (ImageTrackingModel::InProcess == currentState) {
			LOGI("[%s] Calling is skipped. Object is recognizing.", __FUNCTION__);
			return;
		}
	}

	pthread_spin_lock(&target.m_stateGuard);
	currentState = target.m_state;
	pthread_spin_unlock(&target.m_stateGuard);

	if (ImageTrackingModel::Invalid == currentState) {
		pthread_mutex_unlock(&target.m_globalGuard);
		LOGE("[%s] Tracking model is invalid.", __FUNCTION__);
		return;
	}

	switch (target.m_state) {
	case ImageTrackingModel::Appeared:
	case ImageTrackingModel::Tracked: {
		pthread_spin_lock(&target.m_stateGuard);
		target.m_state = ImageTrackingModel::InProcess;
		pthread_spin_unlock(&target.m_stateGuard);

		trackDetectedObject(frame, target);
		break;
	}
	case ImageTrackingModel::Undetected: {
		pthread_spin_lock(&target.m_stateGuard);
		target.m_state = ImageTrackingModel::InProcess;
		pthread_spin_unlock(&target.m_stateGuard);

		trackUndetectedObject(frame, target);

		/* Recognition thread is started. Don't use target here, just exit! */
		return;
	}
	case ImageTrackingModel::InProcess:
	default: {
		/* Abnormal behaviour:
		 * tracking model state is InProcess but globalGuard is not locked
		 */
		LOGE("[%s] Abnormal behaviour. Tracking model status is"
					"\"InProgress\" but it is not in progress.", __FUNCTION__);

		pthread_spin_lock(&target.m_stateGuard);
		if (target.m_recognitionObject.isEmpty()) {
			target.m_state = ImageTrackingModel::Invalid;
			LOGI("[%s] Tracking model status is changed on \"Invalid\"", __FUNCTION__);
		} else {
			target.m_state = ImageTrackingModel::Undetected;
			LOGI("[%s] Tracking model status is changed on \"Undetected\"", __FUNCTION__);
		}
		pthread_spin_unlock(&target.m_stateGuard);

		pthread_mutex_unlock(&target.m_globalGuard);
		break;
	}
	}
}

void ImageTracker::trackDetectedObject(
		const cv::Mat& frame,
		ImageTrackingModel& target)
{
	cv::Rect expectedArea = computeExpectedArea(target, frame.size());

	std::vector<cv::Point2f> resultContour;

	ImageRecognizer recognizer(
			frame(expectedArea),
			m_trackingParams.mFramesFeaturesExtractingParams);

	const bool isRecognized = recognizer.recognize(
			target.m_recognitionObject,
			 m_trackingParams.mRecognitionParams,
		resultContour);

	if (isRecognized) {
		for (size_t pointIdx = 0; pointIdx < resultContour.size(); ++pointIdx) {
			resultContour[pointIdx].x += expectedArea.x;
			resultContour[pointIdx].y += expectedArea.y;
		}

		if (m_trackingParams.mStabilizationParams.mHistoryAmount > 0) {
			target.m_stabilizator.stabilize(
					resultContour,
					m_trackingParams.mStabilizationParams);
		}

		target.m_stabilizator.stabilize(
				resultContour,
				m_trackingParams.mStabilizationParams);

		pthread_spin_lock(&target.m_lastLocationGuard);
		target.m_lastLocation = resultContour;
		pthread_spin_unlock(&target.m_lastLocationGuard);

		pthread_spin_lock(&target.m_stateGuard);
		target.m_state = ImageTrackingModel::Tracked;
		pthread_spin_unlock(&target.m_stateGuard);

		LOGI("[%s] Object is successfully tracked.", __FUNCTION__);
	} else {
		target.m_stabilizator.reset();

		pthread_spin_lock(&target.m_stateGuard);
		target.m_state = ImageTrackingModel::Undetected;
		pthread_spin_unlock(&target.m_stateGuard);

		LOGI("[%s] Object is lost.", __FUNCTION__);
	}

	pthread_mutex_unlock(&target.m_globalGuard);
}

void *ImageTracker::recognitionThreadFunc(void *recognitionInfo)
{
	if (NULL == recognitionInfo) {
		return NULL;
	}

	RecognitionInfo *recogInfo = (RecognitionInfo*)recognitionInfo;

	std::vector<cv::Point2f> resultContour;

	ImageRecognizer recognizer(
			recogInfo->mFrame,
			recogInfo->mSceneFeaturesExtractingParams);

	bool isRecognized = recognizer.recognize(
			recogInfo->mpTarget->m_recognitionObject,
			recogInfo->mRecognitionParams,
			resultContour);

	if (isRecognized) {
		recogInfo->mpTarget->m_stabilizator.reset();

		pthread_spin_lock(&(recogInfo->mpTarget->m_lastLocationGuard));
		recogInfo->mpTarget->m_lastLocation = resultContour;
		pthread_spin_unlock(&(recogInfo->mpTarget->m_lastLocationGuard));

		pthread_spin_lock(&(recogInfo->mpTarget->m_stateGuard));
		recogInfo->mpTarget->m_state = ImageTrackingModel::Appeared;
		pthread_spin_unlock(&(recogInfo->mpTarget->m_stateGuard));
	} else {
		pthread_spin_lock(&(recogInfo->mpTarget->m_stateGuard));
		recogInfo->mpTarget->m_state = ImageTrackingModel::Undetected;
		pthread_spin_unlock(&(recogInfo->mpTarget->m_stateGuard));
	}

	recogInfo->mpTarget->m_recognitionThread = 0;

	pthread_mutex_unlock(&(recogInfo->mpTarget->m_globalGuard));

	delete recogInfo;

	return NULL;
}

void ImageTracker::trackUndetectedObject(
		const cv::Mat& frame,
		ImageTrackingModel& target)
{
	RecognitionInfo *recognitionInfo = new RecognitionInfo;

	recognitionInfo->mFrame = frame.clone();
	recognitionInfo->mpTarget = &target;

	recognitionInfo->mRecognitionParams =
			m_trackingParams.mRecognitionParams;
	recognitionInfo->mSceneFeaturesExtractingParams =
			m_trackingParams.mFramesFeaturesExtractingParams;

	if (target.m_recognitionThread) {
		/* Abnormal behaviour:
		 * Recognition thread isn't finished but guardian mutex is unlocked
		 */
		LOGE("[%s] Abnormal behaviour. Recognition thread isn't finished but"
					"guardian mutex is unlocked.", __FUNCTION__);

		LOGI("[%s] Try to wait recognition thread.", __FUNCTION__);
		pthread_join(target.m_recognitionThread, NULL);
		target.m_recognitionThread = 0;
		LOGI("[%s] Recognition thread is finished.", __FUNCTION__);
	}

	const int err = pthread_create(
			&target.m_recognitionThread,
			NULL,
			recognitionThreadFunc,
			recognitionInfo);

	if (0 == err) {
		LOGI("[%s] Recognition thread is started.", __FUNCTION__);
		/* Recognition thread is started. Don't use target here, just exit! */
		return;
	}
	LOGE("[%s] Recognition thread creation is failed.", __FUNCTION__);

	pthread_spin_lock(&target.m_stateGuard);
	if (target.m_recognitionObject.isEmpty()) {
		target.m_state = ImageTrackingModel::Invalid;
		LOGI("[%s] Tracking model status is changed on \"Invalid\"", __FUNCTION__);
	} else {
		target.m_state = ImageTrackingModel::Undetected;
		LOGI("[%s] Tracking model status is changed on \"Undetected\"", __FUNCTION__);
	}
	pthread_spin_unlock(&target.m_stateGuard);

	pthread_mutex_unlock(&target.m_globalGuard);
}

cv::Rect ImageTracker::computeExpectedArea(
        const ImageTrackingModel& target,
        const cv::Size& frameSize)
{
	if (target.m_state == ImageTrackingModel::Appeared) {
		LOGI("[%s] Expected area for appeared object is full frame.", __FUNCTION__);
		return cv::Rect(0, 0, frameSize.width, frameSize.height);
	}

	if (target.m_lastLocation.empty()) {
		LOGW("[%s] Can't compute expected area for object without last"
				"location.", __FUNCTION__);
		return cv::Rect(0, 0, 0, 0);
	}

	cv::Point2f ltCorner(target.m_lastLocation[0]);
	cv::Point2f rbCorner(target.m_lastLocation[0]);

	const size_t contourPointsNumber = target.m_lastLocation.size();

	for (size_t pointNum = 1; pointNum < contourPointsNumber; ++pointNum) {
		if (ltCorner.x > target.m_lastLocation[pointNum].x) {
			ltCorner.x = target.m_lastLocation[pointNum].x;
		} else if (rbCorner.x < target.m_lastLocation[pointNum].x) {
			rbCorner.x = target.m_lastLocation[pointNum].x;
		}

		if (ltCorner.y > target.m_lastLocation[pointNum].y) {
			ltCorner.y = target.m_lastLocation[pointNum].y;
		} else if (rbCorner.y < target.m_lastLocation[pointNum].y) {
			rbCorner.y = target.m_lastLocation[pointNum].y;
		}
	}

	cv::Point2f center(
			(ltCorner.x + rbCorner.x) / 2.0f,
			(ltCorner.y + rbCorner.y) / 2.0f);

	cv::Size2f halfSize(
			(center.x - ltCorner.x) * (1 + m_trackingParams.mExpectedOffset),
			(center.y - ltCorner.y) * (1 + m_trackingParams.mExpectedOffset));

	cv::Rect expectedArea(
			center.x - halfSize.width, center.y - halfSize.height,
			halfSize.width * 2, halfSize.height * 2);

	if (expectedArea.x < 0) {
		expectedArea.width += expectedArea.x;
		expectedArea.x = 0;
	}

	if (expectedArea.y < 0) {
		expectedArea.height += expectedArea.y;
		expectedArea.y = 0;
	}

	if (expectedArea.x + expectedArea.width > frameSize.width) {
		expectedArea.width = frameSize.width - expectedArea.x;
	}

	if (expectedArea.y + expectedArea.height > frameSize.height) {
		expectedArea.height = frameSize.height - expectedArea.y;
	}

	if (expectedArea.width <= 0 || expectedArea.height <= 0) {
		expectedArea.x = 0;
		expectedArea.y = 0;
		expectedArea.width = 0;
		expectedArea.height = 0;
	}

	return expectedArea;
}

} /* Image */
} /* MediaVision */
