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

#include "ImageObject.h"

#include "ImageMathUtil.h"

#include <app_common.h>

#include "mv_private.h"
#include "mv_common.h"

#include <opencv/cv.h>
#include <opencv2/features2d/features2d.hpp>

#include <fstream>
#include <unistd.h>

namespace MediaVision {
namespace Image {
ImageObject::ImageObject() :
	m_isEmpty(true),
	m_isLabeled(false),
	m_label(0),
	m_recognitionRate(0.f)
{
	; /* NULL */
}

ImageObject::ImageObject(const cv::Mat& image, const FeaturesExtractingParams& params) :
	m_isEmpty(true),
	m_isLabeled(false),
	m_label(0),
	m_recognitionRate(0.f)
{
	fill(image, params);
}

ImageObject::ImageObject(const ImageObject& copy) :
	m_isEmpty(copy.m_isEmpty),
	m_isLabeled(copy.m_isLabeled),
	m_label(copy.m_label),
	m_boundingContour(copy.m_boundingContour),
	m_objectKeypoints(copy.m_objectKeypoints),
	m_objectDescriptors(copy.m_objectDescriptors.clone()),
	m_recognitionRate(copy.m_recognitionRate)
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
		m_objectKeypoints = copy.m_objectKeypoints;
		m_objectDescriptors = copy.m_objectDescriptors.clone();
		m_recognitionRate = copy.m_recognitionRate;
	}
	return *this;
}

ImageObject::~ImageObject()
{
	; /* NULL */
}

void ImageObject::fill(const cv::Mat& image, const FeaturesExtractingParams& params)
{
	m_isEmpty = false;
	m_boundingContour.resize(NumberOfQuadrangleCorners);

	m_boundingContour[0].x = 0.f;
	m_boundingContour[0].y = 0.f;

	m_boundingContour[1].x = image.cols;
	m_boundingContour[1].y = 0.f;

	m_boundingContour[2].x = image.cols;
	m_boundingContour[2].y = image.rows;

	m_boundingContour[3].x = 0.f;
	m_boundingContour[3].y = image.rows;

	extractFeatures(image, params);

	computeRecognitionRate(image);

	LOGI("[%s] Image object is filled.", __FUNCTION__);
}

bool ImageObject::fill(const cv::Mat& image, const cv::Rect& boundingBox,
		const FeaturesExtractingParams& params)
{
	if ((0 > boundingBox.x) || (0 >= boundingBox.width) ||
		(0 > boundingBox.y) || (0 >= boundingBox.height) ||
		(image.cols < (boundingBox.x + boundingBox.width)) ||
		(image.rows < (boundingBox.y + boundingBox.height))) {
			LOGE("[%s] Invalid ROI.", __FUNCTION__);
			return false;
	}

	m_isEmpty = false;
	m_boundingContour.resize(NumberOfQuadrangleCorners);

	m_boundingContour[0].x = 0.f;
	m_boundingContour[0].y = 0.f;

	m_boundingContour[1].x = boundingBox.width;
	m_boundingContour[1].y = 0.f;

	m_boundingContour[2].x = boundingBox.width;
	m_boundingContour[2].y = boundingBox.height;

	m_boundingContour[3].x = 0.f;
	m_boundingContour[3].y = boundingBox.height;

	cv::Mat objectImage(image, boundingBox);

	extractFeatures(objectImage, params);

	computeRecognitionRate(image);

	LOGI("[%s] Image object is filled.", __FUNCTION__);

	return true;
}

void ImageObject::extractFeatures(const cv::Mat& image,
		const FeaturesExtractingParams& params)
{
	cv::ORB orb(params.mMaximumFeaturesNumber, params.mScaleFactor);

	if (image.cols < MinWidth || image.rows < MinHeight) {
		LOGW("[%s] Area is too small, recognition rate is 0.", __FUNCTION__);
		m_objectKeypoints.clear();
		m_objectDescriptors = cv::Mat();
	} else {
		orb.detect(image, m_objectKeypoints);
		orb.compute(image, m_objectKeypoints, m_objectDescriptors);
	}
}

void ImageObject::computeRecognitionRate(const cv::Mat& image)
{
	const size_t numberOfKeypoints = m_objectKeypoints.size();

	/* it is impossible to calculate the perspective transformation parameters
	 * if number of key points less than MinimumNumberOfFeatures (4)
	 */
	if (numberOfKeypoints < MinimumNumberOfFeatures) {
		m_recognitionRate = 0.f;
		return;
	}

	static const size_t xCellsNumber = 10u;
	static const size_t yCellsNumber = 10u;

	cv::Mat cells[xCellsNumber][yCellsNumber];
	size_t accumulationCounter[xCellsNumber][yCellsNumber];

	const size_t cellWidth = image.cols / xCellsNumber;
	const size_t cellHeight = image.rows / yCellsNumber;

	for (size_t x = 0u; x < xCellsNumber; ++x) {
		for (size_t y = 0u; y < yCellsNumber; ++y) {
			cells[x][y] = image(cv::Rect(
							x * cellWidth,
							y * cellHeight,
							cellWidth,
							cellHeight));

			accumulationCounter[x][y] = 0;
		}
	}

	for (size_t i = 0u; i < numberOfKeypoints; ++i) {
		size_t xCellIdx = m_objectKeypoints[i].pt.x / cellWidth;
		if (xCellIdx >= xCellsNumber) {
			xCellIdx = xCellsNumber - 1;
		}
		size_t yCellIdx = m_objectKeypoints[i].pt.y / cellHeight;
		if (yCellIdx >= yCellsNumber) {
			yCellIdx = yCellsNumber - 1;
		}
		++(accumulationCounter[xCellIdx][yCellIdx]);
	}

		const float exceptedNumber = numberOfKeypoints /
			(float)(xCellsNumber * yCellsNumber);

	float distributedEvaluation = 0.f;

	for (size_t x = 0u; x < xCellsNumber; ++x) {
		for (size_t y = 0u; y < yCellsNumber; ++y) {
			distributedEvaluation += (accumulationCounter[x][y] - exceptedNumber) *
				(accumulationCounter[x][y] - exceptedNumber) / exceptedNumber;
		}
	}

	float maximumDistributedEvaluation = (xCellsNumber * yCellsNumber - 1) *
			exceptedNumber;

	maximumDistributedEvaluation += (numberOfKeypoints - exceptedNumber) *
			(numberOfKeypoints - exceptedNumber) / exceptedNumber;

	distributedEvaluation = 1 -
			(distributedEvaluation / maximumDistributedEvaluation);

	/* Exponentiation to find an approximate confidence value based on the
	 * number of key points on the image.
	 */
	const float cardinalityEvaluation = pow(-0.9, numberOfKeypoints - 3) + 1.0f;

	m_recognitionRate =
			distributedEvaluation *
			cardinalityEvaluation;
}

float ImageObject::getRecognitionRate(void) const
{
	return m_recognitionRate;
}

bool ImageObject::isEmpty() const
{
	return m_isEmpty;
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
	std::string prefix_path = std::string(app_get_data_path());
	LOGD("prefix_path: %s", prefix_path.c_str());

	std::string filePath;
	filePath += prefix_path;
	filePath += fileName;

	/* check the directory is available */
	std::string prefix_path_check = filePath.substr(0, filePath.find_last_of('/'));
	if (access(prefix_path_check.c_str(), F_OK)) {
		LOGE("Can't save image object. Path[%s] doesn't existed.", prefix_path_check.c_str());

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
	LOGI("[%s] Image object is saved.", __FUNCTION__);

	return MEDIA_VISION_ERROR_NONE;
}

int ImageObject::load(const char *fileName)
{
	/* find directory */
	std::string prefix_path = std::string(app_get_data_path());
	LOGD("prefix_path: %s", prefix_path.c_str());

	std::string filePath;
	filePath += prefix_path;
	filePath += fileName;

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

	in >> (*this);

	if (!in.good()) {
		LOGE("[%s] Unexpected end of file.", __FUNCTION__);
		return MEDIA_VISION_ERROR_PERMISSION_DENIED;
	}

	in.close();
	LOGI("[%s] Image object is loaded.", __FUNCTION__);

	return MEDIA_VISION_ERROR_NONE;
}

std::ostream& operator << (std::ostream& os, const ImageObject& obj)
{
	os << std::setprecision(7);

	os << obj.m_isEmpty << '\n';
	os << obj.m_isLabeled << '\n';
	os << obj.m_label << '\n';

	os << obj.m_boundingContour.size() << '\n';
	for (size_t pointNum = 0u; pointNum < obj.m_boundingContour.size(); ++pointNum) {
		os << obj.m_boundingContour[pointNum].x << ' ';
		os << obj.m_boundingContour[pointNum].y << '\n';
	}

	os << obj.m_objectKeypoints.size() << '\n';
	for (size_t keypointNum = 0u; keypointNum < obj.m_objectKeypoints.size(); ++keypointNum) {
		os << obj.m_objectKeypoints[keypointNum].pt.x << ' ';
		os << obj.m_objectKeypoints[keypointNum].pt.y << ' ';
		os << obj.m_objectKeypoints[keypointNum].size << ' ';
		os << obj.m_objectKeypoints[keypointNum].response << ' ';
		os << obj.m_objectKeypoints[keypointNum].angle << ' ';
		os << obj.m_objectKeypoints[keypointNum].octave << ' ';
		os << obj.m_objectKeypoints[keypointNum].class_id << '\n';
	}

	os << obj.m_objectDescriptors.rows << ' ';
	os << obj.m_objectDescriptors.cols << ' ';
	os << obj.m_objectDescriptors.type() << '\n';
	for (int descriptorNum = 0; descriptorNum < obj.m_objectDescriptors.rows;
		++descriptorNum) {
		for (int featureNum = 0; featureNum < obj.m_objectDescriptors.cols;
			++featureNum, os << '\n') {
			os << (int)obj.m_objectDescriptors.at<uchar>(descriptorNum, featureNum) << ' ';
		}
	}

	return os;
}

std::istream& operator >> (std::istream& is, ImageObject& obj)
{
	size_t numberOfContourPoints = 0u;
	size_t numberOfKeyPoints = 0u;
	int rows = 0, cols = 0;
	int descriptorType = 0;

	ImageObject temporal;

#define MEDIA_VISION_CHECK_IFSTREAM \
	if (!is.good()) { \
		return is; \
	}

	is >> temporal.m_isEmpty;
	MEDIA_VISION_CHECK_IFSTREAM
	is >> temporal.m_isLabeled;
	MEDIA_VISION_CHECK_IFSTREAM
	is >> temporal.m_label;
	MEDIA_VISION_CHECK_IFSTREAM

	is >> numberOfContourPoints;
	MEDIA_VISION_CHECK_IFSTREAM

	temporal.m_boundingContour.resize(numberOfContourPoints);
	for (size_t pointNum = 0; pointNum < temporal.m_boundingContour.size(); ++pointNum) {
		is >> temporal.m_boundingContour[pointNum].x;
		MEDIA_VISION_CHECK_IFSTREAM
		is >> temporal.m_boundingContour[pointNum].y;
		MEDIA_VISION_CHECK_IFSTREAM
	}

	is >> numberOfKeyPoints;
	temporal.m_objectKeypoints.resize(numberOfKeyPoints);
	for (size_t keypointNum = 0; keypointNum < temporal.m_objectKeypoints.size(); ++keypointNum) {
		is >> temporal.m_objectKeypoints[keypointNum].pt.x;
		MEDIA_VISION_CHECK_IFSTREAM
		is >> temporal.m_objectKeypoints[keypointNum].pt.y;
		MEDIA_VISION_CHECK_IFSTREAM
		is >> temporal.m_objectKeypoints[keypointNum].size;
		MEDIA_VISION_CHECK_IFSTREAM
		is >> temporal.m_objectKeypoints[keypointNum].response;
		MEDIA_VISION_CHECK_IFSTREAM
		is >> temporal.m_objectKeypoints[keypointNum].angle;
		MEDIA_VISION_CHECK_IFSTREAM
		is >> temporal.m_objectKeypoints[keypointNum].octave;
		MEDIA_VISION_CHECK_IFSTREAM
		is >> temporal.m_objectKeypoints[keypointNum].class_id;
		MEDIA_VISION_CHECK_IFSTREAM
	}

	is >> rows;
	MEDIA_VISION_CHECK_IFSTREAM
	is >> cols;
	MEDIA_VISION_CHECK_IFSTREAM
	is >> descriptorType;
	MEDIA_VISION_CHECK_IFSTREAM
	temporal.m_objectDescriptors = cv::Mat(rows, cols, descriptorType);
	int value = 0;
	for (int descriptorNum = 0; descriptorNum < temporal.m_objectDescriptors.rows; ++descriptorNum) {
		for (int featureNum = 0; featureNum < temporal.m_objectDescriptors.cols; ++featureNum) {
			is >> value;
			MEDIA_VISION_CHECK_IFSTREAM
			temporal.m_objectDescriptors.at<uchar>(descriptorNum, featureNum) = (uchar)value;
		}
	}

#undef MEDIA_VISION_CHECK_IFSTREAM

	obj = temporal;

	return is;
}

} /* Image */
} /* MediaVision */
