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

#include "EngineConfig.h"

#include <mv_private.h>

#include <json/json.h>

/**
 * @file   EngineConfig.cpp
 * @brief  Engine Configuration class methods implementation.
 */

namespace MediaVision
{
namespace Common
{

std::string EngineConfig::DefConfigFilePath =
    std::string("/usr/share/config/capi-media-vision/media-vision-config.json");

std::map<std::string, double> EngineConfig::DefDblDict;
std::map<std::string, int> EngineConfig::DefIntDict;
std::map<std::string, bool> EngineConfig::DefBoolDict;
std::map<std::string, std::string> EngineConfig::DefStrDict;

EngineConfig::EngineConfig()
{
    // Force load default attributes from configuration file
    cacheDictionaries(false);

    // Insert default attribute values into creating engine configuration
    m_dblDict.insert(getDefaultDblDict().begin(), getDefaultDblDict().end());
    m_intDict.insert(getDefaultIntDict().begin(), getDefaultIntDict().end());
    m_boolDict.insert(getDefaultBoolDict().begin(), getDefaultBoolDict().end());
    m_strDict.insert(getDefaultStrDict().begin(), getDefaultStrDict().end());
}

EngineConfig::~EngineConfig()
{
    ; /* NULL */
}

int EngineConfig::setAttribute(const std::string& key, const double value)
{
    LOGI("Set double attribute for the engine config %p. [%s] = %f",
            this, key.c_str(), value);

    if (m_dblDict.find(key) == m_dblDict.end())
    {
        LOGE("Double attribute [%s] can't be set because isn't supported", key.c_str());
        return MEDIA_VISION_ERROR_KEY_NOT_AVAILABLE;
    }

    m_dblDict[key] = value;

    return MEDIA_VISION_ERROR_NONE;
}

int EngineConfig::setAttribute(const std::string& key, const int value)
{
    LOGI("Set integer attribute for the engine config %p. [%s] = %i",
            this, key.c_str(), value);

    if (m_intDict.find(key) == m_intDict.end())
    {
        LOGE("Integer attribute [%s] can't be set because isn't supported", key.c_str());
        return MEDIA_VISION_ERROR_KEY_NOT_AVAILABLE;
    }

    m_intDict[key] = value;

    return MEDIA_VISION_ERROR_NONE;
}

int EngineConfig::setAttribute(const std::string& key, const bool value)
{
    LOGI("Set boolean attribute for the engine config %p. [%s] = %s",
            this, key.c_str(), value ? "TRUE" : "FALSE");

    if (m_boolDict.find(key) == m_boolDict.end())
    {
        LOGE("Boolean attribute [%s] can't be set because isn't supported", key.c_str());
        return MEDIA_VISION_ERROR_KEY_NOT_AVAILABLE;
    }

    m_boolDict[key] = value;

    return MEDIA_VISION_ERROR_NONE;
}

int EngineConfig::setAttribute(const std::string& key, const std::string& value)
{
    LOGI("Set string attribute for the engine config %p. [%s] = %s",
            this, key.c_str(), value.c_str());

    if (m_strDict.find(key) == m_strDict.end())
    {
        LOGE("String attribute [%s] can't be set because isn't supported", key.c_str());
        return MEDIA_VISION_ERROR_KEY_NOT_AVAILABLE;
    }

    m_strDict[key] = value;

    return MEDIA_VISION_ERROR_NONE;
}

int EngineConfig::getDoubleAttribute(const std::string& key, double *value) const
{
    DictDblConstIter dictIter = m_dblDict.find(key);
    if (dictIter == m_dblDict.end())
    {
        LOGE("Attempt to access to the unsupported double attribute [%s] "
                "of the engine config %p", key.c_str(), this);
        return MEDIA_VISION_ERROR_KEY_NOT_AVAILABLE;
    }

    LOGD("Get double attribute from the engine config %p. [%s] = %f",
            this, dictIter->first.c_str(), dictIter->second);

    *value = dictIter->second;

    return MEDIA_VISION_ERROR_NONE;
}

int EngineConfig::getIntegerAttribute(const std::string& key, int *value) const
{
    DictIntConstIter dictIter = m_intDict.find(key);
    if (dictIter == m_intDict.end())
    {
        LOGE("Attempt to access to the unsupported integer attribute [%s] "
                "of the engine config %p", key.c_str(), this);
        return MEDIA_VISION_ERROR_KEY_NOT_AVAILABLE;
    }

    LOGD("Get integer attribute from the engine config %p. [%s] = %i",
            this, dictIter->first.c_str(), dictIter->second);

    *value = dictIter->second;

    return MEDIA_VISION_ERROR_NONE;
}

int EngineConfig::getBooleanAttribute(const std::string& key, bool *value) const
{
    DictBoolConstIter dictIter = m_boolDict.find(key);
    if (dictIter == m_boolDict.end())
    {
        LOGE("Attempt to access to the unsupported boolean attribute [%s] "
                "of the engine config %p", key.c_str(), this);
        return MEDIA_VISION_ERROR_KEY_NOT_AVAILABLE;
    }

    LOGD("Get boolean attribute from the engine config %p. [%s] = %s",
            this, dictIter->first.c_str(), dictIter->second ? "TRUE" : "FALSE");

    *value = dictIter->second;

    return MEDIA_VISION_ERROR_NONE;
}

int EngineConfig::getStringAttribute(const std::string& key, std::string *value) const
{
    DictStrConstIter dictIter = m_strDict.find(key);
    if (dictIter == m_strDict.end())
    {
        LOGE("Attempt to access to the unsupported string attribute [%s] "
                "of the engine config %p", key.c_str(), this);
        return MEDIA_VISION_ERROR_KEY_NOT_AVAILABLE;
    }

    LOGD("Get string attribute from the engine config %p. [%s] = %s",
            this, dictIter->first.c_str(), dictIter->second.c_str());

    *value = dictIter->second;

    return MEDIA_VISION_ERROR_NONE;
}

// static

bool EngineConfig::setDefaultConfigFilePath(const std::string& confFilePath)
{
    if (0 != DefConfigFilePath.compare(confFilePath))
    {
        DefConfigFilePath = confFilePath;
        return true;
    }

    return false;
}

const std::map<std::string, double>& EngineConfig::getDefaultDblDict()
{
    cacheDictionaries();

    return DefDblDict;
}

const std::map<std::string, int>& EngineConfig::getDefaultIntDict()
{
    cacheDictionaries();

    return DefIntDict;
}

const std::map<std::string, bool>& EngineConfig::getDefaultBoolDict()
{
    cacheDictionaries();

    return DefBoolDict;
}

const std::map<std::string, std::string>& EngineConfig::getDefaultStrDict()
{
    cacheDictionaries();

    return DefStrDict;
}

int EngineConfig::cacheDictionaries(bool isLazyCache, std::string configFilePath)
{
    static bool isCached = false;
    if (!isLazyCache || !isCached)
    {
        LOGI("Start to cache default attributes from engine configuration file.");

        DefDblDict.clear();
        DefIntDict.clear();
        DefBoolDict.clear();
        DefStrDict.clear();

        json_object *jobj = json_object_from_file(configFilePath.c_str());

        enum json_type type = json_object_get_type(jobj);
        if (json_type_object != type)
        {
            LOGE("Can't parse engine config file. Incorrect json markup. "
                    "Supported attributes can't be determined.");
            json_object_put(jobj);
            return MEDIA_VISION_ERROR_NO_DATA;
        }

        json_object *pAttributesObj = json_object_object_get(jobj, "attributes");
        type = json_object_get_type(pAttributesObj);
        if (json_type_array != type)
        {
            LOGE("Can't parse engine config file. Incorrect json markup. "
                    "Supported attributes can't be determined.");
            json_object_put(jobj);
            return MEDIA_VISION_ERROR_NO_DATA;
        }

        const int attrNum = json_object_array_length(pAttributesObj);

        for (int attrInd = 0; attrInd < attrNum; ++attrInd)
        {
            json_object *pAttrObj =
                    json_object_array_get_idx(pAttributesObj, attrInd);
            type = json_object_get_type(pAttrObj);

            json_object *pAttrNameObj = NULL;
            json_object *pAttrTypeObj = NULL;
            json_object *pAttrValueObj = NULL;

            if (json_type_object != type ||
                !json_object_object_get_ex(pAttrObj, "name", &pAttrNameObj) ||
                !json_object_object_get_ex(pAttrObj, "type", &pAttrTypeObj) ||
                !json_object_object_get_ex(pAttrObj, "value", &pAttrValueObj))
            {
                LOGW("Attribute %i wasn't parsed from json file.", attrInd);
                continue;
            }

            const char *nameStr = json_object_get_string(pAttrNameObj);
            const char *typeStr = json_object_get_string(pAttrTypeObj);

            if (0 == strcmp("double", typeStr))
            {
                DefDblDict[std::string(nameStr)] =
                        json_object_get_double(pAttrValueObj);
            }
            else if (0 == strcmp("integer", typeStr))
            {
                DefIntDict[std::string(nameStr)] =
                        json_object_get_int(pAttrValueObj);
            }
            else if (0 == strcmp("boolean", typeStr))
            {
                DefBoolDict[std::string(nameStr)] =
                        json_object_get_boolean(pAttrValueObj) ? true : false;
            }
            else if (0 == strcmp("string", typeStr))
            {
                DefStrDict[std::string(nameStr)] =
                        json_object_get_string(pAttrValueObj);
            }
            else
            {
                LOGW("Attribute %i:%s wasn't parsed from json file. Type isn't supported.", attrInd, nameStr);
                continue;
            }
        }

        json_object_put(jobj);
        isCached = true;
    }

    return MEDIA_VISION_ERROR_NONE;
}

} /* namespace Common */
} /* namespace MediaVision */
