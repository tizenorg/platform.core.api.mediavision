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

#include "Tracking/ImageTrackingModel.h"

#include "Tracking/CascadeTracker.h"
#include "Tracking/RecognitionBasedTracker.h"
#include "Tracking/FeatureSubstitutionTracker.h"
#include "Tracking/AsyncTracker.h"
#include "Tracking/MFTracker.h"

#include "mv_private.h"
#include "mv_common.h"

#include <fstream>
#include <unistd.h>

namespace MediaVision {
namespace Image {
ImageTrackingModel::ImageTrackingModel() :
		m_target(),
		m_tracker(),
		m_stabilizator(),
		m_location(),
		m_stabilizationParams()
{
	; /* NULL */
}

ImageTrackingModel::ImageTrackingModel(const ImageTrackingModel& copy) :
		m_target(copy.m_target),
		m_tracker(copy.m_tracker.empty()? NULL: copy.m_tracker->clone()),
		m_stabilizator(copy.m_stabilizator),
		m_location(copy.m_location),
		m_stabilizationParams(copy.m_stabilizationParams)
{
	; /* NULL */
}

void ImageTrackingModel::setTarget(const ImageObject& target)
{
	/* TODO: Here are all the settings.
	 *        This can be transferred to configuration file.
	 *
	 * Parameters of recognition based tracker
	 */

	FeaturesExtractingParams orbFeatureExtractingParams;

	orbFeatureExtractingParams.mKeypointType = KT_ORB;
	orbFeatureExtractingParams.mDescriptorType = DT_ORB;
	orbFeatureExtractingParams.ORB.mMaximumFeaturesNumber = 5000;
	orbFeatureExtractingParams.ORB.mScaleFactor = 1.15;

	RecognitionParams orbRecogParams;

	orbRecogParams.mMinMatchesNumber = 70;
	orbRecogParams.mRequiredMatchesPart = 0.005;
	orbRecogParams.mTolerantMatchesPartError = 0.1;

	/* Parameters of feature substitution tracker */

	FeaturesExtractingParams gfttWbriefFeatureExtractingParams;

	gfttWbriefFeatureExtractingParams.mKeypointType = KT_GFTT;
	gfttWbriefFeatureExtractingParams.mDescriptorType = DT_BRIEF;

	RecognitionParams gfttWbriefRecogParams;

	gfttWbriefRecogParams.mMinMatchesNumber = 30;
	gfttWbriefRecogParams.mRequiredMatchesPart = 0.05;
	gfttWbriefRecogParams.mTolerantMatchesPartError = 0.1;

	const float expectedOffset = 1.0f;

	/* Parameters of median flow tracker */

	MFTracker::Params medianflowTrackingParams;

	medianflowTrackingParams.mPointsInGrid = 10;
	medianflowTrackingParams.mWindowSize = cv::Size(16, 16);
	medianflowTrackingParams.mPyrMaxLevel = 16;

	/* Parameters of cascade tracker */

	const float recognitionBasedTrackerPriotity = 1.0f;
	const float featureSubstitutionTrackerPriotity = 0.6f;
	const float medianFlowTrackerPriotity = 0.1f;

	/* Parameters of stabilization */

	m_stabilizationParams.mIsEnabled = true;
	m_stabilizationParams.mHistoryAmount = 3;
	m_stabilizationParams.mTolerantShift = 0.00006;
	m_stabilizationParams.mTolerantShiftExtra = 1.3;
	m_stabilizationParams.mStabilizationSpeed = 0.3;
	m_stabilizationParams.mStabilizationAcceleration = 0.1;

	/* Parameters definition is finished */

	/* Creating a basic tracker which will have other trackers */

	cv::Ptr<CascadeTracker> mainTracker = new CascadeTracker;

	/* Adding asynchronous recognition based tracker */

	cv::Ptr<RecognitionBasedTracker> recogTracker =
			new RecognitionBasedTracker(
					target,
					orbFeatureExtractingParams,
					orbRecogParams);

	cv::Ptr<AsyncTracker> asyncRecogTracker =
			new AsyncTracker(
					recogTracker,
					true);

	mainTracker->enableTracker(
			asyncRecogTracker,
			recognitionBasedTrackerPriotity);

	/* Adding asynchronous feature substitution based tracker */

	cv::Ptr<FeatureSubstitutionTracker> substitutionTracker =
			new FeatureSubstitutionTracker(
					gfttWbriefFeatureExtractingParams,
					gfttWbriefRecogParams,
					expectedOffset);

	cv::Ptr<AsyncTracker> asyncSubstitutionTracker =
			new AsyncTracker(
					substitutionTracker,
					true);

	mainTracker->enableTracker(
			asyncSubstitutionTracker,
			featureSubstitutionTrackerPriotity);

	/* Adding median flow tracker */

	cv::Ptr<MFTracker> mfTracker = new MFTracker(medianflowTrackingParams);

	mainTracker->enableTracker(
			mfTracker,
			medianFlowTrackerPriotity);

	m_tracker = mainTracker;
	m_target = target;
}

bool ImageTrackingModel::isValid() const
{
	return !(m_target.isEmpty());
}

bool ImageTrackingModel::track(const cv::Mat& frame, std::vector<cv::Point>& result)
{
	result.clear();

	if (m_tracker.empty())
		return false;

	if (!(m_tracker->track(frame, m_location))) {
		m_stabilizator.reset();
		return false;
	}

	const size_t numberOfContourPoints = m_location.size();
	std::vector<cv::Point2f> stabilizedContour(numberOfContourPoints);
	for (size_t i = 0; i < numberOfContourPoints; ++i) {
		stabilizedContour[i].x = (float)m_location[i].x;
		stabilizedContour[i].y = (float)m_location[i].y;
	}

	m_stabilizator.stabilize(stabilizedContour, m_stabilizationParams);
	for (size_t i = 0; i < numberOfContourPoints; ++i) {
		m_location[i].x = (int)stabilizedContour[i].x;
		m_location[i].y = (int)stabilizedContour[i].y;
	}

	result = m_location;

	return true;
}

void ImageTrackingModel::refresh(void)
{
	m_location.clear();
}

ImageTrackingModel& ImageTrackingModel::operator=(const ImageTrackingModel& copy)
{
	if (this != &copy) {
		m_target = copy.m_target;
		if (!copy.m_tracker.empty())
			m_tracker = copy.m_tracker->clone();
		else
			m_tracker.release();

		m_stabilizator = copy.m_stabilizator;
		m_location = copy.m_location;
		m_stabilizationParams = copy.m_stabilizationParams;
	}

	return *this;
}

int ImageTrackingModel::save(const char *filepath) const
{
	std::string filePath;

	filePath = std::string(filepath);

	std::string prefixPath = filePath.substr(0, filePath.find_last_of('/'));
	LOGD("prefixPath: %s", prefixPath.c_str());

	/* check the directory is available */
	if (access(prefixPath.c_str(),F_OK)) {
		LOGE("Can't save tracking model. Path[%s] doesn't existed.", filePath.c_str());

		return MEDIA_VISION_ERROR_INVALID_PATH;
	}

	std::ofstream out;
	out.open(filePath.c_str());

	if (!out.is_open()) {
		LOGE("[%s] Can't create/open file.", __FUNCTION__);
		return MEDIA_VISION_ERROR_PERMISSION_DENIED;
	}

	out<<(*this);

	out.close();
	LOGI("[%s] Image tracking model is saved.", __FUNCTION__);

	return MEDIA_VISION_ERROR_NONE;
}

int ImageTrackingModel::load(const char *filepath)
{
	std::string filePath;

	filePath = std::string(filepath);

	if (access(filePath.c_str(),F_OK)) {
		LOGE("Can't load tracking model. Path[%s] doesn't existed.", filepath);

		return MEDIA_VISION_ERROR_INVALID_PATH;
	}

	std::ifstream in;
	in.open(filePath.c_str());

	if (!in.is_open()) {
		LOGE("[%s] Can't open file.", __FUNCTION__);
		return MEDIA_VISION_ERROR_PERMISSION_DENIED;
	}

	in>>(*this);

	if (!in.good()) {
		LOGE("[%s] Unexpected end of file.", __FUNCTION__);
		return MEDIA_VISION_ERROR_PERMISSION_DENIED;
	}

	in.close();
	LOGI("[%s] Image tracking model is loaded.", __FUNCTION__);

	return MEDIA_VISION_ERROR_NONE;
}

std::ostream& operator << (std::ostream& os, const ImageTrackingModel& obj)
{
	os<<std::setprecision(7);

	os<<obj.m_target;
	os<<obj.m_stabilizationParams.mIsEnabled<<'\n';
	os<<obj.m_stabilizationParams.mHistoryAmount<<'\n';
	os<<obj.m_stabilizationParams.mStabilizationSpeed<<'\n';
	os<<obj.m_stabilizationParams.mStabilizationAcceleration<<'\n';
	os<<obj.m_stabilizationParams.mTolerantShift<<'\n';
	os<<obj.m_stabilizationParams.mTolerantShiftExtra<<'\n';

	const size_t numberOfContourPoints = obj.m_location.size();
	os<<numberOfContourPoints<<'\n';
	for (size_t pointNum = 0u; pointNum < numberOfContourPoints; ++pointNum)
		os<<' '<<obj.m_location[pointNum].x<<' '<<obj.m_location[pointNum].y;

	os<<'\n';

	return os;
}

std::istream& operator >> (std::istream& is, ImageTrackingModel& obj)
{
#define MEDIA_VISION_CHECK_IFSTREAM \
	if (!is.good()) { \
		return is; \
	}

	ImageObject target;
	std::vector<cv::Point> location;

	is>>target;
	MEDIA_VISION_CHECK_IFSTREAM

	StabilizationParams params;
	is>>params.mIsEnabled;
	is>>params.mHistoryAmount;
	is>>params.mStabilizationSpeed;
	is>>params.mStabilizationAcceleration;
	is>>params.mTolerantShift;
	is>>params.mTolerantShiftExtra;

	size_t numberOfContourPoints = 0u;
	is>>numberOfContourPoints;
	MEDIA_VISION_CHECK_IFSTREAM

	location.resize(numberOfContourPoints);
	for (size_t pointNum = 0u; pointNum < numberOfContourPoints; ++pointNum) {
		is>>location[pointNum].x;
		MEDIA_VISION_CHECK_IFSTREAM
		is>>location[pointNum].y;
		MEDIA_VISION_CHECK_IFSTREAM
	}

#undef MEDIA_VISION_CHECK_IFSTREAM

	obj.m_stabilizationParams = params;
	obj.m_location = location;
	if (!(target.isEmpty())) {
		obj.setTarget(target);
		obj.m_tracker->reinforcement(location);
	}

	return is;
}

} /* Image */
} /* MediaVision */
