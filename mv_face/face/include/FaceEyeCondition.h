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

#ifndef __FACEEYECONDITION_H__
#define __FACEEYECONDITION_H__

#include <mv_common_c.h>
#include <mv_face.h>

#include <opencv/cv.h>

/**
 * @file FaceEyeCondition.h
 * @brief This file contains the FaceEyeCondition class which implements the face
 *        eye condition recognition functionality.
 */

namespace MediaVision
{
namespace Face
{

/**
 * @class FaceEyeCondition
 * @brief The FaceEyeCondition implements the face
 *        eye condition recognition functionality.
 *
 * @since_tizen 3.0
 */
class FaceEyeCondition
{
public:

    /**
     * @brief Recognizes eye condition on the image with face location.
     *
     * @since_tizen 3.0
     * @param [in] grayImage        The image  in gray scale with face where
     *                              eye condition will be recognized
     * @param [in] faceLocation     The rectangle with face location
     * @param [out] eyeCondition    The eye condition which was recognized
     * @return @c 0 on success, otherwise a negative error value
     */
    static int recognizeEyeCondition(
                   const cv::Mat& grayImage,
                   mv_rectangle_s faceLocation,
                   mv_face_eye_condition_e *eyeCondition);

private:

    static void splitEyes(
                    /*[in]*/ const cv::Mat& grayImage,
                    /*[in]*/ mv_rectangle_s faceLocation,
                    /*[out]*/ cv::Mat& leftEye,
                    /*[out]*/ cv::Mat& rightEye);

    static int isEyeOpen(/*[in]*/const cv::Mat& eye);
};

} /* Face */
} /* MediaVision */

#endif /* __FACEEYECONDITION_H__ */
