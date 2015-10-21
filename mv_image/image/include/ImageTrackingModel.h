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

#ifndef __IMAGETRACKINGMODEL_H__
#define __IMAGETRACKINGMODEL_H__

#include "ImageObject.h"

#include "ImageContourStabilizator.h"

#include <opencv/cv.h>

#include <pthread.h>

#include <vector>
#include <list>

/**
 * @file  ImageTrackingModel.h
 * @brief This file contains the @ref ImageTrackingModel class.
 */

namespace MediaVision {
namespace Image {
class ImageContourStabilizator;
/**
 * @class    ImageTrackingModel
 * @brief    This class contains the tracking functionality for image objects.
 *
 * @since_tizen 3.0
 */
class ImageTrackingModel {
private:
	/**
	 * @brief   @ref ImageTrackingModel state enumeration.
	 *
	 * @since_tizen 3.0
	 */
	enum State {
		Invalid,     /**< Invalid tracking model can not be tracked. Set not
					empty image object as target by using function
					@ref setTarget() to make tracking model valid, also
					you can load valid tracking model by using @ref load() */
		Undetected,  /**< The object was not recognized on the last frame. Ready
					for further recognition */
		Appeared,    /**< The object was recognized on one of the last frames
					after its absence  */
		Tracked,    /**< The object was recognized on the last frame. Its
					location can be obtained by calling method getLocation() */
		InProcess    /**< The object is in the recognition process */
	};

public:
	/**
	 * @brief   @ref ImageTrackingModel default constructor
	 *
	 * @since_tizen 3.0
	 */
	ImageTrackingModel();

	/**
	 * @brief   @ref ImageTrackingModel constructor based on tracking algorithm
	 *          parameters.
	 *
	 * @since_tizen 3.0
	 * @param[in] recognitionObject  @ref ImageObject which will be tracked
	 */
	ImageTrackingModel(const ImageObject& recognitionObject);

	/**
	 * @brief   @ref ImageTrackingModel copy constructor.
	 * @details Creates copy of @ref ImageTrackingModel
	 *
	 * @since_tizen 3.0
	 * @param   [in] copy @ref ImageTrackingModel which will be copied
	 */
	ImageTrackingModel(const ImageTrackingModel& copy);

	/**
	 * @brief   @ref ImageTrackingModel destructor.
	 *
	 * @since_tizen 3.0
	 */
	~ImageTrackingModel();

	/**
	 * @brief   Sets @ref ImageObject as target which will be tracked.
	 *
	 * @since_tizen 3.0
	 * @param [in] target @ref ImageObject which will be tracked
	 */
	void setTarget(const ImageObject& target);

	/**
	 * @brief Checks whether the tracking model is valid for tracking.
	 * @details Image tracking model is valid if its target is set and not empty.
	 *
	 * @since_tizen 3.0
	 * @remarks Invalid tracking model can not be tracked. Set not empty target
	 *          by using corresponding constructor or function @ref setTarget()
	 *          to make tracking model valid. Also you can load valid tracking
	 *          model by using @ref load().
	 * @return @c true if tracking model is valid, otherwise return @c false
	 */
	bool isValid() const;

	/**
	 * @brief   Refreshes tracking model.
	 *
	 * @since_tizen 3.0
	 * @remarks Call it before starting track on the new video stream.
	 */
	void refresh(void);

	/**
	 * @brief   @ref ImageTrackingModel copy assignment operator.
	 * @details Fills the information based on the @a copy
	 *
	 * @since_tizen 3.0
	 * @param   [in] copy @ref ImageTrackingModel which will be copied
	 */
	ImageTrackingModel& operator=(const ImageTrackingModel& copy);

	/**
	 * @brief  Stores the @ref ImageTrackingModel in a file.
	 *
	 * @since_tizen 3.0
	 * @param  [in] filepath  File name which will be generated
	 * @return @a 0 on success, otherwise a negative error value
	 */
	int save(const char *filepath) const;

	/**
	 * @brief  Loads the @ref ImageTrackingModel from the file.
	 *
	 * @since_tizen 3.0
	 * @param  [in] filepath  File name from which will be loaded a model
	 * @return @a 0 on success, otherwise a negative error value
	*/
	int load(const char *filepath);

	/**
	 * @brief  Checks state of the @ref ImageTrackingModel.
	 *
	 * @since_tizen 3.0
	 * @return @a true if object was detected on the last processed frame,
	 *         otherwise a @a false value
	 */
	bool isDetected() const;

	/**
	 * @brief  Gets last location of the @ref ImageTrackingModel.
	 *
	 * @since_tizen 3.0
	 * @return Last detected location
	 */
	std::vector<cv::Point2f> getLastlocation() const;

private:
	ImageObject m_recognitionObject;

	ImageContourStabilizator m_stabilizator;

	std::vector<cv::Point2f> m_lastLocation;

	State m_state;

	pthread_t m_recognitionThread;

	mutable pthread_mutex_t m_globalGuard;

	mutable pthread_spinlock_t m_lastLocationGuard;

	mutable pthread_spinlock_t m_stateGuard;

	friend std::ostream& operator << (
			std::ostream& os,
			const ImageTrackingModel::State& state);

	friend std::istream& operator >> (
			std::istream& is,
			ImageTrackingModel::State& state);

	friend std::ostream& operator << (
			std::ostream& os,
			const ImageTrackingModel& obj);

	friend std::istream& operator >> (
			std::istream& is,
			ImageTrackingModel& obj);

	friend class ImageTracker;
};

} /* Image */
} /* MediaVision */

#endif /* __IMAGETRACKINGMODEL_H__ */
