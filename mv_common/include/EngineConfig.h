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

#ifndef __ENGINECONFIG_H__
#define __ENGINECONFIG_H__

#include <string>
#include <map>

#include "mv_common.h"

/**
 * @file   EngineConfig.h
 * @brief  Engine Configuration class definition.
 */

namespace MediaVision {
namespace Common {

typedef std::map<std::string, double>::const_iterator DictDblConstIter;
typedef std::map<std::string, int>::const_iterator DictIntConstIter;
typedef std::map<std::string, bool>::const_iterator DictBoolConstIter;
typedef std::map<std::string, std::string>::const_iterator DictStrConstIter;

class EngineConfig {
public:
	/**
	 * @brief Engine configuration constructor.
	 * @details Create new engine configuration dictionary and set default
	 *          attributes values.
	 *
	 * @since_tizen @if MOBILE 2.4 @else 3.0 @endif
	 */
	EngineConfig();

	/**
	 * @brief Engine configuration destructor.
	 */
	virtual ~EngineConfig();

	/**
	 * @brief Sets attribute with double value.
	 *
	 * @since_tizen @if MOBILE 2.4 @else 3.0 @endif
	 * @param [in] key          The string name of the attribute
	 * @param [in] value        The double attribute value to be set
	 * @return @c MEDIA_VISION_ERROR_NONE on success,\n
	 *         otherwise a negative error value
	 */
	int setAttribute(const std::string& key, const double value);

	/**
	 * @brief Sets attribute with integer value.
	 *
	 * @since_tizen @if MOBILE 2.4 @else 3.0 @endif
	 * @param [in] key          The string name of the attribute
	 * @param [in] value        The integer attribute value to be set
	 * @return @c MEDIA_VISION_ERROR_NONE on success,\n
	 *         otherwise a negative error value
	 */
	int setAttribute(const std::string& key, const int value);

	/**
	 * @brief Sets attribute with boolean value.
	 *
	 * @since_tizen @if MOBILE 2.4 @else 3.0 @endif
	 * @param [in] key          The string name of the attribute
	 * @param [in] value        The boolean attribute value to be set
	 * @return @c MEDIA_VISION_ERROR_NONE on success,\n
	 *         otherwise a negative error value
	 */
	int setAttribute(const std::string& key, const bool value);

	/**
	 * @brief Sets attribute with string value.
	 *
	 * @since_tizen @if MOBILE 2.4 @else 3.0 @endif
	 * @param [in] key          The string name of the attribute
	 * @param [in] value        The string attribute value to be set
	 * @return @c MEDIA_VISION_ERROR_NONE on success,\n
	 *         otherwise a negative error value
	 */
	int setAttribute(const std::string& key, const std::string& value);

	/**
	 * @brief Gets double attribute value by attribute name.
	 *
	 * @since_tizen @if MOBILE 2.4 @else 3.0 @endif
	 * @param [in]  key          The string name of the attribute
	 * @param [out] value   r    The double attribute value to be obtained
	 * @return @c MEDIA_VISION_ERROR_NONE on success,\n
	 *         otherwise a negative error value
	 * @retval #MEDIA_VISION_ERROR_KEY_NOT_AVAILABLE If attribute with name @a key
	 *         doesn't exist in the engine configuration dictionary
	 */
	int getDoubleAttribute(const std::string& key, double *value) const;

	/**
	 * @brief Gets integer attribute value by attribute name.
	 *
	 * @since_tizen @if MOBILE 2.4 @else 3.0 @endif
	 * @param [in]  key          The string name of the attribute
	 * @param [out] value        The integer attribute value to be obtained
	 * @return @c MEDIA_VISION_ERROR_NONE on success,\n
	 *         otherwise a negative error value
	 * @retval #MEDIA_VISION_ERROR_KEY_NOT_AVAILABLE If attribute with name @a key
	 *         doesn't exist in the engine configuration dictionary
	 */
	int getIntegerAttribute(const std::string& key, int *value) const;

/**
	 * @brief Gets boolean attribute value by attribute name.
	 *
	 * @since_tizen @if MOBILE 2.4 @else 3.0 @endif
	 * @param [in]  key          The string name of the attribute
	 * @param [out] value        The boolean attribute value to be obtained
	 * @return @c MEDIA_VISION_ERROR_NONE on success,\n
	 *         otherwise a negative error value
	 * @retval #MEDIA_VISION_ERROR_KEY_NOT_AVAILABLE If attribute with name @a key
	 *         doesn't exist in the engine configuration dictionary
	 */
	int getBooleanAttribute(const std::string& key, bool *value) const;

	/**
	 * @brief Gets string attribute value by attribute name.
	 *
	 * @since_tizen @if MOBILE 2.4 @else 3.0 @endif
	 * @param [in]  key          The string name of the attribute
	 * @param [out] value        The string attribute value to be obtained
	 * @return @c MEDIA_VISION_ERROR_NONE on success,\n
	 *         otherwise a negative error value
	 * @retval #MEDIA_VISION_ERROR_KEY_NOT_AVAILABLE If attribute with name @a key
	 *         doesn't exist in the engine configuration dictionary
	 */
	int getStringAttribute(const std::string& key, std::string *value) const;

public:
	static bool setDefaultConfigFilePath(const std::string& confFilePath);

	static const std::map<std::string, double>& getDefaultDblDict();
	static const std::map<std::string, int>& getDefaultIntDict();
	static const std::map<std::string, bool>& getDefaultBoolDict();
	static const std::map<std::string, std::string>& getDefaultStrDict();
	static int cacheDictionaries(
			bool isLazyCache = true,
			std::string configFilePath = DefConfigFilePath);

private:
	std::map<std::string, double> m_dblDict;
	std::map<std::string, int> m_intDict;
	std::map<std::string, bool> m_boolDict;
	std::map<std::string, std::string> m_strDict;

private:
	static std::string DefConfigFilePath;

	static std::map<std::string, double> DefDblDict;
	static std::map<std::string, int> DefIntDict;
	static std::map<std::string, bool> DefBoolDict;
	static std::map<std::string, std::string> DefStrDict;
};

} /* Common */
} /* MediaVision */

#endif /* __ENGINECONFIG_H__ */
