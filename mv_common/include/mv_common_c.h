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

#ifndef __TIZEN_MEDIAVISION_COMMON_C_H__
#define __TIZEN_MEDIAVISION_COMMON_C_H__

#include "mv_common.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @file  mv_common_c.h
 * @brief This file contains the Media Vision Common module API.
 */

/**
 * @brief Creates a source handle.
 *
 * @since_tizen 2.4
 * @remarks You must release @a source by using @ref mv_destroy_source_c().
 * @param [out] source    A new handle to the source
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @see mv_destroy_source_c()
 */
int mv_create_source_c(
        mv_source_h *source);

/**
 * @brief Destroys the source handle and releases all its resources.
 *
 * @since_tizen 2.4
 * @param [in] source    The handle to the source to be destroyed
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see mv_create_source_c()
 */
int mv_destroy_source_c(
        mv_source_h source);

/**
 * @brief Fills the media source based on the media packet.
 *
 * @since_tizen 2.4
 * @param [in,out] source          The handle to the source
 * @param [in]     media_packet    The handle to the media packet from which
 *                                 will be filled the source
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_INVALID_OPERATION Invalid operation
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported media format
 * @retval #MEDIA_VISION_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @pre Create a source handle by calling @ref mv_create_source_c()
 *
 * @see mv_create_source_c()
 * @see mv_destroy_source_c()
 */
int mv_source_fill_by_media_packet_c(
        mv_source_h source,
        media_packet_h media_packet);

/**
 * @brief Fills the media source based on the buffer and metadata.
 *
 * @since_tizen 2.4
 * @param [in,out] source             The handle to the source
 * @param [in]     data_buffer        The buffer of image data
 * @param [in]     buffer_size        The buffer size
 * @param [in]     image_width        The image width
 * @param [in]     image_height       The image height
 * @param [in]     image_colorspace   The image colorspace
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @pre Create a source handle by calling @ref mv_create_source_c()
 *
 * @see mv_source_clear_c()
 */
int mv_source_fill_by_buffer_c(
        mv_source_h source,
        unsigned char *data_buffer,
        unsigned int buffer_size,
        unsigned int image_width,
        unsigned int image_height,
        mv_colorspace_e image_colorspace);

/**
 * @brief Clears the buffer of the media source.
 *
 * @since_tizen 2.4
 * @param [in] source    The handle to the source
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see mv_source_fill_by_buffer_c()
 */
int mv_source_clear_c(
        mv_source_h source);

/**
 * @brief Gets buffer of the media source.
 *
 * @since_tizen 2.4
 * @remarks Note that the retrieved buffer will be destroyed when
 *          @ref mv_destroy_source_c() or @ref mv_source_clear_c() function
 *          is called for the @a source.
 *
 * @param [in]    source        The handle to the source
 * @param [out]   data_buffer   The buffer of the source
 * @param [out]   buffer_size   The size of buffer
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see mv_source_get_width_c()
 * @see mv_source_get_height_c()
 * @see mv_source_get_colorspace_c()
 */
int mv_source_get_buffer_c(
        mv_source_h source,
        unsigned char **data_buffer,
        unsigned int *buffer_size);

/**
 * @brief Gets height of the media source.
 *
 * @since_tizen 2.4
 * @param [in]    source         The handle to the source
 * @param [out]   image_height   The height of the source
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see mv_source_get_width_c()
 * @see mv_source_get_colorspace_c()
 * @see mv_source_get_buffer_c()
 */
int mv_source_get_height_c(
        mv_source_h source,
        unsigned int *image_height);

/**
 * @brief Gets width of the media source.
 *
 * @since_tizen 2.4
 * @param [in]    source        The handle to the source
 * @param [out]   image_width   The width of the source
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see mv_source_get_height_c()
 * @see mv_source_get_colorspace_c()
 * @see mv_source_get_buffer_c()
 */
int mv_source_get_width_c(
        mv_source_h source,
        unsigned int *image_width);

/**
 * @brief Gets colorspace of the media source.
 *
 * @since_tizen 2.4
 * @param [in]    source             The handle to the source
 * @param [out]   image_colorspace   The colorspace of the source
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see mv_source_get_width_c()
 * @see mv_source_get_height_c()
 * @see mv_source_get_buffer_c()
 */
int mv_source_get_colorspace_c(
        mv_source_h source,
        mv_colorspace_e *image_colorspace);

/**
 * @brief Creates the handle to the configuration of engine.
 *
 * @since_tizen 2.4
 * @param [out] engine_cfg    The handle to the engine to be created
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @see mv_engine_config_h
 * @see mv_destroy_engine_config_c()
 * @see mv_engine_config_set_double_attribute_c()
 * @see mv_engine_config_set_int_attribute_c()
 * @see mv_engine_config_set_bool_attribute_c()
 * @see mv_engine_config_set_string_attribute_c()
 * @see mv_engine_config_get_double_attribute_c()
 * @see mv_engine_config_get_int_attribute_c()
 * @see mv_engine_config_get_bool_attribute_c()
 * @see mv_engine_config_get_string_attribute_c()
 */
int mv_create_engine_config_c(
        mv_engine_config_h *engine_cfg);

/**
 * @brief Destroys the engine configuration handle and releases all its
 *        resources.
 *
 * @since_tizen 2.4
 * @param [in] engine_cfg    The handle to the engine configuration
 *                           to be destroyed
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see mv_engine_config_h
 * @see mv_create_engine_config_c()
 */
int mv_destroy_engine_config_c(
        mv_engine_config_h engine_cfg);

/**
 * @brief Sets the double attribute to the configuration.
 *
 * @since_tizen 2.4
 * @param [in] engine_cfg        Engine configuration for which @a value has
 *                               to be set
 * @param [in] name              String key of the attribute will be used for
 *                               storing the @a value into configuration
 *                               dictionary
 * @param [in] value             The double value of the attribute
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see mv_engine_config_get_double_attribute_c()
 * @see mv_engine_config_set_int_attribute_c()
 * @see mv_engine_config_set_bool_attribute_c()
 * @see mv_engine_config_set_string_attribute_c()
 */
int mv_engine_config_set_double_attribute_c(
        mv_engine_config_h engine_cfg,
        const char *name,
        double value);

/**
 * @brief Sets the integer attribute to the configuration.
 *
 * @since_tizen 2.4
 * @param [in] engine_cfg        Engine configuration for which @a value has
 *                               to be set
 * @param [in] name              String key of the attribute will be used for
 *                               storing the @a value into configuration
 *                               dictionary
 * @param [in] value             The integer value of the attribute
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see mv_engine_config_get_int_attribute_c()
 * @see mv_engine_config_set_double_attribute_c()
 * @see mv_engine_config_set_bool_attribute_c()
 * @see mv_engine_config_set_string_attribute_c()
 */
int mv_engine_config_set_int_attribute_c(
        mv_engine_config_h engine_cfg,
        const char *name,
        int value);

/**
 * @brief Sets the boolean attribute to the configuration.
 *
 * @since_tizen 2.4
 * @param [in] engine_cfg        Engine configuration for which @a value has
 *                               to be set
 * @param [in] name              String key of the attribute will be used for
 *                               storing the @a value into configuration
 *                               dictionary
 * @param [in] value             The boolean value of the attribute
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see mv_engine_config_get_bool_attribute_c()
 * @see mv_engine_config_set_double_attribute_c()
 * @see mv_engine_config_set_int_attribute_c()
 * @see mv_engine_config_set_string_attribute_c()
 */
int mv_engine_config_set_bool_attribute_c(
        mv_engine_config_h engine_cfg,
        const char *name,
        bool attribute);

/**
 * @brief Sets the string attribute to the configuration.
 *
 * @since_tizen 2.4
 * @param [in] engine_cfg        Engine configuration for which @a value has
 *                               to be set
 * @param [in] name              String key of the attribute will be used for
 *                               storing the @a value into configuration
 *                               dictionary
 * @param [in] value             The string value of the attribute
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see mv_engine_config_get_string_attribute_c()
 * @see mv_engine_config_set_double_attribute_c()
 * @see mv_engine_config_set_int_attribute_c()
 * @see mv_engine_config_set_bool_attribute_c()
 */
int mv_engine_config_set_string_attribute_c(
        mv_engine_config_h engine_cfg,
        const char *name,
        const char *value);

/**
 * @brief Gets the double attribute from the configuration dictionary.
 *
 * @since_tizen 2.4
 * @param [in] engine_cfg        Engine configuration from which @a value
 *                               has to be gotten
 * @param [in] name              String key of the attribute will be used for
 *                               getting the @a value from the
 *                               configuration dictionary
 * @param [out] value            The attribute to be filled with double value
 *                               from dictionary
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_KEY_NOT_AVAILABLE Parameter key isn't available
 *
 * @see mv_engine_config_set_double_attribute_c()
 * @see mv_engine_config_get_int_attribute_c()
 * @see mv_engine_config_get_bool_attribute_c()
 * @see mv_engine_config_get_string_attribute_c()
 */
int mv_engine_config_get_double_attribute_c(
        mv_engine_config_h engine_cfg,
        const char *name,
        double *value);

/**
 * @brief Gets the integer attribute from the configuration dictionary.
 *
 * @since_tizen 2.4
 * @param [in] engine_cfg        Engine configuration from which @a value
 *                               has to be gotten
 * @param [in] name              String key of the attribute will be used for
 *                               getting the @a value from the
 *                               configuration dictionary
 * @param [out] value            The attribute to be filled with integer value
 *                               from dictionary
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_KEY_NOT_AVAILABLE Parameter key isn't available
 *
 * @see mv_engine_config_set_int_attribute_c()
 * @see mv_engine_config_get_double_attribute_c()
 * @see mv_engine_config_get_bool_attribute_c()
 * @see mv_engine_config_get_string_attribute_c()
 */
int mv_engine_config_get_int_attribute_c(
        mv_engine_config_h engine_cfg,
        const char *name,
        int *value);

/**
 * @brief Gets the boolean attribute from the configuration dictionary.
 *
 * @since_tizen 2.4
 * @param [in] engine_cfg        Engine configuration from which @a value
 *                               has to be gotten
 * @param [in] name              String key of the attribute will be used for
 *                               getting the @a value from the
 *                               configuration dictionary
 * @param [out] value            The attribute to be filled with boolean value
 *                               from dictionary
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_KEY_NOT_AVAILABLE Parameter key isn't available
 *
 * @see mv_engine_config_set_bool_attribute_c()
 * @see mv_engine_config_get_double_attribute_c()
 * @see mv_engine_config_get_int_attribute_c()
 * @see mv_engine_config_get_string_attribute_c()
 */
int mv_engine_config_get_bool_attribute_c(
        mv_engine_config_h engine_cfg,
        const char *name,
        bool *value);

/**
 * @brief Gets the string attribute from the configuration dictionary.
 *
 * @since_tizen 2.4
 * @remarks Function allocates memory required for output @a value, so
 *          it has to be removed by the user himself.
 * @param [in] engine_cfg        Engine configuration from which @a value
 *                               has to be gotten
 * @param [in] name              String key of the attribute will be used for
 *                               getting the @a value from the
 *                               configuration dictionary
 * @param [out] value            The attribute to be filled with string value
 *                               from dictionary
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_KEY_NOT_AVAILABLE Parameter key isn't available
 *
 * @see mv_engine_config_set_string_attribute_c()
 * @see mv_engine_config_get_double_attribute_c()
 * @see mv_engine_config_get_int_attribute_c()
 * @see mv_engine_config_get_bool_attribute_c()
 */
int mv_engine_config_get_string_attribute_c(
        mv_engine_config_h engine_cfg,
        const char *name,
        char **value);

/**
 * @brief Traverses the list of supported attribute names and types.
 * @details Using this function names of supported attributes can be obtained.
 *          Names of the attributes can be used with @ref mv_engine_config_h
 *          related getters and setters to get/set appropriate attribute values.
 *
 * @since_tizen 2.4
 * @remarks If @a callback is called zero times after
 *          @ref mv_engine_config_foreach_supported_attribute() call, then
 *          engine configuration is not supported and setting of attributes will
 *          cause no effect. In this case for all Media Vision functions which
 *          require @ref mv_engine_config_h handle as in parameter this
 *          parameter can be set NULL.
 * @remarks If @a callback is called at least once, then attribute names and
 *          types obtained in the @ref mv_supported_attribute_cb callback can be
 *          changed after mv_engine_config_h handle creation (with
 *          @ref mv_create_engine_config_c() function) by corresponding setters.
 *          Although, if attributes aren't changed by setters, then default
 *          values will be used.\n
 *          Changing of attribute values will affect internal functionality
 *          provided by concrete library underlying Media Vision API.
 * @param [in] callback     The iteration callback function
 * @param [in] user_data    The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 *                                               (@a callback can't be NULL)
 * @retval #MEDIA_VISION_ERROR_NO_DATA Can't determine list of supported attributes
 *
 * @see mv_engine_config_set_double_attribute_c()
 * @see mv_engine_config_set_int_attribute_c()
 * @see mv_engine_config_set_bool_attribute_c()
 * @see mv_engine_config_set_string_attribute_c()
 * @see mv_engine_config_get_double_attribute_c()
 * @see mv_engine_config_get_int_attribute_c()
 * @see mv_engine_config_get_bool_attribute_c()
 * @see mv_engine_config_get_string_attribute_c()
 */
int mv_engine_config_foreach_supported_attribute_c(
        mv_supported_attribute_cb callback,
        void *user_data);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TIZEN_MEDIAVISION_COMMON_C_H__ */
