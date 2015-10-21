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

#ifndef __IMAGECONTOURSTABILIZATOR_H__
#define __IMAGECONTOURSTABILIZATOR_H__

#include "ImageConfig.h"

#include <opencv/cv.h>

/**
 * @file  ImageContourStabilizator.h
 * @brief This file contains functionality for image contour stabilization
 *        during tracking.
 */

namespace MediaVision {
namespace Image {
/**
 * @class    ImageContourStabilizator
 * @brief    This class contains functionality for image contour stabilization
 *           during tracking.
 *
 * @since_tizen 3.0
 */
class ImageContourStabilizator {
public:
	/**
	 * @brief   @ref ImageContourStabilizator default constructor.
	 *
	 * @since_tizen 3.0
	 */
	ImageContourStabilizator();

	/**
	 * @brief Stabilizes @a contour.
	 *
	 * @since_tizen 3.0
	 * @remarks Call this function alternately for each contour from sequence
	 * @param [in,out] contour   @ref contour, which will be stabilized
	 * @param [in]     params    configuration parameters
	 * @return true if contour is stabilized, otherwise return false
	 */
	bool stabilize(
				std::vector<cv::Point2f>& contour,
				const StabilizationParams& params);

	/**
	 * @brief Resets stabilization process.
	 *
	 * @since_tizen 3.0
	 * @remarks Call it before starting track on the new sequence of contours.
	 */
	void reset(void);

private:
	std::vector<cv::Point2f> computeStabilizedQuadrangleContour(void);

private:
	static const size_t MovingHistoryAmount = 3u;

	std::vector<float> m_speeds;

	std::vector<size_t> m_currentCornersSpeed;

	std::deque<std::vector<cv::Point2f> > m_movingHistory;

	std::vector<cv::Point2f> m_lastStabilizedContour;

	size_t m_currentHistoryAmount;

	int m_tempContourIndex;

	std::vector<float> m_priorities;

	bool m_isPrepared;
};

} /* Image */
} /* MediaVision */

#endif /* __IMAGECONTOURSTABILIZATOR_H__ */
