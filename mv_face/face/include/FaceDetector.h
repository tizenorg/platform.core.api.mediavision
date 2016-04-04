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

#ifndef __FACEDETECTOR_H__
#define __FACEDETECTOR_H__

#include <opencv/cv.h>
#include <vector>
#include <string>

/**
 * @file FaceDetector.h
 * @brief This file contains the FaceDetector class which implement the face
 *        detection functionality.
 */

namespace MediaVision {
namespace Face {
/**
 * @class   FaceDetector
 * @brief   The Face Detector container.
 * @details It is class which contains face detection functionality.
 *
 * @since_tizen 3.0
 */
class EXPORT_API FaceDetector {
public:
	/**
	 * @brief   Creates a FaceDetector.
	 *
	 * @since_tizen 3.0
	 */
	FaceDetector();

	/**
	 * @brief   Destroys the FaceDetector and releases all its resources.
	 *
	 * @since_tizen 3.0
	 */
	virtual ~FaceDetector();

	/**
	 * @brief Performs face detection functionality.
	 * @details Use this function to launch face detection algorithm which
	 *          used the haarcascade set by setHaarcascadeFilepath().
	 *
	 * @since_tizen 3.0
	 * @param [in]  image           The image where faces will be detected
	 * @param [in]  roi             Region of image where faces will be detected
	 * @param [in]  minSize         Minimum size of faces which will be detected
	 * @param [out] faceLocations   The result locations of detected faces.
	 * @return true if detect process is completely finished. Otherwise return false.
	 *
	 * @pre Set a face haarcascade by calling setHaarcascadeFilepath()
	 *
	 * @see setHaarcascadeFilepath()
	 */
	bool detectFaces(
			const cv::Mat& image,
			const cv::Rect& roi,
			const cv::Size& minSize,
			std::vector<cv::Rect>& faceLocations);

	/**
	 * @brief Loads haar cascade classifier for detection process.
	 * @details This method is mandatory for normally detecting process.
	 *
	 * @since_tizen 3.0
	 * @param [in] haarcascadeFilepath The path to the file, which contains haar
	 *                                 cascade classifier information for
	 *                                 detection process.
	 * @return true if cascade is loaded from file and ready for detecting
	 *         process. Otherwise is false.
	 */
	bool loadHaarcascade(const std::string& haarcascadeFilepath);

private:
	cv::CascadeClassifier m_faceCascade; /**< Cascade classifier of the face
												detecting process. */

	std::string m_haarcascadeFilepath;   /**< Path to the file, which contains
												cascade classifier information. */

	bool m_faceCascadeIsLoaded; /**< Flag to determine the state of the
										m_faceCascade class. true if cascade is loaded
										from file and is ready to detecting process.
										Otherwise is false. */
};

} /* Face */
} /* MediaVision */

#endif /* __FACEDETECTOR_H__ */
