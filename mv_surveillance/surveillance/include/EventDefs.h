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

#ifndef __MEDIA_VISION_EVENT_DEFS_H__
#define __MEDIA_VISION_EVENT_DEFS_H__

/**
 * @file  EventDefs.h
 * @brief This file contains definitions for event triggers.
 */

#include <mv_common.h>

#include <string>
#include <vector>
#include <list>
#include <map>

#include <opencv2/opencv.hpp>

namespace mediavision {
namespace surveillance {

typedef std::map<std::string, std::vector<std::string> > EventTypesMap;
typedef EventTypesMap::iterator EventTypesMapIter;
typedef EventTypesMap::const_iterator EventTypesMapConstIter;

typedef std::vector<std::string> StringVector;
typedef StringVector::iterator StringIter;
typedef StringVector::const_iterator StringConstIter;

typedef std::vector<int> IntVector;
typedef IntVector::iterator IntIter;
typedef IntVector::const_iterator IntConstIter;

typedef std::vector<double> DoubleVector;
typedef DoubleVector::iterator DoubleIter;
typedef DoubleVector::const_iterator DoubleConstIter;

typedef std::vector<mv_rectangle_s> MVRectangles;
typedef MVRectangles::iterator MVRectanglesIter;
typedef MVRectangles::const_iterator MVRectanglesConstIter;

typedef std::vector<cv::Rect> CVRectangles;
typedef CVRectangles::iterator CVRectanglesIter;
typedef CVRectangles::const_iterator CVRectanglesConstIter;

typedef std::vector<mv_point_s> MVPoints;
typedef MVPoints::iterator MVPointsIter;
typedef MVPoints::const_iterator MVPointsConstIter;

typedef std::vector<cv::Point> CVPoints;
typedef std::vector<CVPoints> Contours;

} /* surveillance */
} /* mediavision */

#endif /* __MEDIA_VISION_EVENT_DEFS_H__ */
