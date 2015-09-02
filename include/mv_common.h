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

#ifndef __TIZEN_MEDIAVISION_COMMON_H__
#define __TIZEN_MEDIAVISION_COMMON_H__

#include <media_packet.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @file  mv_common.h
 * @brief This file contains the Media Vision Common API.
 */

/**
 * @addtogroup CAPI_MEDIA_VISION_COMMON_MODULE
 * @{
 */

/**
 * @brief Point in 2D space.
 *
 * @since_tizen 2.4
 */
typedef struct
{
    int x; /**< X-axis coordinate of the point in 2D space */
    int y; /**< Y-axis coordinate of the point in 2D space */
} mv_point_s;

/**
 * @brief Location of the object bounded by quadrangle defined by four 2D points.
 *
 * @since_tizen 2.4
 */
typedef struct
{
    mv_point_s points[4];    /**< Four points that define object bounding
                                  quadrangle */
} mv_quadrangle_s;

/**
 * @brief Location of the object bounded by rectangle defined by
 *        coordinates of top left corner, width and height.
 *
 * @since_tizen 2.4
 */
typedef struct
{
    mv_point_s point;     /**< Top left corner of rectangle coordinates */
    int width;            /**< Width of the bounding rectangle */
    int height;           /**< Height of the bounding rectangle */
} mv_rectangle_s;

/**
 * @brief Enumeration for Media Vision error.
 *
 * @since_tizen 2.4
 */
typedef enum
{
    MEDIA_VISION_ERROR_NONE
            = TIZEN_ERROR_NONE,                /**< Successful */
    MEDIA_VISION_ERROR_NOT_SUPPORTED
            = TIZEN_ERROR_NOT_SUPPORTED,       /**< Not supported */
    MEDIA_VISION_ERROR_MSG_TOO_LONG
            = TIZEN_ERROR_MSG_TOO_LONG,        /**< Message too long */
    MEDIA_VISION_ERROR_NO_DATA
            = TIZEN_ERROR_NO_DATA,             /**< No data */
    MEDIA_VISION_ERROR_KEY_NOT_AVAILABLE
            = TIZEN_ERROR_KEY_NOT_AVAILABLE,   /**< Key not available */
    MEDIA_VISION_ERROR_OUT_OF_MEMORY
            = TIZEN_ERROR_OUT_OF_MEMORY,       /**< Out of memory */
    MEDIA_VISION_ERROR_INVALID_PARAMETER
            = TIZEN_ERROR_INVALID_PARAMETER,   /**< Invalid parameter */
    MEDIA_VISION_ERROR_INVALID_OPERATION
            = TIZEN_ERROR_INVALID_OPERATION,   /**< Invalid operation */
    MEDIA_VISION_ERROR_PERMISSION_DENIED
            = TIZEN_ERROR_NOT_PERMITTED,       /**< Not permitted */
    MEDIA_VISION_ERROR_NOT_SUPPORTED_FORMAT
            = TIZEN_ERROR_MEDIA_VISION | 0x01, /**< Not supported format */
    MEDIA_VISION_ERROR_INTERNAL
            = TIZEN_ERROR_MEDIA_VISION | 0x02, /**< Internal error */
    MEDIA_VISION_ERROR_INVALID_DATA
            = TIZEN_ERROR_MEDIA_VISION | 0x03, /**< Invalid data */
    MEDIA_VISION_ERROR_INVALID_PATH
            = TIZEN_ERROR_MEDIA_VISION | 0x04, /**< Invalid path (Since 3.0) */
} mv_error_e;

/**
 * @brief Enumeration for Media Vision @ref mv_engine_config_h handle attribute
 *        type.
 *
 * @since_tizen 2.4
 */
typedef enum
{
    MV_ENGINE_CONFIG_ATTR_TYPE_DOUBLE,     /**< Double attribute type */
    MV_ENGINE_CONFIG_ATTR_TYPE_INTEGER,    /**< Integer attribute type */
    MV_ENGINE_CONFIG_ATTR_TYPE_BOOLEAN,    /**< Boolean attribute type */
    MV_ENGINE_CONFIG_ATTR_TYPE_STRING      /**< String attribute type */
} mv_config_attribute_type_e;

/**
 * @brief Enumeration for Media Vision colorspace.
 *
 * @since_tizen 2.4
 */
typedef enum
{
    MEDIA_VISION_COLORSPACE_INVALID, /**< The colorspace type is invalid */
    MEDIA_VISION_COLORSPACE_Y800,    /**< The colorspace type is Y800 */
    MEDIA_VISION_COLORSPACE_I420,    /**< The colorspace type is I420 */
    MEDIA_VISION_COLORSPACE_NV12,    /**< The colorspace type is NV12 */
    MEDIA_VISION_COLORSPACE_YV12,    /**< The colorspace type is YV12 */
    MEDIA_VISION_COLORSPACE_NV21,    /**< The colorspace type is NV21 */
    MEDIA_VISION_COLORSPACE_YUYV,    /**< The colorspace type is YUYV */
    MEDIA_VISION_COLORSPACE_UYVY,    /**< The colorspace type is UYVY */
    MEDIA_VISION_COLORSPACE_422P,    /**< The colorspace type is 422P */
    MEDIA_VISION_COLORSPACE_RGB565,  /**< The colorspace type is RGB565 */
    MEDIA_VISION_COLORSPACE_RGB888,  /**< The colorspace type is RGB888 */
    MEDIA_VISION_COLORSPACE_RGBA,    /**< The colorspace type is RGBA */
} mv_colorspace_e;

/**
 * @brief The handle to the Media Vision API engine algorithms configuration.
 * @details Configuration is a dictionary consists of key and value pairs to
 *          collect engine-specific settings and allow Media Vision module to
 *          access them internally. Engine configuration provides developer by
 *          the possibility to make computer vision algorithms work better
 *          in particular conditions of API usage. To create engine
 *          configuration handle @ref mv_create_engine_config() function has to
 *          be used. When configuration is not needed any more, it is required to
 *          destroy it and release resources by @ref mv_destroy_engine_config()
 *          function.
 *
 * @since_tizen 2.4
 *
 * @see mv_create_engine_config()
 * @see mv_destroy_engine_config()
 */
typedef void *mv_engine_config_h;

/**
 * @brief The handle to the source.
 *
 * @since_tizen 2.4
 */
typedef void *mv_source_h;

/**
 * @brief Creates a source handle.
 *
 * @since_tizen 2.4
 * @remarks You must release @a source by using @ref mv_destroy_source().
 * @param [out] source    A new handle to the source
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @see mv_destroy_source()
 */
int mv_create_source(
        mv_source_h *source);

/**
 * @brief Destroys the source handle and releases all its resources.
 *
 * @since_tizen 2.4
 * @param [in] source    The handle to the source to be destroyed
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @see mv_create_source()
 */
int mv_destroy_source(
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
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED_FORMAT Not supported format
 * @retval #MEDIA_VISION_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Create a source handle by calling @ref mv_create_source()
 *
 * @see mv_create_source()
 * @see mv_destroy_source()
 */
int mv_source_fill_by_media_packet(
        mv_source_h source,
        media_packet_h media_packet);

/**
 * @brief Fills the media source based on the buffer and metadata.
 *
 * @since_tizen 2.4
 * @param [in,out] source             The handle to the source
 * @param [in]     data_buffer        The buffer of image data
 * @param [in]     buffer_size        The buffer size
 * @param [in]     image_width        The width of image data
 * @param [in]     image_height       The height of image data
 * @param [in]     image_colorspace   The image colorspace
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Create a source handle by calling @ref mv_create_source()
 *
 * @see mv_source_clear()
 */
int mv_source_fill_by_buffer(
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
 * @param [in,out] source    The handle to the source
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @see mv_source_fill_by_buffer()
 */
int mv_source_clear(
        mv_source_h source);

/**
 * @brief Gets buffer of the media source.
 *
 * @since_tizen 2.4
 * @remarks Note that the retrieved buffer will be destroyed when
 *          @ref mv_destroy_source() or @ref mv_source_clear() function
 *          is called for the @a source.
 *
 * @param [in]    source        The handle to the source
 * @param [out]   data_buffer   The buffer of the source
 * @param [out]   buffer_size   The size of buffer
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @see mv_source_get_width()
 * @see mv_source_get_height()
 * @see mv_source_get_colorspace()
 */
int mv_source_get_buffer(
        mv_source_h source,
        unsigned char **data_buffer,
        unsigned int *buffer_size);

/**
 * @brief Gets height of the media source.
 *
 * @since_tizen 2.4
 * @param [in]    source         The handle to the source
 * @param [out]   image_height   The height of an image in the source
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @see mv_source_get_width()
 * @see mv_source_get_colorspace()
 * @see mv_source_get_buffer()
 */
int mv_source_get_height(
        mv_source_h source,
        unsigned int *image_height);

/**
 * @brief Gets width of the media source.
 *
 * @since_tizen 2.4
 * @param [in]    source        The handle to the source
 * @param [out]   image_width   The width of an image in the source
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @see mv_source_get_height()
 * @see mv_source_get_colorspace()
 * @see mv_source_get_buffer()
 */
int mv_source_get_width(
        mv_source_h source,
        unsigned int *image_width);

/**
 * @brief Gets colorspace of the media source.
 *
 * @since_tizen 2.4
 * @param [in]    source             The handle to the source
 * @param [out]   image_colorspace   The colorspace of an image in the source
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @see mv_source_get_width()
 * @see mv_source_get_height()
 * @see mv_source_get_buffer()
 */
int mv_source_get_colorspace(
        mv_source_h source,
        mv_colorspace_e *image_colorspace);

/**
 * @brief Creates the handle to the configuration of engine.
 *
 * @since_tizen 2.4
 * @remarks Available engine configuration attributes can be get by using
 *          @ref mv_engine_config_foreach_supported_attribute().
 *          The attributes can be changed by @ref mv_engine_config_h
 *          related setters. Default values are used if the attributes
 *          are not changed.
 * @param [out] engine_cfg    The handle to the engine to be created
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @see mv_engine_config_h
 * @see mv_destroy_engine_config()
 * @see mv_engine_config_set_double_attribute()
 * @see mv_engine_config_set_int_attribute()
 * @see mv_engine_config_set_bool_attribute()
 * @see mv_engine_config_set_string_attribute()
 * @see mv_engine_config_get_double_attribute()
 * @see mv_engine_config_get_int_attribute()
 * @see mv_engine_config_get_bool_attribute()
 * @see mv_engine_config_get_string_attribute()
 */
int mv_create_engine_config(
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
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @see mv_engine_config_h
 * @see mv_create_engine_config()
 */
int mv_destroy_engine_config(
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
 * @retval #MEDIA_VISION_ERROR_KEY_NOT_AVAILABLE Attribute key isn't available
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @see mv_engine_config_get_double_attribute()
 * @see mv_engine_config_set_int_attribute()
 * @see mv_engine_config_set_bool_attribute()
 * @see mv_engine_config_set_string_attribute()
 */
int mv_engine_config_set_double_attribute(
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
 * @retval #MEDIA_VISION_ERROR_KEY_NOT_AVAILABLE Attribute key isn't available
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @see mv_engine_config_get_int_attribute()
 * @see mv_engine_config_set_double_attribute()
 * @see mv_engine_config_set_bool_attribute()
 * @see mv_engine_config_set_string_attribute()
 * @see mv_barcode_detect_attr_mode_e
 * @see mv_barcode_detect_attr_target_e
 */
int mv_engine_config_set_int_attribute(
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
 * @retval #MEDIA_VISION_ERROR_KEY_NOT_AVAILABLE Attribute key isn't available
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @see mv_engine_config_get_bool_attribute()
 * @see mv_engine_config_set_double_attribute()
 * @see mv_engine_config_set_int_attribute()
 * @see mv_engine_config_set_string_attribute()
 */
int mv_engine_config_set_bool_attribute(
        mv_engine_config_h engine_cfg,
        const char *name,
        bool value);

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
 * @retval #MEDIA_VISION_ERROR_KEY_NOT_AVAILABLE Attribute key isn't available
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @see mv_engine_config_get_string_attribute()
 * @see mv_engine_config_set_double_attribute()
 * @see mv_engine_config_set_int_attribute()
 * @see mv_engine_config_set_bool_attribute()
 */
int mv_engine_config_set_string_attribute(
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
 * @retval #MEDIA_VISION_ERROR_KEY_NOT_AVAILABLE Attribute key isn't available
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @see mv_engine_config_set_double_attribute()
 * @see mv_engine_config_get_int_attribute()
 * @see mv_engine_config_get_bool_attribute()
 * @see mv_engine_config_get_string_attribute()
 */
int mv_engine_config_get_double_attribute(
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
 * @retval #MEDIA_VISION_ERROR_KEY_NOT_AVAILABLE Attribute key isn't available
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @see mv_engine_config_set_int_attribute()
 * @see mv_engine_config_get_double_attribute()
 * @see mv_engine_config_get_bool_attribute()
 * @see mv_engine_config_get_string_attribute()
 * @see mv_barcode_detect_attr_mode_e
 * @see mv_barcode_detect_attr_target_e
 */
int mv_engine_config_get_int_attribute(
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
 * @retval #MEDIA_VISION_ERROR_KEY_NOT_AVAILABLE Attribute key isn't available
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @see mv_engine_config_set_bool_attribute()
 * @see mv_engine_config_get_double_attribute()
 * @see mv_engine_config_get_int_attribute()
 * @see mv_engine_config_get_string_attribute()
 */
int mv_engine_config_get_bool_attribute(
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
 * @retval #MEDIA_VISION_ERROR_KEY_NOT_AVAILABLE Attribute key isn't available
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @see mv_engine_config_set_string_attribute()
 * @see mv_engine_config_get_double_attribute()
 * @see mv_engine_config_get_int_attribute()
 * @see mv_engine_config_get_bool_attribute()
 */
int mv_engine_config_get_string_attribute(
        mv_engine_config_h engine_cfg,
        const char *name,
        char **value);

/**
 * @brief Called to get information (type and name) once for each supported
 *        attribute.
 *
 * @since_tizen 2.4
 * @param [in] attribute_type    The supported attribute type
 * @param [in] attribute_name    The supported attribute name
 * @param [in] user_data         The user data passed from the
 *                               @ref mv_engine_config_foreach_supported_attribute()
 *                               function
 * @return @c true to continue with the next iteration of the loop, \n
 *         otherwise @c false to break out of the loop
 *
 * @pre mv_engine_config_foreach_supported_attribute() will invoke this callback
 * @see mv_engine_config_foreach_supported_attribute()
 */
typedef bool (*mv_supported_attribute_cb)(
        mv_config_attribute_type_e attribute_type,
        const char *attribute_name,
        void *user_data);

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
 *          @ref mv_create_engine_config() function) by corresponding setters.
 *          Although, if attributes aren't changed by setters, then default
 *          values will be used.\n
 *          Changing of attribute values will affect internal functionality
 *          provided by concrete library underlying Media Vision API.
 * @param [in] callback     The iteration callback function
 * @param [in] user_data    The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_NO_DATA Can't determine list of supported attributes
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @see mv_engine_config_set_double_attribute()
 * @see mv_engine_config_set_int_attribute()
 * @see mv_engine_config_set_bool_attribute()
 * @see mv_engine_config_set_string_attribute()
 * @see mv_engine_config_get_double_attribute()
 * @see mv_engine_config_get_int_attribute()
 * @see mv_engine_config_get_bool_attribute()
 * @see mv_engine_config_get_string_attribute()
 */
int mv_engine_config_foreach_supported_attribute(
        mv_supported_attribute_cb callback,
        void *user_data);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TIZEN_MEDIAVISION_COMMON_H__ */
