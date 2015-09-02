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

#ifndef __TIZEN_MEDIAVISION_IMAGE_OPEN_H__
#define __TIZEN_MEDIAVISION_IMAGE_OPEN_H__

#include "mv_image.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @file  mv_image_open.h
 * @brief This file contains the Media Vision Image API for the open module.
 *        Working with images (like planar objects): recognition and tracking.
 */

/****************************/
/* Image object recognition */
/****************************/

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
 *
 * @pre Create a set of image objects using @ref mv_image_object_create_open()
 *      for each of them and construct (fill / load / clone) them on images that
 *      will be recognized
 * @pre Create a source handle by calling @ref mv_create_source() and fill
 *      by the image for which recognition will be performed
 * @post @a mv_image_recognized_cb will be called to process recognition result
 * @post Release source image by using @ref mv_destroy_source()
 * @post Release image objects by using @ref mv_image_object_destroy_open() for
 *       each handle from @a image_objects set
 *
 * @see mv_image_recognized_cb
 * @see mv_source_h
 * @see mv_create_source()
 * @see mv_destroy_source()
 * @see mv_image_object_h
 * @see mv_image_object_create_open()
 * @see mv_image_object_destroy_open()
 * @see mv_engine_config_h
 */
int mv_image_recognize_open(
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
 * @remarks Previous calls of @ref mv_image_track_open() for this
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
 *
 * @pre Create image tracking model by calling
 *      @ref mv_image_tracking_model_create_open() and set target by calling
 *      @ref mv_image_tracking_model_set_target_open()
 * @pre Create a source images by calling @ref mv_create_source() for each of
 *      them and construct them based on sequence of images for which will be
 *      held image tracking
 * @post @a tracked_cb will be called to process tracking result
 * @post Release image tracking model by using
 *       @ref mv_image_tracking_model_destroy_open()
 *
 * @see mv_image_tracked_cb
 * @see mv_source_h
 * @see image_tracking_model_h
 * @see mv_image_tracking_model_create_open()
 * @see mv_image_tracking_model_set_target_open()
 * @see mv_image_tracking_model_destroy_open()
 */
int mv_image_track_open(
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
 * @retval #MEDIA_VISION_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @pre Release image object by using mv_image_object_destroy_open()
 *
 * @see mv_image_object_destroy_open()
 */
int mv_image_object_create_open(
        mv_image_object_h *image_object);

/**
 * @brief Destroys the image object.
 *
 * @since_tizen 3.0
 * @param [in] image_object    The handle to the image object to be destroyed
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see mv_image_object_create_open()
 */
int mv_image_object_destroy_open(
        mv_image_object_h image_object);

/**
 * @brief Fills the image object.
 * @details Extracts data from @a source image which will be needed for
 *          recognition of depicted object in @a location.
 *
 * @since_tizen 3.0
 * @remarks After filling the image object it can be evaluated by
 *          @ref mv_image_object_get_recognition_rate_open(). If recognition
 *          rate is too low, try to use another image of object or change
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
 *
 * @pre Create image object by using @ref mv_image_object_create_open()
 * @post Release image object by using @ref mv_image_object_destroy_open()
 *
 * @see mv_image_object_h
 * @see mv_image_object_create_open()
 * @see mv_image_object_get_recognition_rate_open()
 * @see mv_image_recognize_open()
 * @see mv_image_object_destroy_open()
 * @see mv_engine_config_h
 */
int mv_image_object_fill_open(
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
 *          and fill the image object again
 *          (see @ref mv_image_object_fill_open()).
 * @param [in]  image_object        The handle to the image object which will be
 *                                  evaluated by this function
 * @param [out] recognition_rate    A value that determines how well an image
 *                                  object can be recognized, if 0 then object
 *                                  can not be recognized
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @pre Create image object by using @ref mv_image_object_create_open()
 * @post Release image object by using @ref mv_image_object_destroy_open()
 *
 * @see mv_image_object_h
 * @see mv_image_object_create_open()
 * @see mv_image_object_fill_open()
 * @see mv_image_object_destroy_open()
 * @see mv_engine_config_h
 */
int mv_image_object_get_recognition_rate_open(
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
 *
 * @pre Create image object by using @ref mv_image_object_create_open()
 * @post Label could be received by using
 *       @ref mv_image_object_get_label_open()
 * @post Release image object by using @ref mv_image_object_destroy_open()
 *
 * @see mv_image_object_get_label_open()
 * @see mv_image_object_h
 * @see mv_image_object_create_open()
 * @see mv_image_object_destroy_open()
 */
int mv_image_object_set_label_open(
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
 *
 * @pre Create image object by using @ref mv_image_object_create_open()
 * @pre Set label for the image object by using
 *      @ref mv_image_object_set_label_open()
 * @post Release image object by using @ref mv_image_object_destroy_open()
 *
 * @see mv_image_object_set_label_open()
 * @see mv_image_object_h
 * @see mv_image_object_create_open()
 * @see mv_image_object_destroy_open()
 */
int mv_image_object_get_label_open(
        mv_image_object_h image_object,
        int *label);

/**
 * @brief Clones the image object.
 *
 * @since_tizen 3.0
 * @remarks @a dst must be released using mv_image_object_destroy_open().
 * @param [in]  src    The handle to the source image object
 * @param [out] dst    The handle to the destination image object
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @pre Create image object handles by calling mv_image_object_create_open()
 *
 * @see mv_image_object_create_open()
 * @see mv_image_object_destroy_open()
 */
int mv_image_object_clone_open(
        mv_image_object_h src,
        mv_image_object_h *dst);

/**
 * @brief Saves the image object.
 *
 * @since_tizen 3.0
 * @remarks @a image_object is saved to the application's data directory.
 * @param [in] file_name       Name of the file to save the image object
 * @param [in] image_object    The handle to the image object which will be saved
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_INVALID_PATH Invalid path
 * @retval #MEDIA_VISION_ERROR_PERMISSION_DENIED Not permitted
 *
 * @see mv_image_object_create_open()
 * @see mv_image_object_load_open()
 * @see mv_image_object_destroy_open()
 */
int mv_image_object_save_open(
        const char *file_name, mv_image_object_h image_object);

/**
 * @brief Loads an image object from the file.
 *
 * @since_tizen 3.0
 * @remarks @a image_object is loaded from the application's data directory.
 *          @a image_object must be destroyed using
 *          @ref mv_image_object_destroy().
 * @param [in]  file_name       Name of file to load the image object
 * @param [out] image_object    The handle to the image object which will be
 *                              filled
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_INVALID_PATH Invalid path
 * @retval #MEDIA_VISION_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #MEDIA_VISION_ERROR_PERMISSION_DENIED Not permitted
 *
 * @pre Image object can be preliminary saved with mv_image_object_save()
 *      function
 *
 * @see mv_image_object_save_open()
 * @see mv_image_object_destroy_open()
 */
int mv_image_object_load_open(
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
 * @retval #MEDIA_VISION_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @pre Release image tracking model by using mv_image_tracking_model_destroy_open()
 *
 * @see mv_image_tracking_model_destroy_open()
 */
int mv_image_tracking_model_create_open(
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
 *
 * @pre Create image tracking model by calling
 *      @ref mv_image_tracking_model_create_open()
 * @pre Create an image object using @ref mv_image_object_create_open() and
 *      construct (fill / load / clone) it on image that will be tracking
 * @post Release image object by using @ref mv_image_object_destroy_open()
 * @post Release image tracking model by using
 *       @ref mv_image_tracking_model_destroy_open()
 *
 * @see mv_image_object_h
 * @see mv_image_tracking_model_h
 * @see mv_image_object_create_open()
 * @see mv_image_object_destroy_open()
 * @see mv_image_tracking_model_create_open()
 * @see mv_image_track_open()
 * @see mv_image_tracking_model_destroy_open()
 */
int mv_image_tracking_model_set_target_open(
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
 *
 * @pre Create image tracking model by using mv_image_tracking_model_create()
 *
 * @see mv_image_tracking_model_create_open()
 */
int mv_image_tracking_model_destroy_open(
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
 *
 * @pre Create image tracking model by calling
 *      @ref mv_image_tracking_model_create_open()
 * @post Release image tracking model by using
 *       @ref mv_image_tracking_model_destroy_open()
 *
 * @see mv_image_tracking_model_h
 * @see mv_image_tracking_model_create_open()
 * @see mv_image_track_open()
 * @see mv_image_tracking_model_destroy_open()
 */
int mv_image_tracking_model_refresh_open(
        mv_image_tracking_model_h image_tracking_model,
        mv_engine_config_h engine_cfg);

/**
 * @brief Clones the image tracking model.
 *
 * @since_tizen 3.0
 * @remarks @a dst must be released using mv_image_tracking_model_destroy_open().
 * @param [in]  src    The handle to the source image tracking model
 * @param [out] dst    The handle to the destination image tracking model
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @see mv_image_tracking_model_create_open()
 * @see mv_image_tracking_model_destroy_open()
 */
int mv_image_tracking_model_clone_open(
        mv_image_tracking_model_h src,
        mv_image_tracking_model_h *dst);

/**
 * @brief Saves the image tracking model.
 *
 * @since_tizen 3.0
 * @remarks @a image_tracking_model is saved to the application's data directory.
 * @param [in] file_name               Name of file to save the model
 * @param [in] image_tracking_model    The handle to the image tracking model
 *                                     to be saved
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_INVALID_PATH Invalid path
 * @retval #MEDIA_VISION_ERROR_PERMISSION_DENIED Not permitted
 *
 * @pre Create image tracking model handle by calling
 *      mv_image_tracking_model_create()
 * @post Saved model can be loaded later by calling
 *       mv_image_tracking_model_load() function

 * @see mv_image_tracking_model_create_open()
 * @see mv_image_tracking_model_load_open()
 * @see mv_image_tracking_model_destroy_open()
 */
int mv_image_tracking_model_save_open(
        const char *file_name, mv_image_tracking_model_h image_tracking_model);

/**
 * @brief Loads an image tracking model from the file.
 *
 * @since_tizen 3.0
 * @remarks @a image_tracking_model is loaded from the application's data directory.
 *          @a image_tracking_model must be destroyed using
 *          @ref mv_image_tracking_model_destroy.
 * @param [in]  file_name               Name of file to load model
 * @param [out] image_tracking_model    The handle to the image tracking
 *                                      model to be filled
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_INVALID_PATH Invalid path
 * @retval #MEDIA_VISION_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #MEDIA_VISION_ERROR_PERMISSION_DENIED Not permitted
 *
 * @pre Image tracking model handle can be preliminary saved with
 *      mv_image_tracking_model_save() function
 *
 * @see mv_image_tracking_model_save_open()
 * @see mv_image_tracking_model_destroy_open()
 */
int mv_image_tracking_model_load_open(
        const char *file_name, mv_image_tracking_model_h *image_tracking_model);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TIZEN_MEDIAVISION_IMAGE_OPEN_H__ */
