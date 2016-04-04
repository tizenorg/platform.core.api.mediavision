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

#ifndef __TIZEN_MEDIAVISION_IMAGE_H__
#define __TIZEN_MEDIAVISION_IMAGE_H__

#include <mv_common.h>
#include <mv_image_type.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @file  mv_image.h
 * @brief This file contains the Media Vision Image API.
 *        Working with images (like planar objects): recognition and tracking.
 */

/**
 * @addtogroup CAPI_MEDIA_VISION_IMAGE_MODULE
 * @{
 */

/**
 * @brief Defines MV_IMAGE_RECOGNITION_OBJECT_SCALE_FACTOR to set the image to
 *        be recognized scale factor attribute of the engine configuration.
 * @details The value of the factor will be used for resizing of
 *          the images (objects) for recognition. Scale factor is the double value
 *          and the defalut is 1.2
 *
 * @since_tizen 3.0
 * @see mv_engine_config_set_double_attribute()
 * @see mv_engine_config_get_double_attribute()
 */
#define MV_IMAGE_RECOGNITION_OBJECT_SCALE_FACTOR "MV_IMAGE_RECOGNITION_OBJECT_SCALE_FACTOR"

/**
 * @brief Defines MV_IMAGE_RECOGNITION_OBJECT_MAX_KEYPOINTS_NUM to set the maximum
 *        keypoints should be detected on the image attribute of the
 *        engine configuration.
 * @details The maximal number of keypoints can be selected on the image
 *          object to calculate descriptors. This keypoints will be used
 *          for image (object) recognition and has to be specified as integer number
 *          and the defalut is 1000
 * @since_tizen 3.0
 * @see mv_engine_config_set_int_attribute()
 * @see mv_engine_config_get_int_attribute()
 */
#define MV_IMAGE_RECOGNITION_OBJECT_MAX_KEYPOINTS_NUM "MV_IMAGE_RECOGNITION_OBJECT_MAX_KEYPOINTS_NUM"

/**
 * @brief Defines MV_IMAGE_RECOGNITION_SCENE_SCALE_FACTOR to set the scene scale
 *        factor attribute of the engine configuration.
 * @details The value of the factor will be used for resizing of
 *          the scene including the images (objects) for recognition.
 *          Scale factor is the double value and the defalut is 1.2
 *
 * @since_tizen 3.0
 * @see mv_engine_config_set_double_attribute()
 * @see mv_engine_config_get_double_attribute()
 */
#define MV_IMAGE_RECOGNITION_SCENE_SCALE_FACTOR "MV_IMAGE_RECOGNITION_SCENE_SCALE_FACTOR"

/**
 * @brief Defines MV_IMAGE_RECOGNITION_SCENE_MAX_KEYPOINTS_NUM to set the maximum
 *        keypoints should be detected on the scene attribute of the engine
 *        configuration.
 * @details The maximal number of keypoints can be selected on the scene including
 *          the images (objects) to calculate descriptors.
 *          This keypoints will be used for image recognition and has to be specified
 *          as unsigned integer and the defalut is 5000
 *
 * @since_tizen 3.0
 * @see mv_engine_config_set_double_attribute()
 * @see mv_engine_config_get_double_attribute()
 */
#define MV_IMAGE_RECOGNITION_SCENE_MAX_KEYPOINTS_NUM "MV_IMAGE_RECOGNITION_SCENE_MAX_KEYPOINTS_NUM"

/**
 * @brief Defines MV_IMAGE_RECOGNITION_MIN_MATCH_NUM to set the minimum number
 *        of keypoints matches required for recognition attribute of the engine
 *        configuration.
 * @details The minimal number of keypoints should be matched between an image and a scene.
 *          It will be taken into account for image objects recognition.
 *          Value is unsigned integer and the defalut is 30
 *
 * @since_tizen 3.0
 * @see mv_engine_config_set_int_attribute()
 * @see mv_engine_config_get_int_attribute()
 */
#define MV_IMAGE_RECOGNITION_MIN_MATCH_NUM "MV_IMAGE_RECOGNITION_MIN_MATCH_NUM"

/**
 * @brief Defines MV_IMAGE_RECOGNITION_REQ_MATCH_PART to set the required
 *        matching part for the image recognition attribute of the engine
 *        configuration.
 * @details To recognize occluded or hidden an image by other images,
 *          required relative part of the matches in respect to the total
 *          amount of matching keypoints required for image recognition.
 *          Too low value will result in unsustainable behavior, but
 *          effect of object overlapping will be reduced. Value can be from 0 to 1
 *          and the defalut is 0.05
 *
 * @since_tizen 3.0
 * @see mv_engine_config_set_double_attribute()
 * @see mv_engine_config_get_double_attribute()
 */
#define MV_IMAGE_RECOGNITION_REQ_MATCH_PART "MV_IMAGE_RECOGNITION_REQ_MATCH_PART"

/**
 * @brief Defines MV_IMAGE_RECOGNITION_TOLERANT_MATCH_PART_ERR to set the part
 *        matching error for the image recognition attribute of the engine
 *        configuration.
 * @details Allowable error of matches number. Value can be from 0 to 1
 *          and the defalut is 0.1
 *
 * @since_tizen 3.0
 * @see mv_engine_config_set_double_attribute()
 * @see mv_engine_config_get_double_attribute()
 */
#define MV_IMAGE_RECOGNITION_TOLERANT_MATCH_PART_ERR "MV_IMAGE_RECOGNITION_TOLERANT_MATCH_PART_ERR"

/**
 * @brief Defines MV_IMAGE_TRACKING_HISTORY_AMOUNT to set the number of
 *        recognition results in the tracking history attribute of the engine
 *        configuration.
 * @details Number of previous recognition results, which will influence
 *         the stabilization. Value is unsigned integer and the defalut is 3
 *
 * @since_tizen 3.0
 * @see mv_engine_config_set_int_attribute()
 * @see mv_engine_config_get_int_attribute()
 */
#define MV_IMAGE_TRACKING_HISTORY_AMOUNT "MV_IMAGE_TRACKING_HISTORY_AMOUNT"

/**
 * @brief Defines MV_IMAGE_TRACKING_EXPECTED_OFFSET to set the expected tracking
 *        offset attribute of the engine configuration.
 * @details Relative offset value, for which the object offset is
 *          expected (relative to the object size in the current frame).
 *          Value is a double and the defalut is 0
 *
 * @since_tizen 3.0
 * @see mv_engine_config_set_double_attribute()
 * @see mv_engine_config_get_double_attribute()
 */
#define MV_IMAGE_TRACKING_EXPECTED_OFFSET "MV_IMAGE_TRACKING_EXPECTED_OFFSET"

/**
 * @brief Defines MV_IMAGE_TRACKING_USE_STABLIZATION to enable the contour
 *        stabilization during tracking process. Default value is true.
 *
 * @since_tizen 3.0
 * @see mv_engine_config_set_bool_attribute()
 * @see mv_engine_config_get_bool_attribute()
 */
#define MV_IMAGE_TRACKING_USE_STABLIZATION "MV_IMAGE_TRACKING_USE_STABLIZATION"

/**
 * @brief Defines MV_IMAGE_TRACKING_STABLIZATION_TOLERANT_SHIFT to set the
 *        relative tolerant shift for the tracking stabilization attribute of
 *        the engine configuration.
 * @details It is component of tolerant shift which will be ignored by
 *          stabilization process. (this value is relative to the object size in
 *          the current frame). Tolerant shift will be computed like R * S + C,
 *          where R - value set to MV_IMAGE_TRACKING_STABLIZATION_TOLERANT_SHIFT,
 *          S - area of object location on frame, C - constant value equal 1.3.
 *          Value is a double
 *
 * @since_tizen 3.0
 * @see mv_engine_config_set_double_attribute()
 * @see mv_engine_config_get_double_attribute()
 */
#define MV_IMAGE_TRACKING_STABLIZATION_TOLERANT_SHIFT "MV_IMAGE_TRACKING_STABLIZATION_TOLERANT_SHIFT"

/**
 * @brief Defines MV_IMAGE_TRACKING_STABLIZATION_SPEED to set the
 *        speed of the tracking stabilization attribute of the engine
 *        configuration.
 * @details Start speed will be used for image stabilization. Value is a double
 *          and the defalut is 0.3
 * @since_tizen 3.0
 * @see mv_engine_config_set_double_attribute()
 * @see mv_engine_config_get_double_attribute()
 */
#define MV_IMAGE_TRACKING_STABLIZATION_SPEED "MV_IMAGE_TRACKING_STABLIZATION_SPEED"

/**
 * @brief Defines MV_IMAGE_TRACKING_STABLIZATION_ACCELERATION to set the
 *        acceleration of the tracking stabilization attribute of the engine
 *        configuration.
 * @details Acceleration will be used for image stabilization (relative to
 *          the distance from current location to stabilized location).
 *          Value is double from 0 to 1 and the defalut is 0.1
 *
 * @since_tizen 3.0
 * @see mv_engine_config_set_double_attribute()
 * @see mv_engine_config_get_double_attribute()
 */
#define MV_IMAGE_TRACKING_STABLIZATION_ACCELERATION "MV_IMAGE_TRACKING_STABLIZATION_ACCELERATION"

/****************************/
/* Image object recognition */
/****************************/

/**
 * @brief Called when image recognition results are received from @ref mv_image_recognize()
 * @details This type of callback will be called after @ref mv_image_recognize()
 *          in order to process recognition result.
 *
 * @since_tizen 3.0
 * @remarks Values @a source, @a engine_cfg, @a image_objects, and @a number_of_objects
 *          are the same as values of input parameters of @ref mv_image_recognize().
 * @remarks @a locations are valid only inside callback.
 * @param [in] source              The handle to the source image on which the
 *                                 recognition was carried out
 * @param [in] engine_cfg          The handle to the configuration of engine
 *                                 that was used for image recognition, or NULL
 *                                 if default settings were applied
 * @param [in] image_objects       The set of handles to the image objects which
 *                                 have been processed as targets of recognition
 * @param [in] locations           The locations of image objects on the source
 *                                 image. This array corresponding to an array
 *                                 of image objects and each element contains a
 *                                 location of corresponding object on the
 *                                 @a source image or NULL if object is not
 *                                 recognized
 * @param [in] number_of_objects   The number of image objects and corresponding
 *                                 locations
 * @param [in] user_data           The user data passed from the
 *                                 @ref mv_image_recognize() function
 *
 * @pre Call @ref mv_image_recognize() function to perform recognition of the
 *      image objects on the source image and invoke this callback as a result
 *
 * @see mv_image_recognize()
 * @see mv_source_h
 * @see mv_image_object_h
 * @see mv_engine_config_h
 * @see mv_quadrangle_s
 */
typedef void (*mv_image_recognized_cb)(
        mv_source_h source,
        mv_engine_config_h engine_cfg,
        const mv_image_object_h *image_objects,
        mv_quadrangle_s **locations,
        unsigned int number_of_objects,
        void *user_data);

/**
 * @brief Recognizes the given image objects on the source image.
 * @details Use this function to launch image recognition algorithm configured
 *          by @a engine_conf configuration.
 *
 * @since_tizen 3.0
 * @param [in] source              The handle to the source image on which image
 *                                 objects will be recognized
 * @param [in] image_objects       The set of handles to the image objects which
 *                                 will be processed as targets of recognition
 * @param [in] number_of_objects   The number of image objects
 * @param [in] engine_cfg          The handle to the configuration of engine
 *                                 which will be used for recognition. If NULL,
 *                                 then default settings will be used.
 * @param [in] recognized_cb       The callback which will be called in order to
 *                                 process recognition result
 * @param [in] user_data           The user data to be passed to the
 *                                 @a recognized_cb
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED_FORMAT Source colorspace
 *                                                  isn't supported
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Create a set of image objects using @ref mv_image_object_create() for
 *      each of them and construct (fill / load / clone) them on images that
 *      will be recognized
 * @pre Create a source handle by calling @ref mv_create_source() and fill
 *      by the image for which recognition will be performed
 * @post @a mv_image_recognized_cb will be called to process recognition result
 * @post Release source image by using @ref mv_destroy_source()
 * @post Release image objects by using @ref mv_image_object_destroy() for each
 *       handle from @a image_objects set
 *
 * @see mv_image_recognized_cb
 * @see mv_source_h
 * @see mv_create_source()
 * @see mv_destroy_source()
 * @see mv_image_object_h
 * @see mv_image_object_create()
 * @see mv_image_object_destroy()
 * @see mv_engine_config_h
 */
EXPORT_API int mv_image_recognize(
        mv_source_h source,
        const mv_image_object_h *image_objects,
        int number_of_objects,
        mv_engine_config_h engine_cfg,
        mv_image_recognized_cb recognized_cb,
        void *user_data);

/*************************/
/* Image object tracking */
/*************************/

/**
 * @brief Called when image tracking result received from @ref mv_image_track()
 * @details Image tracking on a sequence of frames assumes calling
 *          @ref mv_image_track() function for each frame in the correct order.
 *          This type of callback will be called after each @ref mv_image_track()
 *          call for processing result data.
 *
 * @since_tizen 3.0
 * @remarks If image object is not tracked then the callback will be invoked,
 *          but @a location will be NULL.
 * @remarks Handles @a image_tracking_model, @a source and @a engine_cfg the
 *          same as input parameters of @ref mv_image_track().
 * @remarks @a location pointer is valid only inside callback
 * @param [in] source                 The handle to the source image on which
 *                                    the tracking was carried out
 * @param [in] image_tracking_model   The handle to the image tracking model
 *                                    which processed as target of tracking
 * @param [in] engine_cfg             The handle to the configuration of engine
 *                                    that was used to image tracking, or
 *                                    NULL if default settings were applied
 * @param [in] location               The image object location on the source
 *                                    image or NULL if objects is not tracked
 * @param [in] user_data              The user data passed from the
 *                                    @ref mv_image_track() function
 *
 * @pre Call @ref mv_image_track() function to perform tracking of the image
 *      object on the current image from the sequence and invoke this callback
 *      as a result
 *
 * @see mv_image_track()
 * @see mv_source_h
 * @see image_tracking_model_h
 * @see mv_engine_config_h
 * @see mv_quadrangle_s
 */
typedef void (*mv_image_tracked_cb)(
        mv_source_h source,
        mv_image_tracking_model_h image_tracking_model,
        mv_engine_config_h engine_cfg,
        mv_quadrangle_s *location,
        void *user_data);

/**
 * @brief Tracks the given image tracking model on the current frame
 * @details Image tracking on a sequence of frames assumes calling this
 *          function for each frame in the correct order.
 *          @a tracked_cb will be called  for result processing.
 *
 * @since_tizen 3.0
 * @remarks Tracking algorithm is usually using for recognition of image object
 *          on the sequence of images that are organized by time. For example,
 *          it may be the sequence of frames from a video stream.
 * @remarks If object is lost during the tracking, system tries to find it
 *          further for the following frames. Therefore, tracking will be
 *          recovered when object appears again.
 * @remarks Previous calls of @ref mv_image_track() for this
 *          @a image_tracking_model will affect on current call
 * @param [in]     source                 The handle to the current image of
 *                                        sequence where image tracking model
 *                                        will be tracked
 * @param [in,out] image_tracking_model   The handle to the image tracking model
 *                                        which processed as target of tracking
 * @param [in]     engine_cfg             The handle to the configuration of
 *                                        engine which will be used for tracking.
 *                                        If NULL, then default settings will be
 *                                        used.
 * @param [in]     tracked_cb             The callback which will receive
 *                                        tracking results
 * @param [in]     user_data              The user data to be passed to the
 *                                        @a tracked_cb
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED_FORMAT Source colorspace
 *                                                  isn't supported
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Create image tracking model by calling
 *      @ref mv_image_tracking_model_create() and set target by calling
 *      @ref mv_image_tracking_model_set_target()
 * @pre Create a source images by calling @ref mv_create_source() for each of
 *      them and construct them based on sequence of images for which will be
 *      held image tracking
 * @post @a tracked_cb will be called to process tracking result
 * @post Release image tracking model by using
 *       @ref mv_image_tracking_model_destroy()
 *
 * @see mv_image_tracked_cb
 * @see mv_source_h
 * @see image_tracking_model_h
 * @see mv_image_tracking_model_create()
 * @see mv_image_tracking_model_set_target()
 * @see mv_image_tracking_model_destroy()
 */
EXPORT_API int mv_image_track(
        mv_source_h source,
        mv_image_tracking_model_h image_tracking_model,
        mv_engine_config_h engine_cfg,
        mv_image_tracked_cb tracked_cb,
        void *user_data);

/**************************/
/* Image object behaviour */
/**************************/

/**
 * @brief Creates an image object.
 *
 * @since_tizen 3.0
 * @param [out] image_object    A new handle to the image object
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 * @retval #MEDIA_VISION_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @post Release image object by using @ref mv_image_object_destroy()
 *
 * @see mv_image_object_destroy()
 * @see mv_image_object_h
 */
EXPORT_API int mv_image_object_create(
        mv_image_object_h *image_object);

/**
 * @brief Destroys the image object.
 *
 * @since_tizen 3.0
 * @param [in] image_object    The handle to the image object to be destroyed
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Create image object by using @ref mv_image_object_create()
 *
 * @see mv_image_object_create()
 * @see mv_image_object_h
 */
EXPORT_API int mv_image_object_destroy(
        mv_image_object_h image_object);

/**
 * @brief Fills the image object.
 * @details Extracts data from @a source image which will be needed for
 *          recognition of depicted object in @a location.
 *
 * @since_tizen 3.0
 * @remarks After filling the image object it can be evaluated by
 *          @ref mv_image_object_get_recognition_rate(). If recognition rate
 *          is too low, try to use another image of object or change
 *          configuration parameters (see @ref mv_engine_config_h) and construct
 *          the image object again.
 * @param [in,out] image_object    The handle to the image object which will be
 *                                 filled and can be recognized in future
 * @param [in]     engine_cfg      The handle to the configuration of engine
 *                                 which will be used for extract recognition
 *                                 data from @a source. If NULL, then default
 *                                 settings will be used.
 * @param [in]     source          The source image where image object is depicted
 * @param [in]     location        The pointer to location of the image object
 *                                 on the source image, or NULL if the object is
 *                                 shown in full
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED_FORMAT Source colorspace
 *                                                  isn't supported
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Create image object by using @ref mv_image_object_create()
 * @post Release image object by using @ref mv_image_object_destroy()
 *
 * @see mv_image_object_h
 * @see mv_image_object_create()
 * @see mv_image_object_get_recognition_rate()
 * @see mv_image_recognize()
 * @see mv_image_object_destroy()
 * @see mv_engine_config_h
 */
EXPORT_API int mv_image_object_fill(
        mv_image_object_h image_object,
        mv_engine_config_h engine_cfg,
        mv_source_h source,
        mv_rectangle_s *location);

/**
 * @brief Gets a value that determines how well an image object can be recognized.
 * @details Recognition rate determines how well an image object can be
 *          recognized. This value can be from 0 to 1. If the recognition rate
 *          is 0 object can not be recognized and the bigger it is the more
 *          likely to recognize the object.
 *
 * @since_tizen 3.0
 * @remarks If recognition rate is too low, try to use another image of object
 *          or change some configuration parameters (see @ref mv_engine_config_h)
 *          and fill the image object again (see @ref mv_image_object_fill()).
 * @param [in]  image_object        The handle to the image object which will be
 *                                  evaluated by this function
 * @param [out] recognition_rate    A value that determines how well an image
 *                                  object can be recognized, if 0 then object
 *                                  can not be recognized
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Create image object by using @ref mv_image_object_create()
 * @post Release image object by using @ref mv_image_object_destroy()
 *
 * @see mv_image_object_h
 * @see mv_image_object_create()
 * @see mv_image_object_fill()
 * @see mv_image_object_destroy()
 * @see mv_engine_config_h
 */
EXPORT_API int mv_image_object_get_recognition_rate(
        mv_image_object_h image_object,
        double *recognition_rate);

/**
 * @brief Sets a label for the image object.
 *
 * @since_tizen 3.0
 * @param [in] image_object   The handle to the image object for which the label
 *                            will be assigned
 * @param [in] label          The label which will be assigned to the image
 *                            object
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Create image object by using @ref mv_image_object_create()
 * @post Label could be received by using @ref mv_image_object_get_label()
 * @post Release image object by using @ref mv_image_object_destroy()
 *
 * @see mv_image_object_get_label()
 * @see mv_image_object_h
 * @see mv_image_object_create()
 * @see mv_image_object_destroy()
 */
EXPORT_API int mv_image_object_set_label(
        mv_image_object_h image_object,
        int label);

/**
 * @brief Gets a label of image object.
 *
 * @since_tizen 3.0
 * @remarks If @a image_object have not a label, this function return
 *          MEDIA_VISION_ERROR_NO_DATA value.
 * @param [in]  image_object   The handle to the image object from which a
 *                             label will be received
 * @param [out] label          The label of image object
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NO_DATA Image object hasn't label
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Create image object by using @ref mv_image_object_create()
 * @pre Set label for the image object by using @ref mv_image_object_set_label()
 * @post Release image object by using @ref mv_image_object_destroy()
 *
 * @see mv_image_object_set_label()
 * @see mv_image_object_h
 * @see mv_image_object_create()
 * @see mv_image_object_destroy()
 */
EXPORT_API int mv_image_object_get_label(
        mv_image_object_h image_object,
        int *label);

/**
 * @brief Clones the image object.
 *
 * @since_tizen 3.0
 * @remarks @a dst must be released using mv_image_object_destroy().
 * @param [in]  src    The handle to the source image object
 * @param [out] dst    The handle to the destination image object
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 * @retval #MEDIA_VISION_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @pre Create image object handles by calling mv_image_object_create()
 *
 * @see mv_image_object_create()
 * @see mv_image_object_destroy()
 */
EXPORT_API int mv_image_object_clone(
        mv_image_object_h src,
        mv_image_object_h *dst);

/**
 * @brief Saves the image object.
 *
 * @since_tizen 3.0
 * @remarks @a image_object is saved to the absolute path directory.
 *          Use @ref app_get_data_path for the private app storage path.
 * @param [in] file_name       Name of the file to path/save the image object
 * @param [in] image_object    The handle to the image object which will be saved
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_INVALID_PATH Invalid path
 * @retval #MEDIA_VISION_ERROR_PERMISSION_DENIED Not permitted
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Create image object handle by calling mv_image_object_create()
 * @post Saved model can be loaded later by calling
 *       mv_image_object_load() function
 *
 * @see mv_image_object_create()
 * @see mv_image_object_load()
 * @see mv_image_object_destroy()
 */
EXPORT_API int mv_image_object_save(
        const char *file_name, mv_image_object_h image_object);

/**
 * @brief Loads an image object from the file.
 *
 * @since_tizen 3.0
 * @remarks @a image_object is loaded from the absolute path directory.
 *          Use @ref app_get_data_path for the private app storage path.
 *          @a image_object must be destroyed using
 *          @ref mv_image_object_destroy().
 * @param [in]  file_name       Name of path/file to load the image object
 * @param [out] image_object    The handle to the image object which will be
 *                              filled
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_INVALID_PATH Invalid path
 * @retval #MEDIA_VISION_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #MEDIA_VISION_ERROR_PERMISSION_DENIED Not permitted
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Image object can be preliminary saved with mv_image_object_save()
 *      function
 *
 * @see mv_image_object_save()
 * @see mv_image_object_destroy()
 */
EXPORT_API int mv_image_object_load(
        const char *file_name, mv_image_object_h *image_object);

/**********************************/
/* Image tracking model behaviour */
/**********************************/

/**
 * @brief Creates an image tracking model.
 *
 * @since_tizen 3.0
 * @param [out] image_tracking_model    A new handle to the image tracking model
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @post Release image tracking model by using mv_image_tracking_model_destroy()
 *
 * @see mv_image_tracking_model_destroy()
 */
EXPORT_API int mv_image_tracking_model_create(
        mv_image_tracking_model_h *image_tracking_model);

/**
 * @brief Sets target of image tracking model.
 * @details Sets image object which will be tracked by using tracking
 *          functionality with @a image_tracking_model.
 *
 * @since_tizen 3.0
 * @param [in] image_object            Image object which will be set
 *                                     as target for tracking
 * @param [in] image_tracking_model    Handle to the image tracking model
 *                                     for which will be set a new target
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Create image tracking model by calling
 *      @ref mv_image_tracking_model_create()
 * @pre Create an image object using @ref mv_image_object_create() and construct
 *      (fill / load / clone) it on image that will be tracking
 * @post Release image object by using @ref mv_image_object_destroy()
 * @post Release image tracking model by using
 *       @ref mv_image_tracking_model_destroy()
 *
 * @see mv_image_object_h
 * @see mv_image_tracking_model_h
 * @see mv_image_object_create()
 * @see mv_image_object_destroy()
 * @see mv_image_tracking_model_create()
 * @see mv_image_track()
 * @see mv_image_tracking_model_destroy()
 */
EXPORT_API int mv_image_tracking_model_set_target(
        mv_image_object_h image_object,
        mv_image_tracking_model_h image_tracking_model);

/**
 * @brief Destroys the image tracking model.
 *
 * @since_tizen 3.0
 * @param [in] image_tracking_model    The handle to the image tracking model
 *                                     to be destroyed
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Create image tracking model by using mv_image_tracking_model_create()
 *
 * @see mv_image_tracking_model_create()
 */
EXPORT_API int mv_image_tracking_model_destroy(
        mv_image_tracking_model_h image_tracking_model);

/**
 * @brief Refreshes the state of image tracking model.
 * @details Clears moving history and change state to undetected. This function
 *          is usually called each time before tracking is started for the new
 *          sequence of sources which is not the direct continuation of the
 *          sequence for which tracking has been performed before. Tracking
 *          algorithm will try to find image by itself.
 *
 * @since_tizen 3.0
 * @param [in] image_tracking_model   The handle to the image tracking model
 *                                    which will be refreshed
 * @param [in] engine_cfg             The handle to the configuration of
 *                                    engine which will be used. If NULL,
 *                                    then default settings will be used.
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Create image tracking model by calling
 *      @ref mv_image_tracking_model_create()
 * @post Release image tracking model by using
 *       @ref mv_image_tracking_model_destroy()
 *
 * @see mv_image_tracking_model_h
 * @see mv_image_tracking_model_create()
 * @see mv_image_track()
 * @see mv_image_tracking_model_destroy()
 */
EXPORT_API int mv_image_tracking_model_refresh(
        mv_image_tracking_model_h image_tracking_model,
        mv_engine_config_h engine_cfg);

/**
 * @brief Clones the image tracking model.
 *
 * @since_tizen 3.0
 * @remarks @a dst must be released using mv_image_tracking_model_destroy().
 * @param [in]  src    The handle to the source image tracking model
 * @param [out] dst    The handle to the destination image tracking model
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 * @retval #MEDIA_VISION_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @pre Create image tracking model handles by calling
 *      mv_image_tracking_model_create()
 *
 * @see mv_image_tracking_model_create()
 * @see mv_image_tracking_model_destroy()
 */
EXPORT_API int mv_image_tracking_model_clone(
        mv_image_tracking_model_h src,
        mv_image_tracking_model_h *dst);

/**
 * @brief Saves the image tracking model.
 *
 * @since_tizen 3.0
 * @remarks @a image_tracking_model is saved to the absolute path directory.
 *          Use @ref app_get_data_path for the private app storage path.
 * @param [in] file_name               Name of path/file to save the model
 * @param [in] image_tracking_model    The handle to the image tracking model
 *                                     to be saved
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_INVALID_PATH Invalid path
 * @retval #MEDIA_VISION_ERROR_PERMISSION_DENIED Not permitted
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Create image tracking model handle by calling
 *      mv_image_tracking_model_create()
 * @post Saved model can be loaded later by calling
 *       mv_image_tracking_model_load() function
 *
 * @see mv_image_tracking_model_create()
 * @see mv_image_tracking_model_load()
 * @see mv_image_tracking_model_destroy()
 */
EXPORT_API int mv_image_tracking_model_save(
        const char *file_name, mv_image_tracking_model_h image_tracking_model);

/**
 * @brief Loads an image tracking model from the file.
 *
 * @since_tizen 3.0
 * @remarks @a image_tracking_model is loaded from the absolute path directory.
 *          Use @ref app_get_data_path for the private app storage path.
 *          @a image_tracking_model must be destroyed using
 *          @ref mv_image_tracking_model_destroy.
 * @param [in]  file_name               Name of path/file to load model
 * @param [out] image_tracking_model    The handle to the image tracking
 *                                      model to be filled
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_INVALID_PATH Invalid path
 * @retval #MEDIA_VISION_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #MEDIA_VISION_ERROR_PERMISSION_DENIED Not permitted
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Image tracking model handle can be preliminary saved with
 *      mv_image_tracking_model_save() function
 *
 * @see mv_image_tracking_model_save()
 * @see mv_image_tracking_model_destroy()
 */
EXPORT_API int mv_image_tracking_model_load(
        const char *file_name, mv_image_tracking_model_h *image_tracking_model);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TIZEN_MEDIAVISION_IMAGE_H__ */
