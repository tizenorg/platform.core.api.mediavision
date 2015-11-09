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

	const float res2x = semiperimeter *
			(semiperimeter - distances[0]) *
			(semiperimeter - distances[1]) *
			(semiperimeter - distances[2]);

	if (res2x < 0.f)
		return 0.f;

	return sqrt(res2x);
}

float getQuadrangleArea(const cv::Point2f points[NumberOfQuadrangleCorners])
{
	return getTriangleArea(points[0], points[1], points[2]) +
			getTriangleArea(points[0], points[3], points[2]);
}

bool checkAccessory(
		const cv::Point2f& point,
		const std::vector<cv::Point2f>& region)
{
	if (region.size() < 3)
		return false;

	bool insideFlag = false;
	const size_t numberOfContourPoints = region.size();

	for (size_t i = 0u, j = numberOfContourPoints - 1; i < numberOfContourPoints; j = i++) {
		if (((region[i].y > point.y) != (region[j].y > point.y)) &&
			((float) point.x < (float)
			(region[j].x - region[i].x) * (point.y - region[i].y) /
			(region[j].y - region[i].y) + region[i].x)) {
			insideFlag = !insideFlag;
			}
	}

	return insideFlag;
}

void catRect(cv::Rect& rectange, const cv::Size& maxSize)
{
	if (rectange.width < 0) {
		rectange.x += rectange.width;
		rectange.width *= -1;
	}

	if (rectange.height < 0) {
		rectange.y += rectange.height;
		rectange.height *= -1;
	}

	if (rectange.x > maxSize.width || rectange.y > maxSize.height) {
		rectange.x = 0;
		rectange.y = 0;
		rectange.width = 0;
		rectange.height = 0;
		return;
	}

	if (rectange.x < 0) {
		rectange.width += rectange.x;
		rectange.x = 0;
	}

	if (rectange.y < 0) {
		rectange.height += rectange.y;
		rectange.y = 0;
	}

	if (rectange.x + rectange.width > maxSize.width)
		rectange.width = maxSize.width - rectange.x;

	if (rectange.y + rectange.height > maxSize.height)
		rectange.height = maxSize.height - rectange.y;
}

std::vector<cv::Point2f> contourResize(
		const std::vector<cv::Point2f>& roi,
		float scalingCoefficient)
{
	const size_t numberOfContourPoints = roi.size();
	cv::Point2f centre(0, 0);
	for (size_t i = 0; i < numberOfContourPoints; ++i) {
		centre.x += roi[i].x;
		centre.y += roi[i].y;
	}
	centre.x /= numberOfContourPoints;
	centre.y /= numberOfContourPoints;

	std::vector<cv::Point2f> result(numberOfContourPoints);
	for (size_t i = 0; i < numberOfContourPoints; ++i) {
		result[i].x = (roi[i].x - centre.x) * scalingCoefficient + centre.x;
		result[i].y = (roi[i].y - centre.y) * scalingCoefficient + centre.y;
	}

	return result;
}

} /* Image */
} /* MediaVision */
