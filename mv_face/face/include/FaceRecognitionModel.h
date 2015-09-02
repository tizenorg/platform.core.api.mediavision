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

#ifndef __FACERECOGNITIONMODEL_H__
#define __FACERECOGNITIONMODEL_H__

#include "FaceUtil.h"

#include <opencv2/core/core.hpp>
#include <opencv2/contrib/contrib.hpp>

#include <cstring>
#include <vector>

/**
 * @file FaceRecognitionModel.h
 * @brief This file contains the FaceRecognitionModel class definition which
 *        provides face recognition model interface.
 */

namespace MediaVision
{
namespace Face
{

/**
 * @brief Structure containing supported recognition algorithms settings.
 *
 * @since_tizen 3.0
 */
struct FaceRecognitionModelConfig
{
    /**
     * @brief Default constructor for the @ref FaceRecognitionModelConfig
     *
     * @since_tizen 3.0
     */
    FaceRecognitionModelConfig();

    bool operator!=(
            const FaceRecognitionModelConfig& other) const;

    FaceRecognitionModelType mModelType; /**<
                                Type of the recognition algorithm */

    int mNumComponents;    /**< How many principal components will be included
                                to the Eigenvectors */

    double mThreshold;     /**< Minimal distance between principal components of
                                the model allowed */

    int mRadius;           /**< Radius of the local features for LBHP algorithm */

    int mNeighbors;        /**< How many neighboring pixels has to be analyzed
                                when LBHP learning applied. Usually set as
                                8*radius */

    int mGridX;            /**< X size of the spatial histogram (LBPH) */

    int mGridY;            /**< Y size of the spatial histogram (LBPH) */

    int mImgWidth;         /**< Width of the image to resize the samples for
                                algorithms working on the samples of the same
                                size (Eigenfaces, Fisherfaces) */

    int mImgHeight;        /**< Height of the image to resize the samples for
                                algorithms working on the samples of the same
                                size (Eigenfaces, Fisherfaces) */
};

/**
 * @brief Structure where results of
 *        @ref MediaVision::Face::FaceRecognitionModel::recognize() call is
 *        stored.
 *
 * @since_tizen 3.0
 */
struct FaceRecognitionResults
{
    /**
     * @brief Default constructor for the @ref FaceRecognitionResults
     *
     * @since_tizen 3.0
     */
    FaceRecognitionResults();

    bool mIsRecognized;               /**< The flag indication success of the
                                           recognition */
    cv::Rect_<int> mFaceLocation;     /**< Location of the face where face has
                                           been recognized */
    int mFaceLabel;                   /**< Unique label of the face */
    double mConfidence;               /**< Recognition confidence level */
};

/**
 * @class FaceRecognitionModel
 * @brief Class providing interface for management of face recognition model.
 *
 * @since_tizen 3.0
 */
class FaceRecognitionModel
{
public:

    /**
     * @brief    Creates a FaceRecognitionModel class instance.
     *
     * @since_tizen 3.0
     */
    FaceRecognitionModel();

    /**
     * @brief    Creates a FaceRecognitionModel class instance based on existed
     *           instance.
     *
     * @since_tizen 3.0
     * @param [in] origin    The FaceRecognitionModel object that will be used
     *                       for creation of new one
     */
    FaceRecognitionModel(const FaceRecognitionModel& origin);

    /**
     * @brief @ref FaceRecognitionModel copy assignment operator.
     * @details Fills the information based on the @a copy
     *
     * @since_tizen 3.0
     * @param [in] copy      @ref FaceRecognitionModel object which will be
     *                       copied
     */
    FaceRecognitionModel& operator=(const FaceRecognitionModel& copy);

    /**
     * @brief    Destroys the FaceRecognitionModel class instance including all
     *           its resources.
     *
     * @since_tizen 3.0
     */
    ~FaceRecognitionModel();

    /**
     * @brief Serializes FaceRecognitionModel object to the file.
     *
     * @since_tizen 3.0
     * @param [in] fileName    The name of the file to which serialized
     *                         FaceRecognitionModel object will be saved
     * @return @c 0 on success, otherwise a negative error value
     *
     * @see FaceRecognitionModel::load()
     */
    int save(const std::string& fileName);

    /**
     * @brief Deserializes FaceRecognitionModel object from the file.
     *
     * @since_tizen 3.0
     * @param [in] fileName    The name to the file from which serialized
     *                         FaceRecognitionModel object will be deserialized
     * @return @c 0 on success, otherwise a negative error value
     *
     * @see FaceRecognitionModel::save()
     */
    int load(const std::string& fileName);

    /**
     * @brief Adds face image example for face labeled by @a faceLabel
     *
     * @since_tizen 3.0
     * @param [in] faceImage    Face image to be added to the training set
     * @param [in] faceLabel    Label that defines class of the face
     * @return @c 0 on success, otherwise a negative error value
     *
     * @see FaceRecognitionModel::resetFaceExamples()
     */
    int addFaceExample(const cv::Mat& faceImage, int faceLabel);

    /**
     * @brief Clears the internal set of face image examples.
     *
     * @since_tizen 3.0
     * @remarks Internal set of face image examples contains all samples
     *          collected with @ref FaceRecognitionModel::addPositiveExample()
     *          method.
     * @return @c 0 on success, otherwise a negative error value
     *
     * @see FaceRecognitionModel::addFaceExample()
     */
    int resetFaceExamples(void);

    /**
     * @brief Clears the internal set of face image examples labeled with
     *        @a faceLabel.
     *
     * @since_tizen 3.0
     * @remarks Internal set of face image examples contains all samples
     *          collected with @ref FaceRecognitionModel::addPositiveExample()
     *          method.
     * @param faceLabel Unique for the model face label
     * @return @c 0 on success, otherwise a negative error value
     *
     * @see FaceRecognitionModel::addFaceExample()
     */
    int resetFaceExamples(int faceLabel);

    /**
     * @brief Getter for the face labels learned by the model.
     *
     * @since_tizen 3.0
     * @remarks Returning vector will contain only labels had been learned by
     *          FaceRecognitionModel::learn() method.
     * @return Vector of the face labels known by the model
     *
     * @see FaceRecognitionModel::addFaceExample()
     * @see FaceRecognitionModel::learn()
     */
    const std::set<int>& getFaceLabels(void) const;

    /**
     * @brief Learns recognition model based on the set of collected face image
     *        examples.
     *
     * @since_tizen 3.0
     * @param [in] config    Configuration of the algorithm to be used for
     *                       learning the model
     * @return @c 0 on success, otherwise a negative error value
     *
     * @see FaceRecognitionModel::addFaceExample()
     */
    int learn(const FaceRecognitionModelConfig& config = FaceRecognitionModelConfig());

    /**
     * @brief Recognizes faces in the image and outputs recognition results to
     *        the @a results structure.
     *
     * @since_tizen 3.0
     * @param [in] config      Configuration of the algorithm to be used for
     *                         face recognition
     * @param [out] results    Structure that will contain recognition results
     * @return @c 0 on success, otherwise a negative error value
     *
     * @see FaceRecognitionModel::learn()
     */
    int recognize(const cv::Mat& image, FaceRecognitionResults& results);

private:

    /**
     * Factory method for creating of the recognition algorithm based on input
     * configuration:
     */
    static cv::Ptr<cv::FaceRecognizer> CreateRecognitionAlgorithm(
            const FaceRecognitionModelConfig& config =
                    FaceRecognitionModelConfig());

private:

    bool m_canRecognize; /**< The flag showing possibility to recognize with
                              the face recognition model */

    std::map<int, std::vector<cv::Mat> > m_faceSamples; /**< Samples of the
                                                             images which
                                                             will be used for
                                                             the learning */

    FaceRecognitionModelConfig m_learnAlgorithmConfig; /**< Configuration of the
                                                            learning method */

    cv::Ptr<cv::FaceRecognizer> m_recognizer; /**< Recognizer associated with
                                                   the current model */

    std::set<int> m_learnedLabels; /**< Vector of the labels had been learned
                                           by the model */
};

} /* Face */
} /* MediaVision */

#endif /* __FACERECOGNITIONMODEL_H__ */
