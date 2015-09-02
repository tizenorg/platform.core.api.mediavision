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

#include "FaceDetector.h"

namespace MediaVision
{
namespace Face
{

FaceDetector::FaceDetector() :
        m_faceCascade(),
        m_haarcascadeFilepath(),
        m_faceCascadeIsLoaded(false)
{
    ; /* NULL */
}

FaceDetector::~FaceDetector()
{
    ; /* NULL */
}

bool FaceDetector::detectFaces(
        const cv::Mat& image,
        const cv::Rect& roi,
        const cv::Size& minSize,
        std::vector<cv::Rect>& faceLocations)
{
    if (!m_faceCascadeIsLoaded)
    {
        return false;
    }

    faceLocations.clear();

    cv::Mat intrestingRegion = image;

    bool roiIsUsed = false;
    if (roi.x >= 0 && roi.y >= 0 && roi.width > 0 && roi.height > 0 &&
        (roi.x + roi.width) <= image.cols && (roi.y + roi.height) <= image.rows)
    {
        intrestingRegion = intrestingRegion(roi);
        roiIsUsed = true;
    }

    if (minSize.width > 0 && minSize.height > 0 &&
        minSize.width <= image.cols && minSize.height <= image.rows)
    {
        m_faceCascade.detectMultiScale(
                intrestingRegion,
                faceLocations,
                1.1,
                3,
                0,
                minSize);
    }
    else
    {
        m_faceCascade.detectMultiScale(intrestingRegion, faceLocations);
    }

    if (roiIsUsed)
    {
        const size_t numberOfLocations = faceLocations.size();
        for (size_t i = 0u; i < numberOfLocations; ++i)
        {
            faceLocations[i].x += roi.x;
            faceLocations[i].y += roi.y;
        }
    }

    return true;
}

bool FaceDetector::loadHaarcascade(const std::string& haarcascadeFilepath)
{

    if (!m_faceCascadeIsLoaded || m_haarcascadeFilepath != haarcascadeFilepath)
    {
        if (!(m_faceCascadeIsLoaded = m_faceCascade.load(haarcascadeFilepath)))
        {
            return false;
        }
        m_haarcascadeFilepath = haarcascadeFilepath;
    }

    return true;
}

} /* Face */
} /* MediaVision */
