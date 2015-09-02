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

#ifndef __TIZEN_MEDIAVISION_FACE_LIC_H__
#define __TIZEN_MEDIAVISION_FACE_LIC_H__

#include "mv_face.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @file  mv_face_lic.h
 * @brief This file contains the Media Vision Face licensed API
 */

/******************/
/* Face detection */
/******************/

/**
 * @brief Performs face detection on the @a source for the @a engine_conf.
 * @details Use this function to launch face detection algorithm configured by
 *          @a engine_conf configuration. Each time when mv_face_detect_lic is
 *          called, @a detected_cb will receive a set of the detected
 *          faces at the media source.
 *
 * @since_tizen 3.0
 * @param [in] source         The handle to the source of the media where faces
 *                            will be detected
 * @param [in] engine_cfg     The handle to the configuration of engine will be
 *                            used for detecting. If NULL, then default settings
 *                            will be used.
 * @param [in] detected_cb    The callback which will be called for all face
 *                            locations detected on media source. This callback
 *                            will receive detecting results
 * @param [in] user_data      The user data passed from the code where
 *                            @ref mv_face_detect_lic() is invoked. This data will be
 *                            accessible from @a detected_cb callback.
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Create a source handle by calling @ref mv_create_source()
 * @post @a detected_cb will be called to process detection results
 *
 * @see mv_face_detected_cb
 */
int mv_face_detect_lic(
        mv_source_h source,
        mv_engine_config_h engine_cfg,
        mv_face_detected_cb detected_cb,
        void *user_data);


/********************/
/* Face recognition */
/********************/

/**
 * @brief Performs face recognition on the @a source image.
 * @details Use this function to launch face recognition algorithm configured by
 *          @a engine_conf configuration using @a recognition_model recognition
 *          model. Each time when @ref mv_face_recognize_lic() is called,
 *          @a recognized_cb will receive recognition results:\n
 *          - Location in the @a source of the face has been recognized;
 *          - Label of the face has been recognized;
 *          - Confidence of the @a recognition_model that face has been
 *            recognized correctly (value from 0.0 to 1.0).
 *
 * @since_tizen 3.0
 * @remarks Using of untrained or weakly trained recognition models will cause
 *          not accurate results even if resulting confidence will be high.
 *          Use @ref mv_face_recognition_model_learn_lic() function before
 *          @ref mv_face_recognize_lic() call. Best results can be achieved
 *          when big set of face image examples were added by
 *          @ref mv_face_recognition_model_add_lic() before
 *          @ref mv_face_recognition_model_learn_lic() call.
 * @param [in] source              The handle to the source of the media to
 *                                 recognize face(s) for
 * @param [in] recognition_model   The handle to the model will be used for
 *                                 recognition
 * @param [in] engine_cfg          The handle to the configuration of engine
 *                                 will be used for recognition. If NULL, then
 *                                 default settings will be used
 * @param [in] face_location       Rectangular box bounding face image on the
 *                                 @a source. If NULL, then full source will be
 *                                 analyzed
 * @param [in] recognized_cb       The callback which will be called for the
 *                                 face recognition results on the @a source.
 * @param [in] user_data           The user data passed from the code where
 *                                 @ref mv_face_recognize_lic() is invoked.
 *                                 This data will be accessible from
 *                                 @a recognized_cb callback.
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Create a source handle by calling @ref mv_create_source()
 * @pre Create a face recognition model handle by calling
 *      @ref mv_face_recognition_model_create_lic()
 * @post @a recognized_cb will be called to process recognition results
 *
 * @see mv_face_recognized_cb
 */
int mv_face_recognize_lic(
        mv_source_h source,
        mv_face_recognition_model_h recognition_model,
        mv_engine_config_h engine_cfg,
        mv_rectangle_s *face_location,
        mv_face_recognized_cb recognized_cb,
        void *user_data);


/*****************/
/* Face tracking */
/*****************/

/**
 * @brief Performs face tracking on the @a source for the @a tracking_model.
 * @details Use this function to launch face tracking algorithm configured by
 *          @a engine_conf configuration using @a tracking_model tracking
 *          model. Each time when this function is called, @a tracked_cb
 *          will receive updated @a tracking_model, new location determined for
 *          the tracked face and model confidence that location is determined
 *          correctly.
 *
 * @since_tizen 3.0
 * @remarks To allow correct tracking @a tracking_model has to be already used
 *          in previous tracking process(es) or prepared with
 *          @ref mv_face_tracking_model_prepare_lic(). Preparation requires
 *          specifying the face location for the @a source on which tracking was
 *          started. I.e. @ref mv_face_tracking_model_prepare_lic() function
 *          has to be called at least once before this method call.
 * @param [in] source           The handle to the source of the media to
 *                              recognize face for
 * @param [in] tracking_model   The handle to the model will be used for
 *                              tracking
 * @param [in] engine_cfg       The handle to the configuration of engine will
 *                              be used for tracking. If NULL, the default
 *                              configuration will be used.
 * @param [in] tracked_cb       The callback which will be called for tracking
 *                              event on the @a source where face would be
 *                              tracked. This callback will receive tracking
 *                              results
 * @param [in] do_learn         The model learning flag. If it is set @c true
 *                              then model will try to learn (if it supports
 *                              learning feature), otherwise model will be not
 *                              learned during the invoking tracking iteration.
 *                              Learning process improves tracking correctness,
 *                              but can decrease tracking performance
 * @param [in] user_data        The user data passed from the code where
 *                              @ref mv_face_track_lic() is invoked. This data
 *                              will be accessible from @a tracked_cb callback
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Create a source handle by calling @ref mv_create_source()
 * @pre Create a face tracking model handle by calling
 *      @ref mv_face_tracking_model_create_lic()
 * @post @a tracked_cb will be called to process tracking results
 *
 * @see mv_face_tracked_cb
 */
int mv_face_track_lic(
        mv_source_h source,
        mv_face_tracking_model_h tracking_model,
        mv_engine_config_h engine_cfg,
        mv_face_tracked_cb tracked_cb,
        bool do_learn,
        void *user_data);


/********************************/
/* Recognition of eye condition */
/********************************/

/**
 * @brief Determines eye-blink condition for @a face_location on media @a source.
 * @details Use this function to recognize eye-blink condition for the face
 *          bounded by @a face_location at @a source.
 *
 * @since_tizen 3.0
 * @param [in] source           The handle to the source of the media to
 *                              recognize eye-blink condition for
 * @param [in] engine_cfg       The handle to the configuration of engine
 *                              will be used for eye-blink condition
 *                              recognition. If NULL, the default configuration
 *                              will be used.
 * @param [in] face_location    The location bounding the face at the @a source
 * @param [in] eye_condition_recognized_cb    The callback for processing result
 *                              of eye-blink condition recognition
 * @param [in] user_data        The user data passed from the code where
 *                              @ref mv_face_eye_condition_recognize_lic() is
 *                              invoked. This data will be accessible from
 *                              @a eye_condition_recognized_cb callback
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Create a source handle by calling @ref mv_create_source()
 *
 * @see mv_face_eye_condition_recognized_cb
 */
int mv_face_eye_condition_recognize_lic(
        mv_source_h source,
        mv_engine_config_h engine_cfg,
        mv_rectangle_s face_location,
        mv_face_eye_condition_recognized_cb eye_condition_recognized_cb,
        void *user_data);


/************************************/
/* Recognition of facial expression */
/************************************/

/**
 * @brief Determines facial expression for @a face_location on media @a source.
 * @details Use this function to determine facial expression for the face
 *          bounded by @a face_location at @a source.
 *
 * @since_tizen 3.0
 * @param [in] source           The handle to the source of the media
 *                              to recognize facial expression for
 * @param [in] engine_cfg       The handle to the configuration of
 *                              engine will be used for expression recognition
 * @param [in] face_location    The location bounding the face at the @a source
 * @param [in] expression_recognized_cb   The callback for processing result
 *                              of facial expression determining
 * @param [in] user_data        The user data passed from the code where
 *                              @ref mv_face_facial_expression_recognize_lic() is
 *                              invoked. This data will be accessible from
 *                              @a expression_recognized_cb callback.
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Create a source handle by calling @ref mv_create_source()
 * @pre Create a face engine configuration handle by calling @ref mv_create_engine_config()
 *
 * @see mv_face_facial_expression_recognized_cb
 */
int mv_face_facial_expression_recognize_lic(
        mv_source_h source,
        mv_engine_config_h engine_cfg,
        mv_rectangle_s face_location,
        mv_face_facial_expression_recognized_cb expression_recognized_cb,
        void *user_data);

/*******************************/
/* Recognition model behavior */
/*******************************/

/**
 * @brief Creates a face recognition model handle.
 * @details Use this function to create default face recognition model. Creating
 *          process is defined by concrete face engine library. After creation
 *          recognition model has to be learned with
 *          @ref mv_face_recognition_model_learn_lic() function to provide
 *          appropriate results of face recognition functionality. Or learned
 *          earlier model can be loaded by
 *          @ref mv_face_recognition_model_load_lic() function.
 *
 * @since_tizen 3.0
 * @remarks It can cause incompatibility issues when saved models (see
 *          @ref mv_face_recognition_model_save_lic(),
 *          @ref mv_face_recognition_model_load_lic() functions documentation)
 *          are used in applications for different platforms which use different
 *          computer vision libraries underlying this API.
 * @remarks You must release @a recognition_model by using
 *          @ref mv_face_recognition_model_destroy_lic() function.
 * @param [out] recognition_model    The handle to the recognition model to be
 *                                   created
 *
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @post Model can be loaded from the file after creation. Use
 *       @ref mv_face_recognition_model_load_lic() function to load it from file
 * @post Release @a recognition_model by using
 *       @ref mv_face_recognition_model_destroy_lic() function when it is not needed
 *       anymore
 *
 * @see mv_face_recognition_model_destroy_lic()
 */
int mv_face_recognition_model_create_lic(
        mv_face_recognition_model_h *recognition_model);

/**
 * @brief Destroys the face recognition model handle and releases all its
 *        resources.
 *
 * @since_tizen 3.0
 * @param [in] recognition_model    The handle to the face recognition model to
 *                                  be destroyed
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Create recognition model by using @ref mv_face_recognition_model_create_lic()
 *
 * @see mv_face_recognition_model_create_lic()
 */
int mv_face_recognition_model_destroy_lic(
        mv_face_recognition_model_h recognition_model);

/**
 * @brief Creates a copy of existed recognition model handle and clones all its
 *        resources.
 *
 * @since_tizen 3.0
 * @remarks Cloning perform not only handle copy, but also copies all internal
 *          resources of the model. @a dst must be released using
 *          @a mv_face_recognition_model_destroy_open_lic().
 * @param [in]     src    The handle to the recognition model to be copied
 * @param [out]    dst    The handle to the copy of existed recognition model
 *                        specified as @a src
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Create face recognition handles by calling
 *      @ref mv_face_recognition_model_create_lic()
 *
 * @see mv_face_recognition_model_create_lic()
 */
int mv_face_recognition_model_clone_lic(
        mv_face_recognition_model_h src,
        mv_face_recognition_model_h *dst);

/**
 * @brief Saves recognition model to the file.
 *
 * @since_tizen 3.0
 * @remarks This function doesn't save added by
 *          @ref mv_face_recognition_model_add_lic() function face
 *          image examples. This examples can be removed by
 *          @ref mv_face_recognition_model_reset_lic() function
 *          if it is needed to clear the memory.
 * @remarks After model is saved to the file, it can be loaded from this file
 *          by @ref mv_face_recognition_model_load_lic() function.
 * @param [in] file_name            Name of the file to save the model
 * @param [in] recognition_model    The handle to the recognition model to be
 *                                  saved to the file
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Create a face recognition handle by calling
 *      @ref mv_face_recognition_model_create_lic() function
 * @post Saved model can be loaded later by calling
 *      @ref mv_face_recognition_model_load_lic() function
 *
 * @see mv_face_recognition_model_load_lic()
 * @see mv_face_recognition_model_create_lic()
 */
int mv_face_recognition_model_save_lic(
        const char *file_name,
        mv_face_recognition_model_h recognition_model);

/**
 * @brief Loads recognition model from file.
 *
 * @since_tizen 3.0
 * @remarks This function doesn't modify the set of face image examples added
 *          with @ref mv_face_recognition_model_add_lic() function.
 *          Model will be loaded from file without loss of collected examples.
 *          If you want to free memory from examples, use
 *          @ref mv_face_recognition_model_reset_lic() function.
 *          It is recommended to clear the memory if learning algorithm doesn't
 *          support reinforcement learning.
 * @remarks @a recognition_model is loaded from the application's data directory.
 *          @a recognition_model must be destroyed using
 *          @ref mv_face_recognition_model_destroy_open_lic().
 * @param [in]  file_name            Name of file to load the model
 * @param [out] recognition_model    The handle to the recognition model
 *                                   to be loaded from the file
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Create a face recognition handle by calling
 *      @ref mv_face_recognition_model_create_lic() function
 *
 * @see mv_face_recognition_model_save_lic()
 * @see mv_face_recognition_model_create_lic()
 */
int mv_face_recognition_model_load_lic(
        const char *file_name,
        mv_face_recognition_model_h *recognition_model);

/**
 * @brief Adds face image example to be used for face recognition model learning
 *        with @ref mv_face_recognition_model_learn_lic().
 *
 * @since_tizen 3.0
 * @remarks It is possible to destroy @a source after calling this method.
 *          Source isn't used for learning directly.
 * @remarks Face image @a example_location location can be determined using
 *          @ref mv_face_detect_lic function.
 * @param [in] source               The handle to @a source that contains face
 *                                  image
 * @param [in] recognition_model    The handle to the recognition model which
 *                                  could be learned based on example
 * @param [in] example_location     The pointer to the rectangular location of
 *                                  the face image at the source image. If NULL,
 *                                  then full image will be analyzed as the face
 *                                  image
 * @param [in] face_label           The label that identifies face for which
 *                                  example is adding. Specify the same labels
 *                                  for the face images of a single person when
 *                                  calling this method. Has to be unique for
 *                                  each face
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Create a face recognition handle by calling
 *      @ref mv_face_recognition_model_create_lic() function
 * @post When appropriate amount of face image examples is added to the
 *       @a recognition_model, this model has to be learned by
 *       @ref mv_face_recognition_model_learn_lic() function call. Only after
 *       learning of the model it can be used for face recognition with
 *       @a mv_face_recognize_lic() function
 *
 * @see mv_face_recognition_model_reset_lic()
 * @see mv_face_recognition_model_learn_lic()
 */
int mv_face_recognition_model_add_lic(
        const mv_source_h source,
        mv_face_recognition_model_h recognition_model,
        const mv_rectangle_s *example_location,
        int face_label);

/**
 * @brief Remove from @a recognition_model all collected with
 *        @ref mv_face_recognition_model_add_lic() function
 *        face examples labeled with @a face_label.
 *
 * @since_tizen 3.0
 * @remarks Be aware that if this function is called before
 *          @ref mv_face_recognition_model_learn_lic() function call, all or
 *          part of the required for learning data will be lost. It means that
 *          face image examples determined by the @a face_label label will be
 *          removed from the model and not taken taken into account when
 *          @ref mv_face_recognition_model_learn_lic() will be called next
 *          time.
 * @remarks Call of this function will free all the memory has been allocated
 *          during previous
 *          @ref mv_face_recognition_model_add_lic() calls for
 *          the corresponding @a face_label label.
 * @param [in] recognition_model    The handle to the recognition model for
 *                                  which face image examples will be reset.
 * @param [in] face_label           The label that identifies face for which
 *                                  examples will be removed from the
 *                                  @a recognition_model. If NULL, then all
 *                                  known by @a recognition_model face image
 *                                  examples will be removed
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @see mv_face_recognition_model_add_lic()
 * @see mv_face_recognition_model_learn_lic()
 */
int mv_face_recognition_model_reset_lic(
        mv_face_recognition_model_h recognition_model,
        const int *face_label);

/**
 * @brief Learns face recognition model.
 * @details Before you start learning process, face recognition models has to be
 *          filled with training data - face image examples. These examples has
 *          to be provided by
 *          mv_face_recognition_model_add_lic() function. Usually,
 *          recognition accuracy is increased when number of not identical
 *          examples is large. But it depends of the used learning algorithm.
 *
 * @since_tizen 3.0
 * @remarks Common flow is to collect face examples as much as possible, add
 *          them to the recognition model with
 *          @ref mv_face_recognition_model_add_lic(), then call
 *          @ref mv_face_recognition_model_learn_lic() for this recognition
 *          model to learn it (or reinforce the model if reinforcement learning
 *          is supported by the used algorithm).
 * @remarks Selection of the learning algorithm can be performed by setting
 *          corresponding attributes for the @a engine_cfg. You can check
 *          supported by @a engine_cfg attributes using
 *          @ref mv_engine_config_foreach_supported_attribute() function call.
 * @param [in]     engine_cfg           The handle to the configuration of
 *                                      engine will be used for learning of the
 *                                      recognition models. If NULL, then
 *                                      default settings will be used
 * @param [in,out] recognition_model    The model which will be learned. After
 *                                      learning process these model may be
 *                                      changed, so
 *                                      @ref mv_face_recognize_lic() results
 *                                      may differ before and after method call
 *                                      respectively to the face examples
 *                                      collected for the @a recognition_model
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Create a face engine configuration handle by calling
 *      @ref mv_create_engine_config() and set supported parameters if
 *      needed. Or just set @a engine_cfg as NULL to learn with default settings
 * @pre Create a face recognition model handles by calling
 *      @ref mv_face_recognition_model_create_lic() function
 * @pre Add face image examples to the @a recognition_model by calling
 *      @ref mv_face_recognition_model_add_lic() function
 * @post If it is not planned to learn the model again, clear memory by
 *       @ref mv_face_recognition_model_reset_lic() function
 * @post When model has been learned, it can be used for face recognition with
 *       @ref mv_face_recognize_lic() function
 *
 * @see mv_face_recognition_model_add_lic()
 * @see mv_face_recognition_model_reset_lic()
 * @see mv_face_recognize_lic()
 */
int mv_face_recognition_model_learn_lic(
        mv_engine_config_h engine_cfg,
        mv_face_recognition_model_h recognition_model);

/**
 * @brief Queries labels list and number of labels had been learned by the model.
 *
 * @since_tizen 3.0
 * @remarks @a labels array has to be released using free().
 * @param [in]  recognition_model    The handle to the recognition model for
 *                                   which set of the learned labels will be
 *                                   queried
 * @param [out] labels               The array which will be filled with labels
 *                                   had been learned by the model
 * @param [out] number_of_labels     The number of labels in @a labels array
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Add face image examples with labels to the @a recognition_model by
 *      calling the @ref mv_face_recognition_model_add_lic()
 *      function
 * @pre Learn the @a recognition_model by labeled examples using
 *      @ref mv_face_recognition_model_learn_lic() function
 * @post @a labels array has to be freed in the function invoking code
 *
 * @see mv_face_recognition_model_add_lic()
 * @see mv_face_recognition_model_reset_lic()
 * @see mv_face_recognition_model_learn_lic()
 */
int mv_face_recognition_model_query_labels_lic(
        mv_face_recognition_model_h recognition_model,
        int **labels,
        unsigned int *number_of_labels);

/***************************/
/* Tracking model behavior */
/***************************/

/**
 * @brief Calls this function to create a face tracking model handle.
 * @details Use this function to create default face tracking model handle.
 *          After creation this handle has to be initialized with
 *          @ref mv_face_tracking_model_prepare_lic() function to provide
 *          appropriate results of face tracking functionality. When handle is
 *          prepared, it is possible to use it for tracking on continuous
 *          sequence of the sources. Call
 *          @ref mv_face_tracking_model_prepare_lic() function each time before
 *          starting tracking on the new sequence. The exception is situation
 *          when the new sequence is continuation of the previous sequence for
 *          which model has been tracked.
 *
 * @since_tizen 3.0
 * @param [out] tracking_model    The pointer to the handle to the tracking
 *                                model that will be created
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @post Model can be loaded from the file after creation. Use
 *       @ref mv_face_tracking_model_load_lic() function to load it from file
 * @post Use @ref mv_face_tracking_model_prepare_lic() function before tracking
 *       on the new video or continuous images sequence
 * @post You must release @a tracking_model by using
 *       mv_face_tracking_model_destroy_lic() function when it is not needed
 *       anymore
 *
 * @see mv_face_tracking_model_destroy_lic()
 * @see mv_face_tracking_model_prepare_lic()
 * @see mv_face_tracking_model_load_lic()
 */
int mv_face_tracking_model_create_lic(
        mv_face_tracking_model_h *tracking_model);

/**
 * @brief Calls this function to destroy the face tracking model handle and
 *        release all its resources.
 *
 * @since_tizen 3.0
 * @param [in] tracking_model    The handle to the face tracking model that
 *                               will be destroyed
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Create tracking model by using @ref mv_face_tracking_model_create_lic()
 *
 * @see mv_face_tracking_model_create_lic()
 */
int mv_face_tracking_model_destroy_lic(
        mv_face_tracking_model_h tracking_model);

/**
 * @brief Calls this function to initialize tracking model by the location of the
 *        face to be tracked.
 * @details This function is usually called once after tracking model is created
 *          and each time before tracking is started for the new sequence of
 *          sources which is not the direct continuation of the sequence for
 *          which tracking has been performed before. But it is allowed to call
 *          it between tracking sessions to allow Media Vision start to track
 *          more accurately.
 *
 * @since_tizen 3.0
 * @param [in] tracking_model    The handle to the tracking model that will be
 *                               prepared for tracking on new video or image
 *                               sequence
 * @param [in] engine_cfg        The handle to the configuration of engine
 *                               will be used for model preparing. If NULL, then
 *                               default settings will be used.
 * @param [in] source            The handle to the source where face @a location
 *                               is specified. Usually it is the first frame of
 *                               the video or the first image in the continuous
 *                               image sequence planned to be used for tracking
 * @param [in] location          The quadrangle-shaped location (actually,
 *                               rectangle can be used) determining position
 *                               of the face to be tracked on the @a source. If
 *                               @c NULL, then tracking model will try to find
 *                               previously tracked face by itself. Don't set
 *                               NULL when called first time for the tracking
 *                               model.
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Create a face tracking model handle by calling
 *      @ref mv_face_tracking_model_create_lic() function
 * @pre Create a source handle by calling @ref mv_create_source() function
 * @post When model is prepared, @ref mv_face_track_lic() function can be used
 *       to track on the video or continuous image sequence
 *
 * @see mv_face_tracking_model_create_lic()
 * @see mv_face_track_lic()
 */
int mv_face_tracking_model_prepare_lic(
        mv_face_tracking_model_h tracking_model,
        mv_engine_config_h engine_cfg,
        mv_source_h source,
        mv_quadrangle_s *location);

/**
 * @brief Calls this function to make a copy of existed tracking model handle and
 *        clone all its resources to the copy.
 *
 * @since_tizen 3.0
 * @remarks Cloning performs not only handle copy, but also copies all internal
 *          resources of the model.
 * @param [in]     src    The handle to the tracking model to be copied
 * @param [out]    dst    The handle to the copy of existed tracking model
 *                        specified as @a src
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Create face tracking @a src and @a dst handles by calling
 *      @ref mv_face_tracking_model_create_lic()
 *
 * @see mv_face_tracking_model_create_lic()
 */
int mv_face_tracking_model_clone_lic(
        mv_face_tracking_model_h src,
        mv_face_tracking_model_h *dst);

/**
 * @brief Calls this method to save tracking model to the file.
 *
 * @since_tizen 3.0
 * @remarks @ tracking_model is saved to the application's data directory.
 *          After model is saved to the file, it can be loaded from this file
 *          with @ref mv_face_tracking_model_load_lic() function.
 * @param [in] file_name         Name of the file to save the model
 * @param [in] tracking_model    The handle to the tracking model to be
 *                               saved to the file
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Create a face tracking handle by calling
 *      @ref mv_face_tracking_model_create_lic()
 * @post Saved model can be loaded from file using
 *       @ref mv_face_tracking_model_load_lic() function
 *
 * @see mv_face_tracking_model_load_lic()
 * @see mv_face_tracking_model_create_lic()
 */
int mv_face_tracking_model_save_lic(
        const char *file_name,
        mv_face_tracking_model_h tracking_model);

/**
 * @brief Calls this method to load a tracking model from file.
 *
 * @since_tizen 3.0
 * @remarks @a tracking_model is loaded from the application's data directory.
 *          @a tracking_model must be destroyed using
 *          @ref mv_face_tracking_model_destroy_lic().
 * @param [in] file_name         Name of file to load the model
 * @param [out] tracking_model   The handle to the tracking model to be
 *                               loaded from file
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Models has been saved by @ref mv_face_tracking_model_save_lic()
 *      function can be loaded with this function
 * @post After model has been loaded and if further tracking will be performed
 *       on the video which is not continuation of the last tracking performed
 *       for the model, it is recommended to call
 *       @ref mv_face_tracking_model_prepare_lic() function
 *
 * @see mv_face_tracking_model_save_lic()
 * @see mv_face_tracking_model_create_lic()
 */
int mv_face_tracking_model_load_lic(
        const char *file_name,
        mv_face_tracking_model_h *tracking_model);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TIZEN_MEDIAVISION_FACE_LIC_H__ */
