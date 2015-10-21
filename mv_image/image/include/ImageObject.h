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

#ifndef __IMAGEOBJECT_H__
#define __IMAGEOBJECT_H__

#include "ImageConfig.h"

#include <opencv/cv.h>

/**
 * @file  ImageObject.h
 * @brief This file contains the @ref ImageObject class.
 */

namespace MediaVision {
namespace Image {
/**
 * @class    ImageObject
 * @brief    This class contains the image information, which will
 *           be used in recognition algorithms.
 *
 * @since_tizen 3.0
 */
class ImageObject {
public:
	/**
	 * @brief   @ref ImageObject default constructor.
	 *
	 * @since_tizen 3.0
	 */
	ImageObject();

	/**
	 * @brief   @ref ImageObject constructor based on image.
	 *
	 * @since_tizen 3.0
	 * @remarks Detects keypoints and extracts features from image and creates
	 *          new @ref ImageObject
	 * @param [in] image    The image for which instance of @ref ImageObject
	 *                      will be created
	 * @param [in] params   Features extracting parameters
 */
	ImageObject(const cv::Mat& image, const FeaturesExtractingParams& params);

	/**
	 * @brief   @ref ImageObject copy constructor.
	 * @details Creates copy of @ref ImageObject
	 *
	 * @since_tizen 3.0
	 * @param   [in] copy @ref ImageObject which will be copied
	 */
	ImageObject(const ImageObject& copy);

	/**
	 * @brief   @ref ImageObject copy assignment operator.
	 * @details Fills the information based on the @a copy
	 *
	 * @since_tizen 3.0
	 * @param   [in] copy @ref ImageObject which will be copied
	 *
	 */
	ImageObject& operator=(const ImageObject& copy);

	/**
	 * @brief   @ref ImageObject destructor.
	 *
	 * @since_tizen 3.0
	 */
	virtual ~ImageObject();

	/**
	 * @brief   Fills @ref ImageObject class based on image.
	 * @details Detects keypoints and extracts features from image and creates
	 *          new @ref ImageObject
	 *
	 * @since_tizen 3.0
	 * @param [in] image    The image for which instance of @ref ImageObject
	 *                      will be created
	 * @param [in] params   Features extracting parameters
	 */
	void fill(const cv::Mat& image, const FeaturesExtractingParams& params);

	/**
	 * @brief   Fills @ref ImageObject class based on image.
	 * @details Detects keypoints and extracts features from image and creates
	 *          new @ref ImageObject
	 *
	 * @since_tizen 3.0
	 * @param [in] image         The image for which instance of @ref
	 *                           ImageObject will be created
	 * @param [in] boundingBox   Bounding box of the object being analyzed in
	 *                           the @a image
	 * @param [in] params        Features extracting parameters
	 * @return @a true on success, otherwise a @a false value
	 * @retval true  Successful
	 * @retval false Invalid ROI (bounding box)
	 */
	bool fill(
			const cv::Mat& image,
			const cv::Rect& boundingBox,
			const FeaturesExtractingParams& params);

	/**
	 * @brief Gets a value that determines how well an @ref ImageObject can be recognized.
	 * @details Confidence can be from 0 to 1. If the recognition rate is 0 object can
	 *          not be recognized
	 *
	 * @since_tizen 3.0
	 * @return A value that determines how well an @ref ImageObject can be recognized.
	 */
	float getRecognitionRate(void) const;

	/**
	 * @brief Check whether the object is filled.
	 * @details Image object is empty if it wasn't filled.
	 *
	 * @since_tizen 3.0
	 * @remarks Empty object can not be recognized or tracked. Fill the object
	 *          by using corresponding constructor or function @ref fill() to
	 *          make image object valid. Also you can load image object which is
	 *          not empty by using @ref load().
	 * @return @c false if object is filled, otherwise return @c true
	 */
	bool isEmpty() const;

	/**
	 * @brief Sets a label for the image object.
	 *
	 * @since_tizen 3.0
	 * @param [in] label   The label which will be assigned to the image object
	 */
	void setLabel(int label);

	/**
	 * @brief Gets a label of object.
	 *
	 * @since_tizen 3.0
	 * @param [out] label   The label of image object
	 * @return @c true if object is labeled, otherwise return @c false
	 */
	bool getLabel(int& label) const;

	/**
	 * @brief  Stores the @ref ImageObject in a file.
	 *
	 * @since_tizen 3.0
	 * @param  [in] fileName  File name which will be generated
	 * @return @a 0 on success, otherwise a negative error value
	 */
	int save(const char *fileName) const;

	/**
	 * @brief  Loads the @ref ImageObject from the file.
	 *
	 * @since_tizen 3.0
	 * @param  [in] fileName  File name from which will be loaded an @ref ImageObject
	 * @return @a 0 on success, otherwise a negative error value
	 */
	int load(const char *fileName);

private:
	static const int MinWidth = 5;
	static const int MinHeight = 5;

private:
	void extractFeatures(
			const cv::Mat& image,
			const FeaturesExtractingParams& params);

	void computeRecognitionRate(const cv::Mat& image);

private:
	bool m_isEmpty;

	bool m_isLabeled;

	int m_label;

	std::vector<cv::Point2f> m_boundingContour;

	std::vector<cv::KeyPoint> m_objectKeypoints;

	cv::Mat m_objectDescriptors;

	float m_recognitionRate;

	friend class ImageRecognizer;

	friend std::ostream& operator << (std::ostream& os, const ImageObject& obj);

	friend std::istream& operator >> (std::istream& is, ImageObject& obj);
};

} /* Image */
} /* MediaVision */

#endif /* __IMAGEOBJECT_H__ */
