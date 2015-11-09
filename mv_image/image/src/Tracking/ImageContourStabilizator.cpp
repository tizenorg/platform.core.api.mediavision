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

#include "Tracking/ImageContourStabilizator.h"

#include "mv_private.h"

namespace MediaVision {
namespace Image {
ImageContourStabilizator::ImageContourStabilizator() :
		m_movingHistory(),
		m_priorities()
{
	reset();
}

void ImageContourStabilizator::reset(void)
{
	m_isPrepared = false;
	m_tempContourIndex = -1;
	m_currentHistoryAmount = 0;
	m_historyAmount = 0;
	m_movingHistory.clear();
}

ImageContourStabilizator::StabilizationError ImageContourStabilizator::stabilize(
		std::vector<cv::Point2f>& contour,
		const StabilizationParams& params)
{
	if (!updateSettings(params)) {
		LOGW("Not stabilized. Invalid settings.");

		return InvalidSettings;
	}

	/* current implementation stabilizes quadrangles only */
	if (contour.size() != NumberOfQuadrangleCorners) {
		LOGW("Not stabilized. Unsupported contour type.");

		return UnsupportedContourType;
	}

	m_currentCornersSpeed.resize(contour.size(), 0);

	if (m_lastStabilizedContour.empty())
		m_lastStabilizedContour = contour;

	std::vector<cv::Point2f> stabilizedState;

	/* history amount < 2 it's no sense */
	if (m_historyAmount >= 2) {
		/* first sample */
		if (m_tempContourIndex == -1) {
			m_movingHistory.push_back(contour);
			m_movingHistory.push_back(contour);

			m_tempContourIndex = 1;
			m_currentHistoryAmount = 2;

			LOGI("Not stabilized. Too small moving history. (the first one)");

			return TooShortMovingHistory;
		}

		/* too short moving history */
		if (m_currentHistoryAmount < m_historyAmount) {
			++m_currentHistoryAmount;
			++m_tempContourIndex;
			m_movingHistory.push_back(contour);

			LOGI("Not stabilized. Too short moving history.");

			return TooShortMovingHistory;
		}

		/* saving into moving history */
		m_movingHistory.pop_front();
		m_movingHistory.push_back(contour);

		if (!m_isPrepared) {
			m_lastStabilizedContour = m_movingHistory[m_historyAmount - 2];

			LOGI("Not stabilized. Too small moving history. (the last one)");

			m_isPrepared = true;
		}

		/* stabilization */
		stabilizedState = computeStabilizedQuadrangleContour();

		if (stabilizedState.empty())
			stabilizedState = m_lastStabilizedContour;
	} else {
		stabilizedState = m_lastStabilizedContour;
	}

	const float tolerantShift = getQuadrangleArea(contour.data()) *
				m_tolerantShift + m_tolerantShiftExtra;

	const size_t contourSize = stabilizedState.size();
	for (size_t i = 0u; i < contourSize; ++i) {
		if (fabs(getDistance(stabilizedState[i], contour[i])) > tolerantShift) {
			const float dirX = m_lastStabilizedContour[i].x - contour[i].x;
			const float dirY = m_lastStabilizedContour[i].y - contour[i].y;

			const float speedX = dirX * m_speeds[m_currentCornersSpeed[i]];
			const float speedY = dirY * m_speeds[m_currentCornersSpeed[i]];

			/* final moving */
			m_lastStabilizedContour[i].x -= speedX;
			m_lastStabilizedContour[i].y -= speedY;

			if (m_currentCornersSpeed[i] < m_speeds.size() - 1) {
				++m_currentCornersSpeed[i];
			}
		} else {
			m_currentCornersSpeed[i] = 0;
		}
	}

	/* m_lastStabilizedContour = stabilizedState; */
	contour = m_lastStabilizedContour;

	LOGI("Contour successfully stabilized.");

	return Successfully;
}

bool ImageContourStabilizator::updateSettings(const StabilizationParams& params)
{
	if (params.mHistoryAmount < 1)
		return false;

	m_tolerantShift = (float)params.mTolerantShift;
	m_tolerantShiftExtra = (float)params.mTolerantShiftExtra;

	if (m_historyAmount != (size_t)params.mHistoryAmount) {
		m_historyAmount = (size_t)params.mHistoryAmount;

		m_priorities.resize(m_historyAmount);

		/* calculation of priorities for positions in the moving history */
		for (size_t i = 0u; i < m_historyAmount; ++i) {
			/* linear dependence on the elapsed time */
			m_priorities[i] = ((i + 1) * 2.0f) /
						((m_historyAmount + 1) * m_historyAmount);
		}
	}

	while (m_historyAmount > (size_t)params.mHistoryAmount) {
		m_movingHistory.pop_front();
		--m_historyAmount;
	}

	if ((size_t)params.mHistoryAmount > m_historyAmount) {
		/* TODO: save current moving history */

		m_tempContourIndex = -1;
		m_historyAmount = (size_t)params.mHistoryAmount;
		m_movingHistory.clear();
	}

	bool speedIsValid = false;
	if (m_speeds.size() > 1) {
		const static float Epsilon = 0.0001f;
		if (fabs(m_speeds[0] - params.mStabilizationSpeed) < Epsilon &&
				fabs((m_speeds[1] - m_speeds[0]) -
				params.mStabilizationAcceleration) < Epsilon) {
		speedIsValid = true;
		}
	}

	if (!speedIsValid) {
		m_speeds.clear();

		int speedsSize = (int)((1 - params.mStabilizationSpeed) /
					params.mStabilizationAcceleration) + 1;

		if (speedsSize < 1) {
			m_speeds.push_back(1.0f);
		} else {
			static const int MaxSpeedsSize = 25;

			if (speedsSize > MaxSpeedsSize)
				speedsSize = MaxSpeedsSize;

			float speed = std::max(0.f,
					std::min((float)params.mStabilizationSpeed, 1.0f));

			for (int i = 0; i < speedsSize; ++i) {
				m_speeds.push_back(speed);
				speed += params.mStabilizationAcceleration;
			}
		}
	}

	return true;
}

std::vector<cv::Point2f> ImageContourStabilizator::computeStabilizedQuadrangleContour(void)
{
	/* final contour */
	std::vector<cv::Point2f> stabilizedState(
				NumberOfQuadrangleCorners, cv::Point2f(0.f, 0.f));

	/* calculation the direction of contour corners to a new location */
	std::vector<cv::Point2f> directions(
				NumberOfQuadrangleCorners, cv::Point2f(0.f, 0.f));

	/* computing expected directions and outliers searching */
	bool expressiveTime = false;
	float summPriorityWithoutToLastPos[NumberOfQuadrangleCorners];
	float priorityToLastPos[NumberOfQuadrangleCorners];
	std::vector<cv::Point2f> directionsToLastPos(NumberOfQuadrangleCorners);
	for (size_t j = 0u; j < NumberOfQuadrangleCorners; ++j) {
		/* calculation the moving directions and computing average direction */
		std::vector<cv::Point2f> trackDirections(m_historyAmount - 1);
		cv::Point2f averageDirections(0.f, 0.f);

		for (size_t i = 0u; i < m_historyAmount - 1; ++i) {
			averageDirections.x += (trackDirections[i].x =
					m_movingHistory[i+1][j].x - m_movingHistory[i][j].x) /
					(m_historyAmount - 1);

			averageDirections.y += (trackDirections[i].y =
					m_movingHistory[i+1][j].y - m_movingHistory[i][j].y) /
					(m_historyAmount - 1);
		}

		/* calculation a deviations and select outlier */
		std::vector<float> directionDistances(m_historyAmount - 1);
		float maxDistance = 0.f, prevMaxDistance = 0.f;
		int idxWithMaxDistance = 0;
		int numExpressiveDirection = -1;
		for (size_t i = 0u; i < m_historyAmount - 1; ++i) {
			directionDistances[i] = getDistance(
					trackDirections[i],
					averageDirections);

			if (directionDistances[i] > prevMaxDistance) {
				if (directionDistances[i] > maxDistance) {
					prevMaxDistance = maxDistance;
					maxDistance = directionDistances[i];
					idxWithMaxDistance = i;
				} else {
					prevMaxDistance = directionDistances[i];
				}
			}
		}

		/* check outlier */
		if (0.6f * maxDistance > prevMaxDistance) {
			LOGI("Outlier is detected.");

			numExpressiveDirection = idxWithMaxDistance;
		}

		/* final direction computing */
		float summPriority = 0.f;
		for (size_t i = 0u; i < m_historyAmount - 1; ++i) {
			if ((int)i != numExpressiveDirection) {
				directions[j].x += trackDirections[i].x * m_priorities[i];
				directions[j].y += trackDirections[i].y * m_priorities[i];
				summPriority += m_priorities[i];
			}
		}

		if (numExpressiveDirection == (int)(m_historyAmount - 1))
			expressiveTime = true;

		summPriorityWithoutToLastPos[j] = summPriority;
		priorityToLastPos[j] = m_priorities[m_historyAmount - 1];

		directions[j].x -= directionsToLastPos[j].x =
					(m_lastStabilizedContour[j].x -
					m_movingHistory[m_historyAmount - 1][j].x) *
					priorityToLastPos[j];

		directions[j].y -= directionsToLastPos[j].y =
					(m_lastStabilizedContour[j].y -
					m_movingHistory[m_historyAmount - 1][j].y) *
					priorityToLastPos[j];

		summPriority += priorityToLastPos[j];

		directions[j].x /= summPriority;
		directions[j].y /= summPriority;
	}

	/* final corners computing */
	for (size_t j = 0u; j < NumberOfQuadrangleCorners; ++j) {
		if (expressiveTime) {
			directions[j].x *= (summPriorityWithoutToLastPos[j] +
						priorityToLastPos[j]);
			directions[j].x -= directionsToLastPos[j].x;
			directions[j].x /= summPriorityWithoutToLastPos[j];

			directions[j].y *= (summPriorityWithoutToLastPos[j] +
						priorityToLastPos[j]);
			directions[j].y -= directionsToLastPos[j].y;
			directions[j].y /= summPriorityWithoutToLastPos[j];
		}

		stabilizedState[j].x = m_lastStabilizedContour[j].x + directions[j].x;
		stabilizedState[j].y = m_lastStabilizedContour[j].y + directions[j].y;
	}

	return stabilizedState;
}

} /* Image */
} /* MediaVision */
