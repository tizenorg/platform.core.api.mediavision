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

#include "Recognition/ImageObject.h"

#include "ImageMathUtil.h"

#include "Features/FeatureExtractor.h"
#include "Features/BasicExtractorFactory.h"
#include "Features/ORBExtractorFactory.h"

#include "mv_private.h"
#include "mv_common.h"

#include <opencv/cv.h>
#include <opencv2/features2d/features2d.hpp>

#include <fstream>
#include <unistd.h>

namespace MediaVision {
namespace Image {
ImageObject::ImageObject() :
		m_features(),
		m_isEmpty(true),
		m_isLabeled(false),
		m_label(0)
{
	; /* NULL */
}

ImageObject::ImageObject(const cv::Mat& image, const FeaturesExtractingParams& params) :
		m_featureExtractingParams(),
		m_features(),
		m_isEmpty(true),
		m_isLabeled(false),
		m_label(0)
{
	fill(image, params);
}

ImageObject::ImageObject(const ImageObject& copy) :
		m_featureExtractingParams(copy.m_featureExtractingParams),
		m_features(copy.m_features),
		m_isEmpty(copy.m_isEmpty),
		m_isLabeled(copy.m_isLabeled),
		m_label(copy.m_label),
		m_boundingContour(copy.m_boundingContour)
{
	; /* NULL */
}

ImageObject& ImageObject::operator=(const ImageObject& copy)
{
	if (this != &copy) {
		m_isEmpty = copy.m_isEmpty;
		m_isLabeled = copy.m_isLabeled;
		m_label = copy.m_label;
		m_boundingContour = copy.m_boundingContour;

		m_features = copy.m_features;
	}

	return *this;
}

ImageObject::~ImageObject()
{
	; /* NULL */
}

void ImageObject::fill(
		const cv::Mat& image,
		const FeaturesExtractingParams& params,
		const std::vector<cv::Point2f>& roi)
{
	m_isEmpty = false;

	if (!roi.empty()) {
		m_boundingContour = roi;
	} else {
		m_boundingContour.resize(NumberOfQuadrangleCorners);

		m_boundingContour[0].x = 0.f;
		m_boundingContour[0].y = 0.f;

		m_boundingContour[1].x = image.cols;
		m_boundingContour[1].y = 0.f;

		m_boundingContour[2].x = image.cols;
		m_boundingContour[2].y = image.rows;

		m_boundingContour[3].x = 0.f;
		m_boundingContour[3].y = image.rows;
	}

	extractFeatures(image, params, m_boundingContour);

	m_featureExtractingParams = params;

	LOGI("[%s] Image object is filled.", __FUNCTION__);
}

float ImageObject::getRecognitionRate(void) const
{
	return m_features.m_recognitionRate;
}

void ImageObject::extractFeatures(
		const cv::Mat& image,
		const FeaturesExtractingParams& params,
		const std::vector<cv::Point2f>& roi)
{
	/* TODO: It is advisable to consider the distribution of functional */

	cv::Ptr<FeatureExtractor> extractor;

	if (params.mKeypointType == KT_ORB &&
			params.mDescriptorType == DT_ORB) {
		ORBExtractorFactory extractorFactory;

		extractorFactory.setScaleFactor((float)params.ORB.mScaleFactor);
		extractorFactory.setMaximumFeaturesNumber(params.ORB.mMaximumFeaturesNumber);

		extractor = extractorFactory.buildFeatureExtractor();
	} else {
		BasicExtractorFactory extractorFactory(
				params.mKeypointType,
				params.mDescriptorType);

		extractor = extractorFactory.buildFeatureExtractor();
	}

	if (!extractor.empty())
		extractor->extract(image, m_features, roi);
}

bool ImageObject::isEmpty() const
{
	return (m_features.m_objectKeypoints.empty() ||
				m_features.m_objectDescriptors.empty());
}

void ImageObject::setContour(const std::vector<cv::Point2f>& contour)
{
	m_boundingContour = contour;
}

void ImageObject::setLabel(int label)
{
	m_isLabeled = true;
	m_label = label;
}

bool ImageObject::getLabel(int& label) const
{
	if (!m_isLabeled) {
		LOGW("[%s] Image hasn't label.", __FUNCTION__);
		return false;
	}
	label = m_label;
	return true;
}

int ImageObject::save(const char *fileName) const
{
	std::string filePath;

	filePath = fileName;

	std::string prefixPath = filePath.substr(0, filePath.find_last_of('/'));
	LOGD("prefixPath: %s", prefixPath.c_str());

	/* check the directory is available */
	if (access(prefixPath.c_str(), F_OK)) {
		LOGE("Can't save image object. Path[%s] doesn't existed.", filePath.c_str());

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
	LOGI("[%s] Image object is saved.", __FUNCTION__);

	return MEDIA_VISION_ERROR_NONE;
}

int ImageObject::load(const char *fileName)
{
	std::string filePath;

	filePath = fileName;

	if (access(filePath.c_str(), F_OK)) {
		LOGE("Can't load image object model. Path[%s] doesn't existed.", filePath.c_str());

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
		/* TODO: Provide another error code */
		LOGE("[%s] Unexpected end of file.", __FUNCTION__);
		return MEDIA_VISION_ERROR_PERMISSION_DENIED;
	}

	in.close();
	LOGI("[%s] Image object is loaded.", __FUNCTION__);

	return MEDIA_VISION_ERROR_NONE;
}

std::ostream& operator << (std::ostream& os, const ImageObject& obj)
{
	os<<std::setprecision(7);

	os<<obj.m_isEmpty<<'\n';
	os<<obj.m_isLabeled<<'\n';
	os<<obj.m_label<<'\n';

	os<<obj.m_boundingContour.size()<<'\n';
	for (size_t pointNum = 0u; pointNum < obj.m_boundingContour.size(); ++pointNum) {
		os<<obj.m_boundingContour[pointNum].x<<' ';
		os<<obj.m_boundingContour[pointNum].y<<'\n';
	}

	const size_t numberOfKeypoints = obj.m_features.m_objectKeypoints.size();
	os<<numberOfKeypoints<<'\n';
	for (size_t keypointNum = 0u; keypointNum < numberOfKeypoints; ++keypointNum) {
		os<<obj.m_features.m_objectKeypoints[keypointNum].pt.x<<' ';
		os<<obj.m_features.m_objectKeypoints[keypointNum].pt.y<<' ';
		os<<obj.m_features.m_objectKeypoints[keypointNum].size<<' ';
		os<<obj.m_features.m_objectKeypoints[keypointNum].response<<' ';
		os<<obj.m_features.m_objectKeypoints[keypointNum].angle<<' ';
		os<<obj.m_features.m_objectKeypoints[keypointNum].octave<<' ';
		os<<obj.m_features.m_objectKeypoints[keypointNum].class_id<<'\n';
	}

	const int numberOfDescriptors = obj.m_features.m_objectDescriptors.rows;
	const int sizeOfDescriptor = obj.m_features.m_objectDescriptors.cols;

	os<<numberOfDescriptors<<' ';
	os<<sizeOfDescriptor<<' ';
	os<<obj.m_features.m_objectDescriptors.type()<<'\n';

	for (int descriptorNum = 0; descriptorNum < numberOfDescriptors;
				++descriptorNum, os<<'\n') {
		for (int featureNum = 0; featureNum < sizeOfDescriptor;
					++featureNum) {
				os<<(int)obj.m_features.m_objectDescriptors.at<uchar>(
				descriptorNum,
				featureNum)<<' ';
		}
	}

	return os;
}

std::istream& operator >> (std::istream& is, ImageObject& obj)
{
	size_t numberOfContourPoints = 0u;
	size_t numberOfKeypoints = 0u;
	int rows = 0, cols = 0;
	int descriptorType = 0;

	ImageObject temporal;

#define MEDIA_VISION_CHECK_IFSTREAM \
	if (!is.good()) { \
		return is; \
	}

	is>>temporal.m_isEmpty;
	MEDIA_VISION_CHECK_IFSTREAM
	is>>temporal.m_isLabeled;
	MEDIA_VISION_CHECK_IFSTREAM
	is>>temporal.m_label;
	MEDIA_VISION_CHECK_IFSTREAM

	is>>numberOfContourPoints;
	MEDIA_VISION_CHECK_IFSTREAM

	temporal.m_boundingContour.resize(numberOfContourPoints);
	for (size_t pointNum = 0; pointNum < numberOfContourPoints; ++pointNum) {
		is>>temporal.m_boundingContour[pointNum].x;
		MEDIA_VISION_CHECK_IFSTREAM
		is>>temporal.m_boundingContour[pointNum].y;
		MEDIA_VISION_CHECK_IFSTREAM
	}

	is>>numberOfKeypoints;
	temporal.m_features.m_objectKeypoints.resize(numberOfKeypoints);
	for (size_t keypointNum = 0; keypointNum < numberOfKeypoints; ++keypointNum) {
		is>>temporal.m_features.m_objectKeypoints[keypointNum].pt.x;
		MEDIA_VISION_CHECK_IFSTREAM
		is>>temporal.m_features.m_objectKeypoints[keypointNum].pt.y;
		MEDIA_VISION_CHECK_IFSTREAM
		is>>temporal.m_features.m_objectKeypoints[keypointNum].size;
		MEDIA_VISION_CHECK_IFSTREAM
		is>>temporal.m_features.m_objectKeypoints[keypointNum].response;
		MEDIA_VISION_CHECK_IFSTREAM
		is>>temporal.m_features.m_objectKeypoints[keypointNum].angle;
		MEDIA_VISION_CHECK_IFSTREAM
		is>>temporal.m_features.m_objectKeypoints[keypointNum].octave;
		MEDIA_VISION_CHECK_IFSTREAM
		is>>temporal.m_features.m_objectKeypoints[keypointNum].class_id;
		MEDIA_VISION_CHECK_IFSTREAM
	}

	is>>rows;
	MEDIA_VISION_CHECK_IFSTREAM
	is>>cols;
	MEDIA_VISION_CHECK_IFSTREAM
	is>>descriptorType;
	MEDIA_VISION_CHECK_IFSTREAM
	temporal.m_features.m_objectDescriptors = cv::Mat(rows, cols, descriptorType);
	int value = 0;
	for (int descriptorNum = 0; descriptorNum < rows; ++descriptorNum) {
		for (int featureNum = 0; featureNum < cols; ++featureNum) {
			is>>value;
			MEDIA_VISION_CHECK_IFSTREAM

			temporal.m_features.m_objectDescriptors.at<uchar>(descriptorNum, featureNum) =
					(uchar)value;
		}
	}

#undef MEDIA_VISION_CHECK_IFSTREAM

	obj = temporal;

	return is;
}

} /* Image */
} /* MediaVision */
