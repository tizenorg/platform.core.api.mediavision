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

#ifndef __TIZEN_MEDIAVISION_FACE_TYPE_H__
#define __TIZEN_MEDIAVISION_FACE_TYPE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @file  mv_face_type.h
 * @brief This file contains enumerations and handles definition required by
 *        face detect/recognize/track API.
 */

/**
 * @addtogroup CAPI_MEDIA_VISION_FACE_MODULE
 * @{
 */

/**
 * @brief Enumeration for eyes state type.
 *
 * @since_tizen 3.0
 *
 * @see mv_face_eye_condition_recognize()
 */
typedef enum {
	MV_FACE_EYES_OPEN,      /**< Eyes are open */
	MV_FACE_EYES_CLOSED,    /**< Eyes are closed */
	MV_FACE_EYES_NOT_FOUND  /**< The eyes condition wasn't determined */
} mv_face_eye_condition_e;

/**
 * @brief Enumeration for expression types can be determined for faces.
 *
 * @since_tizen 3.0
 *
 * @see mv_face_facial_expression_recognize()
 */
typedef enum {
	MV_FACE_UNKNOWN,    /**< Unknown face expression */
	MV_FACE_NEUTRAL,    /**< Face expression is neutral */
	MV_FACE_SMILE,      /**< Face expression is smiling */
	MV_FACE_SADNESS,    /**< Face expression is sadness */
	MV_FACE_SURPRISE,   /**< Face expression is surprise */
	MV_FACE_ANGER,      /**< Face expression is anger */
	MV_FACE_FEAR,       /**< Face expression is fear */
	MV_FACE_DISGUST,    /**< Face expression is disgust */
} mv_face_facial_expression_e;

/**
 * @brief The handle to the model aggregating recognition face features.
 * @details This handle can be used for faces recognizing with
 *          @ref mv_face_recognize() function. Handle has to be created by
 *          @ref mv_face_recognition_model_create() and destroyed by
 *          @ref mv_face_recognition_model_destroy() functions. To use
 *          recognition models effectively learning process has to be performed
 *          before recognition. In other words, appropriate set of the face
 *          image examples has to be collected with
 *          @ref mv_face_recognition_model_add() function before
 *          @ref mv_face_recognition_model_learn() function call, then
 *          recognition can be performed with @ref mv_face_recognize().
 *
 * @since_tizen 3.0
 *
 * @see mv_face_recognition_model_create()
 * @see mv_face_recognition_model_destroy()
 * @see mv_face_recognition_model_learn()
 */
typedef void *mv_face_recognition_model_h;

/**
 * @brief The handle to the model aggregating tracking face features.
 * @details This model can be used for face tracking with @ref mv_face_track()
 *          function. Handle has to be created by
 *          @ref mv_face_tracking_model_create() and destroyed by
 *          @ref mv_face_tracking_model_destroy() function. Tracking model can
 *          be improved during tracking task and allows to track face more
 *          accurately. So, you can create several tracking models independently
 *          by default, then apply tracking task for each of them. After some
 *          tracking these models will be different and each of them will be
 *          more efficient for tracking of face for which was created.
 *
 * @since_tizen 3.0
 * @remarks Create each tracking model for single face.
 *
 * @see mv_face_tracking_model_create()
 * @see mv_face_tracking_model_destroy()
 */
typedef void *mv_face_tracking_model_h;

/**
 * @}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TIZEN_MEDIAVISION_FACE_TYPE_H__ */
