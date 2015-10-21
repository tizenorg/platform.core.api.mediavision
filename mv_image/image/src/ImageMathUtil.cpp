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

#include "ImageMathUtil.h"

namespace MediaVision {
namespace Image {
float getDistance(
		const cv::Point2f& point1,
		const cv::Point2f& point2)
{
	return sqrt(
			(point1.x - point2.x) * (point1.x - point2.x) +
			(point1.y - point2.y) * (point1.y - point2.y));
}

float getTriangleArea(
		const cv::Point2f& point1,
		const cv::Point2f& point2,
		const cv::Point2f& point3)
{
	float distances[3];

	distances[0] = getDistance(point1, point2);
	distances[1] = getDistance(point2, point3);
	distances[2] = getDistance(point3, point1);

	const float semiperimeter = (distances[0] + distances[1] + distances[2]) / 2.0f;

	return sqrt(semiperimeter *
			(semiperimeter - distances[0]) *
			(semiperimeter - distances[1]) *
			(semiperimeter - distances[2]));
}

float getQuadrangleArea(const cv::Point2f points[NumberOfQuadrangleCorners])
{
	return getTriangleArea(points[0], points[1], points[2]) +
			getTriangleArea(points[0], points[3], points[2]);
}

} /* Image */
} /* MediaVision */
