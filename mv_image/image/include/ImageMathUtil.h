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

#ifndef __IMAGEMATHUTIL_H__
#define __IMAGEMATHUTIL_H__

#include <opencv/cv.h>

/**
 * @file  ImageMathUtil.h
 * @brief This file contains math utility for Image Module.
 */

namespace MediaVision {
namespace Image {
const size_t MinimumNumberOfFeatures = 4u; /* Minimum number of features
											when perspective transform
											parameters calculation
											have sense */

const size_t NumberOfQuadrangleCorners = 4u; /* Number of quadrangle corneres */

/**
 * @brief   Calculates Euclidean distance between two points.
 *
 * @since_tizen 3.0
 * @param [in] point1   The first point
 * @param [in] point2   The second point
 * @return distance between two points
 */
float getDistance(
		const cv::Point2f& point1,
		const cv::Point2f& point2);

/**
 * @brief   Calculates area of triangle.
 *
 * @since_tizen 3.0
 * @param [in] point1   The first corner of triangle
 * @param [in] point2   The second corner of triangle
 * @param [in] point3   The third corner of triangle
 * @return area of triangle
 */
float getTriangleArea(
		const cv::Point2f& point1,
		const cv::Point2f& point2,
		const cv::Point2f& point3);

/**
 * @brief   Calculates area of quadrangle.
 *
 * @since_tizen 3.0
 * @param [in] points   Four corners of quadrangle
 * @return area of quadrangle
 */
float getQuadrangleArea(
		const cv::Point2f points[NumberOfQuadrangleCorners]);

/**
 * @brief   Checks point on the accessory region.
 *
 * @since_tizen 3.0
 * @param [in] point   Point which will be checked on the accessory region
 * @param [in] region  Contour of region
 * @return true if point is inside the region, otherwise return false
 */
bool checkAccessory(
		const cv::Point2f& point,
		const std::vector<cv::Point2f>& region);

/**
 * @brief   Cuts a rectangle according to the maximum size.
 * @details From the rectangle will remain only the part which is inside the
 *          rectangle from {0,0} to @a maxSize
 *
 * @since_tizen 3.0
 * @param [in] rectange   Rectangle which will be cut
 * @param [in] maxSize    Maximum values of needed rectangle
 */
void catRect(cv::Rect& rectange, const cv::Size& maxSize);

/**
 * @brief   Resizes a region.
 *
 * @since_tizen 3.0
 * @param [in] roi                  Contour of region which will be resized
 * @param [in] scalingCoefficient   Scaling coefficient
 */
std::vector<cv::Point2f> contourResize(
		const std::vector<cv::Point2f>& roi,
		float scalingCoefficient);

} /* Image */
} /* MediaVision */

#endif /* __IMAGEMATHUTIL_H__ */
