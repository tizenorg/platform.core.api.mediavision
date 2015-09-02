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

#include "FaceRecognitionModel.h"

#include <app_common.h>

#include "mv_private.h"
#include "mv_common.h"

#include <map>

#include <stdio.h>
#include <unistd.h>

namespace MediaVision
{
namespace Face
{

namespace
{

int CopyOpenCVAlgorithmParameters(const cv::Ptr<cv::FaceRecognizer>& srcAlg,
        cv::Ptr<cv::FaceRecognizer>& dstAlg)
{
    char tempPath[1024];

    sprintf(tempPath, "/tmp/alg_copy_%p_%p", srcAlg.obj, dstAlg.obj);

    srcAlg->save(tempPath);
    dstAlg->load(tempPath);

    if (0 != remove(tempPath))
    {
        LOGW("Error removing serialized FaceRecognizer in %s", tempPath);
    }

    // todo: consider to uncomment this lines if OpenCV will support deep
    // copy of AlgorithmInfo objects:

    /*std::vector<std::string> paramNames;
    srcAlg->getParams(paramNames);
    size_t paramSize = paramNames.size();
    for (size_t i = 0; i < paramSize; ++i)
    {
        int pType = srcAlg->paramType(paramNames[i]);

        switch(pType)
        {
        case cv::Param::INT:
        case cv::Param::UNSIGNED_INT:
        case cv::Param::UINT64:
        case cv::Param::SHORT:
        case cv::Param::UCHAR:
            dstAlg->set(paramNames[i], srcAlg->getInt(paramNames[i]));
            break;
        case cv::Param::BOOLEAN:
            dstAlg->set(paramNames[i], srcAlg->getBool(paramNames[i]));
            break;
        case cv::Param::REAL:
        case cv::Param::FLOAT:
            dstAlg->set(paramNames[i], srcAlg->getDouble(paramNames[i]));
            break;
        case cv::Param::STRING:
            dstAlg->set(paramNames[i], srcAlg->getString(paramNames[i]));
            break;
        case cv::Param::MAT:
            dstAlg->set(paramNames[i], srcAlg->getMat(paramNames[i]));
            break;
        case cv::Param::MAT_VECTOR:
        {
            //std::vector<cv::Mat> value = srcAlg->getMatVector(paramNames[i]);
            //dstAlg->info()->addParam(*(dstAlg.obj), paramNames[i].c_str(), value);
            dstAlg->set(paramNames[i], srcAlg->getMatVector(paramNames[i]));
            break;
        }
        case cv::Param::ALGORITHM:
            dstAlg->set(paramNames[i], srcAlg->getAlgorithm(paramNames[i]));
            break;
        default:
            LOGE("While copying algorothm parameters unsupported parameter "
                 "%s was found.", paramNames[i].c_str());

            return MEDIA_VISION_ERROR_NOT_SUPPORTED;
            break;
        }
    }*/

    return MEDIA_VISION_ERROR_NONE;
}

void ParseOpenCVLabels(
        const cv::Ptr<cv::FaceRecognizer>& recognizer,
        std::set<int>& outLabels)
{
    if (!recognizer.empty())
    {
        cv::Mat labels = recognizer->getMat("labels");
        for(int i = 0; i < labels.rows; ++i)
        {
            outLabels.insert(labels.at<int>(i, 0));
        }
    }
}

} /* anonymous namespace */

FaceRecognitionModelConfig::FaceRecognitionModelConfig() :
        mModelType(MEDIA_VISION_FACE_MODEL_TYPE_UNKNOWN),
        mNumComponents(0),
        mThreshold(DBL_MAX),
        mRadius(1),
        mNeighbors(8),
        mGridX(8),
        mGridY(8),
        mImgWidth(150),
        mImgHeight(150)
{
    ; /* NULL */
}

FaceRecognitionResults::FaceRecognitionResults() :
        mIsRecognized(false),
        mFaceLabel(-1),
        mConfidence(0.0)
{
    ; /* NULL */
}

bool FaceRecognitionModelConfig::operator!=(
        const FaceRecognitionModelConfig& other) const
{
    return mModelType     != other.mModelType     ||
           mNumComponents != other.mNumComponents ||
           mThreshold     != other.mThreshold     ||
           mRadius        != other.mRadius        ||
           mNeighbors     != other.mNeighbors     ||
           mGridX         != other.mGridX         ||
           mGridY         != other.mGridY         ||
           mImgWidth      != other.mImgWidth      ||
           mImgHeight     != other.mImgHeight;
}

FaceRecognitionModel::FaceRecognitionModel() :
        m_canRecognize(false),
        m_recognizer(NULL)
{
    ; /* NULL */
}

FaceRecognitionModel::FaceRecognitionModel(const FaceRecognitionModel& origin) :
        m_canRecognize(origin.m_canRecognize),
        m_faceSamples(origin.m_faceSamples),
        m_learnAlgorithmConfig(origin.m_learnAlgorithmConfig),
        m_recognizer(CreateRecognitionAlgorithm(origin.m_learnAlgorithmConfig)),
        m_learnedLabels(origin.m_learnedLabels)
{
    if (!m_recognizer.empty())
    {
        CopyOpenCVAlgorithmParameters(origin.m_recognizer, m_recognizer);
    }
}

FaceRecognitionModel& FaceRecognitionModel::operator=(
        const FaceRecognitionModel& copy)
{
    if (this != &copy)
    {
        m_canRecognize = copy.m_canRecognize;
        m_faceSamples = copy.m_faceSamples;
        m_learnAlgorithmConfig = copy.m_learnAlgorithmConfig;
        m_recognizer = CreateRecognitionAlgorithm(m_learnAlgorithmConfig);
        m_learnedLabels = copy.m_learnedLabels;

        if (!m_recognizer.empty())
        {
            CopyOpenCVAlgorithmParameters(copy.m_recognizer, m_recognizer);
        }
    }

    return *this;
}

FaceRecognitionModel::~FaceRecognitionModel()
{
    ; /* NULL */
}

int FaceRecognitionModel::save(const std::string& fileName)
{
    if (!m_recognizer.empty())
    {
        /* find directory */
        std::string prefix_path = std::string(app_get_data_path());
        LOGD("prefix_path: %s", prefix_path.c_str());

        std::string filePath;
        filePath += prefix_path;
        filePath += fileName;

        /* check the directory is available */
        std::string prefix_path_check = filePath.substr(0, filePath.find_last_of('/'));
        if (access(prefix_path_check.c_str(),F_OK))
        {
            LOGE("Can't save recognition model. Path[%s] doesn't existed.", prefix_path_check.c_str());

            return MEDIA_VISION_ERROR_INVALID_PATH;
        }

        cv::FileStorage storage(filePath, cv::FileStorage::WRITE);
        if (!storage.isOpened())
        {
            LOGE("Can't save recognition model. Write to file permission denied.");
            return MEDIA_VISION_ERROR_PERMISSION_DENIED;
        }

        switch (m_learnAlgorithmConfig.mModelType)
        {
            case MEDIA_VISION_FACE_MODEL_TYPE_EIGENFACES:
                storage << "algorithm" << "Eigenfaces";
                break;
            case MEDIA_VISION_FACE_MODEL_TYPE_FISHERFACES:
                storage << "algorithm" << "Fisherfaces";
                break;
            case MEDIA_VISION_FACE_MODEL_TYPE_LBPH:
                storage << "algorithm" << "LBPH";
                break;
            default:
                storage.release();
                return MEDIA_VISION_ERROR_NOT_SUPPORTED_FORMAT;
        }

        storage << "can_recognize" << m_canRecognize;
        m_recognizer->save(storage);

        storage.release();
    }
    else
    {
        LOGE("Attempt to save recognition model before learn");
        return MEDIA_VISION_ERROR_INVALID_OPERATION;
    }

    return MEDIA_VISION_ERROR_NONE;
}

int FaceRecognitionModel::load(const std::string& fileName)
{
    /* find directory */
    std::string prefix_path = std::string(app_get_data_path());
    LOGD("prefix_path: %s", prefix_path.c_str());

    std::string filePath;
    filePath += prefix_path;
    filePath += fileName;

    if (access(filePath.c_str(),F_OK))
    {
        LOGE("Can't load face recognition model. File[%s] doesn't exist.", filePath.c_str());

        return MEDIA_VISION_ERROR_INVALID_PATH;
    }

    cv::FileStorage storage(filePath, cv::FileStorage::READ);
    if (!storage.isOpened())
    {
        LOGE("Can't load recognition model. Read from file permission denied.");

        return MEDIA_VISION_ERROR_PERMISSION_DENIED;
    }

    LOGD("Loading recognition model from file.");

    std::string algName;
    int canRecognize = 0;
    storage["algorithm"] >> algName;
    storage["can_recognize"] >> canRecognize;

    cv::Ptr<cv::FaceRecognizer> tempRecognizer;
    FaceRecognitionModelConfig tempConfig;
    std::set<int> tempLearnedLabels;

    if (algName == "Eigenfaces")
    {
        tempRecognizer = cv::createEigenFaceRecognizer();
        tempRecognizer->load(storage);
        tempConfig.mModelType =
                MEDIA_VISION_FACE_MODEL_TYPE_EIGENFACES;
        tempConfig.mNumComponents =
                tempRecognizer->getInt("ncomponents");
        ParseOpenCVLabels(tempRecognizer, tempLearnedLabels);
    }
    else if (algName == "Fisherfaces")
    {
        tempRecognizer = cv::createFisherFaceRecognizer();
        tempRecognizer->load(storage);
        tempConfig.mModelType =
                MEDIA_VISION_FACE_MODEL_TYPE_FISHERFACES;
        tempConfig.mNumComponents =
                tempRecognizer->getInt("ncomponents");
        ParseOpenCVLabels(tempRecognizer, tempLearnedLabels);
    }
    else if (algName == "LBPH")
    {
        tempRecognizer = cv::createLBPHFaceRecognizer();
        tempRecognizer->load(storage);
        tempConfig.mModelType =
                MEDIA_VISION_FACE_MODEL_TYPE_LBPH;
        tempConfig.mGridX = tempRecognizer->getInt("grid_x");
        tempConfig.mGridY = tempRecognizer->getInt("grid_y");
        tempConfig.mNeighbors = tempRecognizer->getInt("neighbors");
        tempConfig.mRadius = tempRecognizer->getInt("radius");
        ParseOpenCVLabels(tempRecognizer, tempLearnedLabels);
    }
    else
    {
        tempConfig = FaceRecognitionModelConfig();
        LOGE("Failed to load face recognition model from file. File is in "
             "unsupported format");

        storage.release();

        return MEDIA_VISION_ERROR_NOT_SUPPORTED_FORMAT;
    }

    tempConfig.mThreshold = tempRecognizer->getDouble("threshold");

    LOGD("Recognition model of [%s] type has been loaded from file",
            algName.c_str());

    storage.release();

    m_recognizer = tempRecognizer;
    m_learnAlgorithmConfig = tempConfig;
    m_canRecognize = (canRecognize == 1);
    m_learnedLabels.clear();
    m_learnedLabels = tempLearnedLabels;

    return MEDIA_VISION_ERROR_NONE;
}

int FaceRecognitionModel::addFaceExample(
        const cv::Mat& faceImage,
        int faceLabel)
{
    m_faceSamples[faceLabel].push_back(faceImage);

    LOGD("Added face image example for label %i for recognition model",
            faceLabel);

    return MEDIA_VISION_ERROR_NONE;
}

int FaceRecognitionModel::resetFaceExamples(void)
{
    m_faceSamples.clear();

    LOGD("All face image examples have been removed from recognition model");

    return MEDIA_VISION_ERROR_NONE;
}

int FaceRecognitionModel::resetFaceExamples(int faceLabel)
{
    if (1 > m_faceSamples.erase(faceLabel))
    {
        LOGD("Failed to remove face image examples for label %i. "
             "No such examples", faceLabel);

        return MEDIA_VISION_ERROR_KEY_NOT_AVAILABLE;
    }

    LOGD("Face image examples for label %i have been removed from "
         "recognition model", faceLabel);

    return MEDIA_VISION_ERROR_NONE;
}

const std::set<int>& FaceRecognitionModel::getFaceLabels(void) const
{
    return m_learnedLabels;
}

int FaceRecognitionModel::learn(const FaceRecognitionModelConfig& config)
{
    bool isIncremental = false;
    bool isUnisize = false;

    if (MEDIA_VISION_FACE_MODEL_TYPE_LBPH == config.mModelType)
    {
        isIncremental = true;
    }

    if (MEDIA_VISION_FACE_MODEL_TYPE_EIGENFACES == config.mModelType ||
            MEDIA_VISION_FACE_MODEL_TYPE_FISHERFACES == config.mModelType)
    {
        isUnisize = true;
    }

    std::vector<cv::Mat> samples;
    std::vector<int> labels;
    std::set<int> learnedLabels;

    if (isIncremental)
    {
        learnedLabels.insert(m_learnedLabels.begin(), m_learnedLabels.end());
    }

    std::map<int, std::vector<cv::Mat> >::const_iterator it =
            m_faceSamples.begin();
    for (; it != m_faceSamples.end(); ++it)
    {
        const size_t faceClassSamplesSize = it->second.size();
        labels.insert(labels.end(), faceClassSamplesSize, it->first);
        learnedLabels.insert(it->first);

        if (!isUnisize)
        {
            LOGD("%u examples has been added with label %i",
                    it->second.size(), it->first);
            samples.insert(samples.end(), it->second.begin(), it->second.end());
        }
        else
        {
            for (size_t sampleInd = 0; sampleInd < faceClassSamplesSize; ++sampleInd)
            {
                cv::Mat resizedSample;
                cv::resize(it->second[sampleInd],
                           resizedSample,
                           cv::Size(config.mImgWidth, config.mImgHeight),
                           1.0, 1.0, cv::INTER_CUBIC);
                samples.push_back(resizedSample);
            }
        }
    }

    const size_t samplesSize = samples.size();
    const size_t labelsSize = labels.size();

    if (0 != samplesSize && samplesSize == labelsSize)
    {
        LOGD("Start to learn the model for %u samples and %u labels",
             samplesSize, labelsSize);

        if (m_learnAlgorithmConfig != config || m_recognizer.empty())
        {
            m_recognizer = CreateRecognitionAlgorithm(config);
        }

        if (m_recognizer.empty())
        {
            LOGE("Can't create recognition algorithm for recognition model. "
                 "Configuration is not supported by any of known algorithms.");

            return MEDIA_VISION_ERROR_NOT_SUPPORTED;
        }

        isIncremental ? m_recognizer->update(samples, labels) :
                        m_recognizer->train(samples, labels);
        m_canRecognize = true;
        m_learnedLabels.clear();
        m_learnedLabels = learnedLabels;
    }
    else
    {
        LOGE("Can't create recognition algorithm for no examples. Try to add "
                "some face examples before learning");

        return MEDIA_VISION_ERROR_NO_DATA;
    }

    m_learnAlgorithmConfig = config;

    LOGD("Recognition model has been learned");

    return MEDIA_VISION_ERROR_NONE;
}

int FaceRecognitionModel::recognize(const cv::Mat& image, FaceRecognitionResults& results)
{
    if (!m_recognizer.empty() && m_canRecognize)
    {
        double absConf = 0.0;
        m_recognizer->predict(image, results.mFaceLabel, absConf);
        // Normalize the absolute value of the confidence
        absConf = exp(7.5 - (0.05 * absConf));
        results.mConfidence = absConf / (1 + absConf);
        results.mIsRecognized = true;
        results.mFaceLocation = cv::Rect(0, 0, image.cols, image.rows);
    }
    else
    {
        LOGE("Attempt to recognize faces with untrained model");
        return MEDIA_VISION_ERROR_INVALID_OPERATION;
    }

    return MEDIA_VISION_ERROR_NONE;
}

cv::Ptr<cv::FaceRecognizer> FaceRecognitionModel::CreateRecognitionAlgorithm(
        const FaceRecognitionModelConfig& config)
{
    cv::Ptr<cv::FaceRecognizer> tempRecognizer;
    switch (config.mModelType)
    {
        case MEDIA_VISION_FACE_MODEL_TYPE_EIGENFACES:
            tempRecognizer = cv::createEigenFaceRecognizer(
                                     config.mNumComponents,
                                     config.mThreshold);
            break;
        case MEDIA_VISION_FACE_MODEL_TYPE_FISHERFACES:
            tempRecognizer = cv::createFisherFaceRecognizer(
                                     config.mNumComponents,
                                     config.mThreshold);
            break;
        case MEDIA_VISION_FACE_MODEL_TYPE_LBPH:
            tempRecognizer = cv::createLBPHFaceRecognizer(
                                     config.mRadius,
                                     config.mNeighbors,
                                     config.mGridX,
                                     config.mGridY,
                                     config.mThreshold);
            break;
        default:
            return NULL;
    }

    return tempRecognizer;
}

} /* Face */
} /* MediaVision */
