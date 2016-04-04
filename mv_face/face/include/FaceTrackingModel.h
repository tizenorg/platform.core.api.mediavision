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

#ifndef __FACETRACKINGMODEL_H__
#define __FACETRACKINGMODEL_H__

#include "TrackerMedianFlow.h"

/**
 * @file FaceTrackingModel.h
 * @brief This file contains the FaceTrackingModel class definition which
 *        provides face tracking model interface.
 */

namespace MediaVision {
namespace Face {
/**
 * @brief Structure where results of
 *        @ref MediaVision::Face::FaceTrackingModel::track() call are stored.
 *
 * @since_tizen 3.0
 */
struct EXPORT_API FaceTrackingResults {
	/**
	 * @brief Default constructor for the @ref FaceTrackingResults
	 *
	 * @since_tizen 3.0
	 */
	FaceTrackingResults();

	bool mIsTracked;                  /**< The flag indication success of the
											tracking */
	cv::Rect_<float> mFaceLocation;   /**< Location of the face at the current
											track iteration where face position
											is predicted */
	float mConfidence;                /**< Tracking confidence level
											(0.0 .. 1.0) */
};

/**
 * @class FaceTrackingModel
 * @brief Class providing interface for management of face tracking model.
 *
 * @since_tizen 3.0
 */
class EXPORT_API FaceTrackingModel {
public:
	/**
	 * @brief    Creates a FaceTrackingModel class instance.
	 *
	 * @since_tizen 3.0
	 */
	FaceTrackingModel();

	/**
	 * @brief    Creates a FaceTrackingModel class instance based on existed
	 *           instance.
	 *
	 * @since_tizen 3.0
	 * @param [in] origin    The FaceTrackingModel object that will be used
	 *                       for creation of new one
	 */
	FaceTrackingModel(const FaceTrackingModel& origin);

	/**
	 * @brief @ref FaceTrackingModel copy assignment operator.
	 * @details Fills the information based on the @a copy
	 *
	 * @since_tizen 3.0
	 * @param [in] copy      @ref FaceTrackingModel object which will be
	 *                       copied
	 */
	FaceTrackingModel& operator=(const FaceTrackingModel& copy);

	/**
	 * @brief    Destroys the FaceTrackingModel class instance including all
	 *           its resources.
	 *
	 * @since_tizen 3.0
	 */
	~FaceTrackingModel();

	/**
	 * @brief Serializes FaceTrackingModel object to the file.
	 *
	 * @since_tizen 3.0
	 * @param [in] fileName    The name to the file to which serialized
	 *                         FaceTrackingModel object will be saved
	 * @return @c 0 on success, otherwise a negative error value
	 *
	 * @see FaceTrackingModel::load()
	 */
	int save(const std::string& fileName);

	/**
	 * @brief Deserializes FaceTrackingModel object from the file.
	 *
	 * @since_tizen 3.0
	 * @param [in] fileName    The name of the file from which serialized
	 *                         FaceTrackingModel object will be deserialized
	 * @return @c 0 on success, otherwise a negative error value
	 *
	 * @see FaceTrackingModel::save()
	 */
	int load(const std::string& fileName);

	/**
	 * @brief Prepares FaceTrackingModel object to the next tracking session.
	 *
	 * @since_tizen 3.0
	 * @param [in] image          First frame of the video or image sequence for
	 *                            which tracking will be started
	 * @return @c 0 on success, otherwise a negative error value
	 *
	 * @see FaceTrackingModel::save()
	 */
	int prepare(const cv::Mat& image);

	/**
	 * @brief Prepares FaceTrackingModel object to the next tracking session.
	 *
	 * @since_tizen 3.0
	 * @param [in] image          First frame of the video or image sequence for
	 *                            which tracking will be started
	 * @param [in] boundingBox    Rectangular location of the face on the @a
	 *                            image
	 * @return @c 0 on success, otherwise a negative error value
	 *
	 * @see FaceTrackingModel::save()
	 */
	int prepare(const cv::Mat& image, const cv::Rect_<float>& boundingBox);

	/**
	 * @brief Performs one tracking iteration for the video frame or image
	 *        from the continuous sequence of images.
	 *
	 * @since_tizen 3.0
	 * @param [in]  image
	 * @param [out] boundingBox
	 */
	int track(const cv::Mat& image, FaceTrackingResults& results);

private:
	bool m_canTrack;                          /**< The flag showing possibility
													of the tracking model to
													perform track */

	cv::Ptr<cv::TrackerMedianFlow> m_tracker; /**< Underlying OpenCV tracking
													model */
};

} /* Face */
} /* MediaVision */

#endif /* __FACETRACKINGMODEL_H__ */
