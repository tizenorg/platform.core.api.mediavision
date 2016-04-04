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

#ifndef __TIZEN_MEDIAVISION_SURVEILLANCE_H__
#define __TIZEN_MEDIAVISION_SURVEILLANCE_H__

#include <mv_common.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @file  mv_surveillance.h
 * @brief This file contains the Media Vision Surveillance API.
 */

/**
 * @addtogroup CAPI_MEDIA_VISION_SURVEILLANCE_EVENT_TYPES
 * @{
 */

/**
 * @brief Name of the movement detection event type.
 * @details This is common event for a movement detection. When this event
 *          occurs @ref mv_surveillance_result_h allowed from callback can be
 *          used to get number of regions where movement has been detected and
 *          their positions. Out parameters (can be accessed in the
 *          @ref mv_surveillance_event_occurred_cb callback using
 *          @ref mv_surveillance_get_result_value() function):
 *          * @ref MV_SURVEILLANCE_MOVEMENT_NUMBER_OF_REGIONS - the number
 *            of regions where movement has been detected;\n
 *          * @ref MV_SURVEILLANCE_MOVEMENT_REGIONS - the set
 *            of rectangular regions where movement has been detected.
 *
 * @since_tizen 3.0
 */
#define MV_SURVEILLANCE_EVENT_TYPE_MOVEMENT_DETECTED \
                "MV_SURVEILLANCE_EVENT_MOVEMENT_DETECTED"

/**
 * @brief Name of the event result value that contains number of regions where
 *        movement was detected.
 * @details This event result value can be accessed after event triggers of
 *          @ref MV_SURVEILLANCE_EVENT_TYPE_MOVEMENT_DETECTED type activation.
 *          Result value is of @c size_t type, so has to be casted as in the
 *          following example:
 *          @code{.c}
 *          void event_occurred_cb(mv_surveillance_event_trigger_h trigger,
 *                                 mv_source_h source,
 *                                 int video_stream_id,
 *                                 mv_surveillance_result_h event_result,
 *                                 void *user_data)
 *          {
 *              const char *event_type = NULL;
 *              int err = mv_surveillance_get_event_trigger_type(trigger,
 *                                                               event_type);
 *              if (MEDIA_VISION_ERROR_NONE != err) return;
 *
 *              if (0 == strncmp(event_type,
 *                               MV_SURVEILLANCE_EVENT_TYPE_MOVEMENT_DETECTED,
 *                               255))
 *              {
 *                  size_t move_regions_num = 0;
 *                  err = mv_surveillance_get_result_value(
 *                            event_result,
 *                            MV_SURVEILLANCE_MOVEMENT_NUMBER_OF_REGIONS,
 *                            &move_regions_num);
 *                  if (MEDIA_VISION_ERROR_NONE != err)
 *                      return;
 *
 *                  // Do something with number of regions where movement
 *                  // was detected...
 *              }
 *          }
 *          @endcode
 *
 * @since_tizen 3.0
 */
#define MV_SURVEILLANCE_MOVEMENT_NUMBER_OF_REGIONS \
                "NUMBER_OF_MOVEMENT_REGIONS"

/**
 * @brief Name of the event result value that contains rectangular regions where
 *        movement was detected.
 * @details This event result value can be accessed after event triggers of
 *          @ref MV_SURVEILLANCE_EVENT_TYPE_MOVEMENT_DETECTED type activation.
 *          Result value is of @a mv_rectangle_s array type, so has to be casted
 *          as in the following example:
 *          @code{.c}
 *          void event_occurred_cb(mv_surveillance_event_trigger_h trigger,
 *                                 mv_source_h source,
 *                                 int video_stream_id,
 *                                 mv_surveillance_result_h event_result,
 *                                 void *user_data)
 *          {
 *              const char *event_type = NULL;
 *              int err = mv_surveillance_get_event_trigger_type(trigger,
 *                                                               event_type);
 *              if (MEDIA_VISION_ERROR_NONE != err) return;
 *
 *              if (0 == strncmp(event_type,
 *                               MV_SURVEILLANCE_EVENT_TYPE_MOVEMENT_DETECTED,
 *                               255))
 *              {
 *                  size_t move_regions_num = 0;
 *                  err = mv_surveillance_get_result_value(
 *                            event_result,
 *                            MV_SURVEILLANCE_MOVEMENT_NUMBER_OF_REGIONS,
 *                            &move_regions_num);
 *
 *                  if (MEDIA_VISION_ERROR_NONE != err || 0 == move_regions_num)
 *                      return;
 *
 *                  mv_rectangle_s *regions =
 *                            (mv_rectangle_s*)
 *                            malloc(sizeof(mv_rectangle_s) * move_regions_num);
 *                  err = mv_surveillance_get_result_value(
 *                            event_result,
 *                            MV_SURVEILLANCE_MOVEMENT_REGIONS,
 *                            regions);
 *
 *                  // Do something with movement regions...
 *
 *                  free (regions);
 *              }
 *          }
 *          @endcode
 *
 * @since_tizen 3.0
 */
#define MV_SURVEILLANCE_MOVEMENT_REGIONS "MOVEMENT_REGIONS"

/**
 * @brief Name of the person appearance/disappearance event type.
 * @details This is common event for a person appearing (disappearing).
 *          The result will be:
 *          * a number of persons, which were appeared and their positions;\n
 *          * a number of persons, which were tracked and their positions;\n
 *          * a number of persons, which were disappeared and their last
 *            positions.\n
 *          For the first time when a source is loaded, a result should contain
 *          all detected persons (detection will be made using face detection
 *          API). Next time when the source should be loaded, the previously
 *          detected persons will be tracked and new persons will be detected.
 *          For the previously detected persons their locations will be updated.
 *          Out parameters (can be accessed in the
 *          @ref mv_surveillance_event_occurred_cb callback using
 *          @ref mv_surveillance_get_result_value() function):
 *          * @ref MV_SURVEILLANCE_PERSONS_APPEARED_NUMBER - the number
 *            of persons which were appeared;\n
 *          * @ref MV_SURVEILLANCE_PERSONS_APPEARED_LOCATIONS - the locations
 *            of persons which were appeared;\n
 *          * @ref MV_SURVEILLANCE_PERSONS_TRACKED_NUMBER - the number
 *            of persons which were tracked;\n
 *          * @ref MV_SURVEILLANCE_PERSONS_TRACKED_LOCATIONS - the locations
 *            of persons which were tracked;\n
 *          * @ref MV_SURVEILLANCE_PERSONS_DISAPPEARED_NUMBER - the number
 *            of persons which were disappeared;\n
 *          * @ref MV_SURVEILLANCE_PERSONS_DISAPPEARED_LOCATIONS - the locations
 *            of persons which were disappeared.
 *
 * @since_tizen 3.0
 */
#define MV_SURVEILLANCE_EVENT_TYPE_PERSON_APPEARED_DISAPPEARED \
                "MV_SURVEILLANCE_EVENT_PERSON_APPEARED_DISAPEARED"

/**
 * @brief Name of the event result value that contains number
 *        of persons that have been appeared.
 * @details This event result value can be accessed after event triggers of
 *          @ref MV_SURVEILLANCE_EVENT_TYPE_PERSON_APPEARED_DISAPPEARED type
 *          activation. Result value is of @c size_t type, so has to be casted
 *          as in the following example:
 *          @code{.c}
 *          void event_occurred_cb(mv_surveillance_event_trigger_h trigger,
 *                                 mv_source_h source,
 *                                 int video_stream_id,
 *                                 mv_surveillance_result_h event_result,
 *                                 void *user_data)
 *          {
 *              const char *event_type = NULL;
 *              int err = mv_surveillance_get_event_trigger_type(trigger,
 *                                                               event_type);
 *              if (MEDIA_VISION_ERROR_NONE != err) return;
 *
 *              if (0 == strncmp(event_type,
 *                               MV_SURVEILLANCE_EVENT_TYPE_PERSON_APPEARED_DISAPPEARED,
 *                               255))
 *              {
 *                  size_t appear_person_num = 0;
 *                  err = mv_surveillance_get_result_value(
 *                            event_result,
 *                            MV_SURVEILLANCE_PERSONS_APPEARED_NUMBER,
 *                            &appear_person_num);
 *                  if (MEDIA_VISION_ERROR_NONE != err)
 *                      return;
 *
 *                  // Do something with number of appeared persons...
 *              }
 *          }
 *          @endcode
 *
 * @since_tizen 3.0
 */
#define MV_SURVEILLANCE_PERSONS_APPEARED_NUMBER \
                "NUMBER_OF_APPEARED_PERSONS"

/**
 * @brief Name of the event result value that contains number
 *        of persons that have been disappeared.
 * @details This event result value can be accessed after event triggers of
 *          @ref MV_SURVEILLANCE_EVENT_TYPE_PERSON_APPEARED_DISAPPEARED type
 *          activation. Result value is of @c size_t type, so has to be casted
 *          as in the following example:
 *          @code{.c}
 *          void event_occurred_cb(mv_surveillance_event_trigger_h trigger,
 *                                 mv_source_h source,
 *                                 int video_stream_id,
 *                                 mv_surveillance_result_h event_result,
 *                                 void *user_data)
 *          {
 *              const char *event_type = NULL;
 *              int err = mv_surveillance_get_event_trigger_type(trigger,
 *                                                               event_type);
 *              if (MEDIA_VISION_ERROR_NONE != err) return;
 *
 *              if (0 == strncmp(event_type,
 *                               MV_SURVEILLANCE_EVENT_TYPE_PERSON_APPEARED_DISAPPEARED,
 *                               255))
 *              {
 *                  size_t disappear_person_num = 0;
 *                  err = mv_surveillance_get_result_value(
 *                            event_result,
 *                            MV_SURVEILLANCE_PERSONS_DISAPPEARED_NUMBER,
 *                            &disappear_person_num);
 *                  if (MEDIA_VISION_ERROR_NONE != err)
 *                      return;
 *
 *                  // Do something with number of disappeared persons...
 *              }
 *          }
 *          @endcode
 *
 * @since_tizen 3.0
 */
#define MV_SURVEILLANCE_PERSONS_DISAPPEARED_NUMBER \
                "NUMBER_OF_DISAPPEARED_PERSONS"

/**
 * @brief Name of the event result value that contains number
 *        of persons that have been tracked.
 * @details This event result value can be accessed after event triggers of
 *          @ref MV_SURVEILLANCE_EVENT_TYPE_PERSON_APPEARED_DISAPPEARED type
 *          activation. Result value is of @c size_t type, so has to be casted
 *          as in the following example:
 *          @code{.c}
 *          void event_occurred_cb(mv_surveillance_event_trigger_h trigger,
 *                                 mv_source_h source,
 *                                 int video_stream_id,
 *                                 mv_surveillance_result_h event_result,
 *                                 void *user_data)
 *          {
 *              const char *event_type = NULL;
 *              int err = mv_surveillance_get_event_trigger_type(trigger,
 *                                                               event_type);
 *              if (MEDIA_VISION_ERROR_NONE != err) return;
 *
 *              if (0 == strncmp(event_type,
 *                               MV_SURVEILLANCE_EVENT_TYPE_PERSON_APPEARED_DISAPPEARED,
 *                               255))
 *              {
 *                  size_t tracked_person_num = 0;
 *                  err = mv_surveillance_get_result_value(
 *                            event_result,
 *                            MV_SURVEILLANCE_PERSONS_TRACKED_NUMBER,
 *                            &tracked_person_num);
 *                  if (MEDIA_VISION_ERROR_NONE != err)
 *                      return;
 *
 *                  // Do something with number of tracked persons...
 *              }
 *          }
 *          @endcode
 *
 * @since_tizen 3.0
 */
#define MV_SURVEILLANCE_PERSONS_TRACKED_NUMBER \
                "NUMBER_OF_TRACKED_PERSONS"

/**
 * @brief Name of the event result value that contains a set of rectangular
 *        locations where appearances of the persons were detected.
 * @details This event result value can be accessed after event triggers of
 *          @ref MV_SURVEILLANCE_EVENT_TYPE_PERSON_APPEARED_DISAPPEARED type
 *          activation. Result value is of @a mv_rectangle_s array type, so has
 *          to be casted as in the following example:
 *          @code{.c}
 *          void event_occurred_cb(mv_surveillance_event_trigger_h trigger,
 *                                 mv_source_h source,
 *                                 int video_stream_id,
 *                                 mv_surveillance_result_h event_result,
 *                                 void *user_data)
 *          {
 *              const char *event_type = NULL;
 *              int err = mv_surveillance_get_event_trigger_type(trigger,
 *                                                               event_type);
 *              if (MEDIA_VISION_ERROR_NONE != err) return;
 *
 *              if (0 == strncmp(event_type,
 *                               MV_SURVEILLANCE_EVENT_TYPE_PERSON_APPEARED_DISAPPEARED,
 *                               255))
 *              {
 *                  size_t appear_person_num = 0;
 *                  err = mv_surveillance_get_result_value(
 *                            event_result,
 *                            MV_SURVEILLANCE_PERSONS_APPEARED_NUMBER,
 *                            &appear_person_num);
 *
 *                  if (MEDIA_VISION_ERROR_NONE != err ||
 *                      0 == appear_person_num) return;
 *
 *                  mv_rectangle_s *appear_locations =
 *                            (mv_rectangle_s*)
 *                            malloc(sizeof(mv_rectangle_s) * appear_person_num);
 *                  err = mv_surveillance_get_result_value(
 *                            event_result,
 *                            MV_SURVEILLANCE_PERSONS_APPEARED_LOCATIONS,
 *                            appear_locations);
 *
 *                  // Do something with locations where persons were
 *                  // appeared...
 *
 *                  free (appear_locations);
 *              }
 *          }
 *          @endcode
 *
 * @since_tizen 3.0
 */
#define MV_SURVEILLANCE_PERSONS_APPEARED_LOCATIONS \
                "APPEARED_PERSONS_LOCATIONS"

/**
 * @brief Name of the event result value that contains a set of rectangular
 *        locations where disappearances of the persons were detected.
 * @details This event result value can be accessed after event triggers of
 *          @ref MV_SURVEILLANCE_EVENT_TYPE_PERSON_APPEARED_DISAPPEARED type
 *          activation. Result value is of @a mv_rectangle_s array type, so has
 *          to be casted as in the following example:
 *          @code{.c}
 *          void event_occurred_cb(mv_surveillance_event_trigger_h trigger,
 *                                 mv_source_h source,
 *                                 int video_stream_id,
 *                                 mv_surveillance_result_h event_result,
 *                                 void *user_data)
 *          {
 *              const char *event_type = NULL;
 *              int err = mv_surveillance_get_event_trigger_type(trigger,
 *                                                               event_type);
 *              if (MEDIA_VISION_ERROR_NONE != err) return;
 *
 *              if (0 == strncmp(event_type,
 *                               MV_SURVEILLANCE_EVENT_TYPE_PERSON_APPEARED_DISAPPEARED,
 *                               255))
 *              {
 *                  size_t disappear_person_num = 0;
 *                  err = mv_surveillance_get_result_value(
 *                            event_result,
 *                            MV_SURVEILLANCE_PERSONS_DISAPPEARED_NUMBER,
 *                            &disappear_person_num);
 *
 *                  if (MEDIA_VISION_ERROR_NONE != err ||
 *                      0 == disappear_person_num) return;
 *
 *                  mv_rectangle_s *disappear_locations =
 *                            (mv_rectangle_s*)
 *                            malloc(sizeof(mv_rectangle_s) * disappear_person_num);
 *                  err = mv_surveillance_get_result_value(
 *                            event_result,
 *                            MV_SURVEILLANCE_PERSONS_DISAPPEARED_LOCATIONS,
 *                            disappear_locations);
 *
 *                  // Do something with locations where persons were
 *                  // disappeared...
 *
 *                  free (disappear_locations);
 *              }
 *          }
 *          @endcode
 *
 * @since_tizen 3.0
 */
#define MV_SURVEILLANCE_PERSONS_DISAPPEARED_LOCATIONS \
                "DISAPPEARED_PERSONS_LOCATIONS"

/**
 * @brief Name of the event result value that contains a set of rectangular
 *        locations where persons were tracked.
 * @details This event result value can be accessed after event triggers of
 *          @ref MV_SURVEILLANCE_EVENT_TYPE_PERSON_APPEARED_DISAPPEARED type
 *          activation. Result value is of @a mv_rectangle_s array type, so has
 *          to be casted as in the following example:
 *          @code{.c}
 *          void event_occurred_cb(mv_surveillance_event_trigger_h trigger,
 *                                 mv_source_h source,
 *                                 int video_stream_id,
 *                                 mv_surveillance_result_h event_result,
 *                                 void *user_data)
 *          {
 *              const char *event_type = NULL;
 *              int err = mv_surveillance_get_event_trigger_type(trigger,
 *                                                               event_type);
 *              if (MEDIA_VISION_ERROR_NONE != err) return;
 *
 *              if (0 == strncmp(event_type,
 *                               MV_SURVEILLANCE_EVENT_TYPE_PERSON_APPEARED_DISAPPEARED,
 *                               255))
 *              {
 *                  size_t tracked_person_num = 0;
 *                  err = mv_surveillance_get_result_value(
 *                            event_result,
 *                            MV_SURVEILLANCE_PERSONS_TRACKED_NUMBER,
 *                            &tracked_person_num);
 *
 *                  if (MEDIA_VISION_ERROR_NONE != err || 0 == tracked_person_num)
 *                      return;
 *
 *                  mv_rectangle_s *track_locations =
 *                            (mv_rectangle_s*)
 *                            malloc(sizeof(mv_rectangle_s) * tracked_person_num);
 *                  err = mv_surveillance_get_result_value(
 *                            event_result,
 *                            MV_SURVEILLANCE_PERSONS_TRACKED_LOCATIONS,
 *                            track_locations);
 *
 *                  // Do something with locations where persons were tracked...
 *
 *                  free (track_locations);
 *              }
 *          }
 *          @endcode
 *
 * @since_tizen 3.0
 */
#define MV_SURVEILLANCE_PERSONS_TRACKED_LOCATIONS \
                "TRACKED_PERSONS_LOCATIONS"

/**
 * @brief Name of the person recognition event type.
 *
 * @details This is common event for a person recognizing. The result will be
 *          a number of persons, which were recognized, their positions (face
 *          locations), labels and confidences of the recognition models (see
 *          documentation for @ref mv_face_recognize() in
 *          @ref CAPI_MEDIA_VISION_FACE_MODULE). When one subscribes to this
 *          event, the engine configuration must be filled by path to the saved
 *          face recognition model. These path should be set using
 *          @ref mv_engine_config_set_string_attribute() as attribute named
 *          @ref MV_SURVEILLANCE_FACE_RECOGNITION_MODEL_FILE_PATH.
 *          See documentation for face recognition in
 *          @ref CAPI_MEDIA_VISION_FACE_MODULE and for engine configuration in
 *          @ref CAPI_MEDIA_VISION_COMMON_MODULE for details. Out parameters
 *          (can be accessed in the @ref mv_surveillance_event_occurred_cb
 *          callback using @ref mv_surveillance_get_result_value() function):
 *          * @ref MV_SURVEILLANCE_PERSONS_RECOGNIZED_NUMBER - the number
 *            of persons which were recognized;\n
 *          * @ref MV_SURVEILLANCE_PERSONS_RECOGNIZED_LOCATIONS - the locations
 *            of persons which were recognized;\n
 *          * @ref MV_SURVEILLANCE_PERSONS_RECOGNIZED_LABELS - the labels of
 *            persons which were recognized;\n
 *          * @ref MV_SURVEILLANCE_PERSONS_RECOGNIZED_CONFIDENCES -
 *            the confidences values that persons were recognized correctly.
 *
 * @since_tizen 3.0
 */
#define MV_SURVEILLANCE_EVENT_TYPE_PERSON_RECOGNIZED \
                "MV_SURVEILLANCE_EVENT_PERSON_RECOGNIZED"

/**
 * @brief Name of the event result value that contains number of locations where
 *        faces were recognized.
 * @details This event result value can be accessed after event triggers of
 *          @ref MV_SURVEILLANCE_EVENT_TYPE_PERSON_RECOGNIZED type activation.
 *          Result value is of @c size_t type, so has to be casted as in
 *          the following example:
 *          @code{.c}
 *          void event_occurred_cb(mv_surveillance_event_trigger_h trigger,
 *                                 mv_source_h source,
 *                                 int video_stream_id,
 *                                 mv_surveillance_result_h event_result,
 *                                 void *user_data)
 *          {
 *              const char *event_type = NULL;
 *              int err = mv_surveillance_get_event_trigger_type(trigger,
 *                                                               event_type);
 *              if (MEDIA_VISION_ERROR_NONE != err) return;
 *
 *              if (0 == strncmp(event_type,
 *                               MV_SURVEILLANCE_EVENT_TYPE_PERSON_RECOGNIZED,
 *                               255))
 *              {
 *                  size_t rec_person_num = 0;
 *                  err = mv_surveillance_get_result_value(
 *                            event_result,
 *                            MV_SURVEILLANCE_PERSONS_RECOGNIZED_NUMBER,
 *                            &rec_person_num);
 *                  if (MEDIA_VISION_ERROR_NONE != err)
 *                      return;
 *
 *                  // Do something with number of recognized persons...
 *              }
 *          }
 *          @endcode
 *
 * @since_tizen 3.0
 */
#define MV_SURVEILLANCE_PERSONS_RECOGNIZED_NUMBER \
                "NUMBER_OF_PERSONS"

/**
 * @brief Name of the event result value that contains a set of rectangular
 *        locations where person faces were recognized.
 * @details This event result value can be accessed after event triggers of
 *          @ref MV_SURVEILLANCE_EVENT_TYPE_PERSON_RECOGNIZED type activation.
 *          Result value is of @a mv_rectangle_s array type, so has to be casted
 *          as in the following example:
 *          @code{.c}
 *          void event_occurred_cb(mv_surveillance_event_trigger_h trigger,
 *                                 mv_source_h source,
 *                                 int video_stream_id,
 *                                 mv_surveillance_result_h event_result,
 *                                 void *user_data)
 *          {
 *              const char *event_type = NULL;
 *              int err = mv_surveillance_get_event_trigger_type(trigger,
 *                                                               event_type);
 *              if (MEDIA_VISION_ERROR_NONE != err) return;
 *
 *              if (0 == strncmp(event_type,
 *                               MV_SURVEILLANCE_EVENT_TYPE_PERSON_RECOGNIZED,
 *                               255))
 *              {
 *                  size_t rec_person_num = 0;
 *                  err = mv_surveillance_get_result_value(
 *                            event_result,
 *                            MV_SURVEILLANCE_PERSONS_RECOGNIZED_NUMBER,
 *                            &rec_person_num);
 *                  if (MEDIA_VISION_ERROR_NONE != err || 0 == rec_person_num)
 *                      return;
 *
 *                  mv_rectangle_s *locations =
 *                            (mv_rectangle_s*)
 *                            malloc(sizeof(mv_rectangle_s) * rec_person_num);
 *                  err = mv_surveillance_get_result_value(
 *                            event_result,
 *                            MV_SURVEILLANCE_PERSONS_RECOGNIZED_LOCATIONS,
 *                            locations);
 *
 *                  // Do something with locations...
 *
 *                  free (locations);
 *              }
 *          }
 *          @endcode
 *
 * @since_tizen 3.0
 */
#define MV_SURVEILLANCE_PERSONS_RECOGNIZED_LOCATIONS \
                "PERSONS_LOCATIONS"

/**
 * @brief Name of the event result value that contains a set of labels that
 *        correspond to the recognized persons.
 * @details This event result value can be accessed after event triggers of
 *          @ref MV_SURVEILLANCE_EVENT_TYPE_PERSON_RECOGNIZED type activation.
 *          Result value is of integers array type, so has to be casted
 *          as in the following example:
 *          @code{.c}
 *          void event_occurred_cb(mv_surveillance_event_trigger_h trigger,
 *                                 mv_source_h source,
 *                                 int video_stream_id,
 *                                 mv_surveillance_result_h event_result,
 *                                 void *user_data)
 *          {
 *              const char *event_type = NULL;
 *              int err = mv_surveillance_get_event_trigger_type(trigger,
 *                                                               event_type);
 *              if (MEDIA_VISION_ERROR_NONE != err) return;
 *
 *              if (0 == strncmp(event_type,
 *                               MV_SURVEILLANCE_EVENT_TYPE_PERSON_RECOGNIZED,
 *                               255))
 *              {
 *                  size_t rec_person_num = 0;
 *                  err = mv_surveillance_get_result_value(
 *                            event_result,
 *                            MV_SURVEILLANCE_PERSONS_RECOGNIZED_NUMBER,
 *                            &rec_person_num);
 *                  if (MEDIA_VISION_ERROR_NONE != err || 0 == rec_person_num)
 *                      return;
 *
 *                  int *labels =
 *                            (int*)malloc(sizeof(int) * rec_person_num);
 *                  err = mv_surveillance_get_result_value(
 *                            event_result,
 *                            MV_SURVEILLANCE_PERSONS_RECOGNIZED_LABELS,
 *                            labels);
 *
 *                  // Do something with labels...
 *
 *                  free (labels);
 *              }
 *          }
 *          @endcode
 *
 * @since_tizen 3.0
 */
#define MV_SURVEILLANCE_PERSONS_RECOGNIZED_LABELS \
                "PERSONS_LABELS"

/**
 * @brief Name of the event result value that contains a set of confidence
 *        values that correspond to the recognized persons.
 * @details This event result value can be accessed after event triggers of
 *          @ref MV_SURVEILLANCE_EVENT_TYPE_PERSON_RECOGNIZED type activation.
 *          Result value is of doubles array type, so has to be casted
 *          as in the following example:
 *          @code{.c}
 *          void event_occurred_cb(mv_surveillance_event_trigger_h trigger,
 *                                 mv_source_h source,
 *                                 int video_stream_id,
 *                                 mv_surveillance_result_h event_result,
 *                                 void *user_data)
 *          {
 *              const char *event_type = NULL;
 *              int err = mv_surveillance_get_event_trigger_type(trigger,
 *                                                               event_type);
 *              if (MEDIA_VISION_ERROR_NONE != err) return;
 *
 *              if (0 == strncmp(event_type,
 *                               MV_SURVEILLANCE_EVENT_TYPE_PERSON_RECOGNIZED,
 *                               255))
 *              {
 *                  size_t rec_person_num = 0;
 *                  err = mv_surveillance_get_result_value(
 *                            event_result,
 *                            MV_SURVEILLANCE_PERSONS_RECOGNIZED_NUMBER,
 *                            &rec_person_num);
 *                  if (MEDIA_VISION_ERROR_NONE != err || 0 == rec_person_num)
 *                      return;
 *
 *                  double *confidences =
 *                            (double*)malloc(sizeof(double) * rec_person_num);
 *                  err = mv_surveillance_get_result_value(
 *                            event_result,
 *                            MV_SURVEILLANCE_PERSONS_RECOGNIZED_CONFIDENCES,
 *                            confidences);
 *
 *                  // Do something with confidences...
 *
 *                  free (confidences);
 *              }
 *          }
 *          @endcode
 *
 * @since_tizen 3.0
 */
#define MV_SURVEILLANCE_PERSONS_RECOGNIZED_CONFIDENCES \
                "PERSONS_CONFIDENCES"

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_VISION_SURVEILLANCE_MODULE
 * @{
 */

/**
 * @brief Defines MV_SURVEILLANCE_FACE_RECOGNITION_MODEL_FILE_PATH to set face
 *        recognition model file path. It is an attribute of the engine
 *        configuration.
 * @details This value HAS TO BE set in engine configuration before subscription
 *          on @ref MV_SURVEILLANCE_EVENT_TYPE_PERSON_RECOGNIZED event trigger
 *
 * @since_tizen 3.0
 * @see mv_engine_config_set_string_attribute()
 * @see mv_engine_config_get_string_attribute()
 */
#define MV_SURVEILLANCE_FACE_RECOGNITION_MODEL_FILE_PATH \
                "MV_SURVEILLANCE_FACE_RECOGNITION_MODEL_FILE_PATH"

/**
 * @brief Defines MV_SURVEILLANCE_MOVEMENT_DETECTION_THRESOLD to set movement
 *        detection threshold. It is an attribute of the engine configuration.
 * @details This value might be set in engine configuration before subscription
 *          on @ref MV_SURVEILLANCE_EVENT_TYPE_MOVEMENT_DETECTED event trigger
 *          to specify sensitivity of the movement detector. This value has to
 *          be integer in 0..255 range where 255 means that no movements will
 *          be detected, and 0 means that all frame changes will be interpreted
 *          as movements. Default value is 10.
 *
 * @since_tizen 3.0
 * @see mv_engine_config_set_int_attribute()
 * @see mv_engine_config_get_int_attribute()
 */
#define MV_SURVEILLANCE_MOVEMENT_DETECTION_THRESHOLD \
                "MV_SURVEILLANCE_MOVEMENT_DETECTION_THRESHOLD"

/**
 * @brief Defines MV_SURVEILLANCE_SKIP_FRAMES_COUNT to set how many frames
 *        will be skipped during push source. It is an attribute of the engine
 *        configuration.
 * @details This integer value might be set in engine configuration to specify
 *          number of @ref mv_surveillance_push_source() function calls will be
 *          ignored by subscription of the event trigger. Default value is 0.
 *          It means that no frames will be skipped and all
 *          @ref mv_surveillance_push_source() function calls will be processed.
 *
 * @since_tizen 3.0
 * @see mv_engine_config_set_int_attribute()
 * @see mv_engine_config_get_int_attribute()
 */
#define MV_SURVEILLANCE_SKIP_FRAMES_COUNT "MV_SURVEILLANCE_SKIP_FRAMES_COUNT"

/**
 * @brief The handle to event trigger.
 *
 * @since_tizen 3.0
 * @remarks See supported event types and their descriptions in
 *          @ref CAPI_MEDIA_VISION_SURVEILLANCE_EVENT_TYPES documentation
 *          section.
 *          Also the list of supported events can be obtained using
 *          @ref mv_surveillance_foreach_supported_event_type() function
 */
typedef void *mv_surveillance_event_trigger_h;

/**
 * @brief The handle to event trigger activation result.
 * @details Result is a handle to the output values which are specific for each event.
 *          See the output values names in the event types descriptions located
 *          in @ref CAPI_MEDIA_VISION_SURVEILLANCE_EVENT_TYPES documentation
 *          section.
 *          Result values can be gotten by @ref mv_surveillance_get_result_value()
 *          function one by one in order specified in the event description (
 *          the same order of event value names is supported by
 *          @ref mv_surveillance_foreach_event_result_name() function).
 *          This pointer will be destroyed when
 *          @ref mv_surveillance_event_occurred_cb passed.
 *
 * @since_tizen 3.0
 */
typedef void *mv_surveillance_result_h;

/**
 * @brief Called when event trigger activation has been detected.
 *
 * @since_tizen 3.0
 * @remarks Handle @a event_result is valid only inside callback
 * @param [in] trigger            The event trigger handle
 * @param [in] source             The handle to the media source
 * @param [in] video_stream_id    The identifier of the video source where event
 *                                has been detected
 * @param [in] event_result       The event result passed from the
 *                                @ref mv_surveillance_subscribe_event_trigger()
 * @param [in] user_data          The user data passed from the
 *                                @ref mv_surveillance_subscribe_event_trigger()
 *                                function
 *
 * @pre Callback can be invoked only after
 *      @ref mv_surveillance_subscribe_event_trigger()
 *      was called for particular event trigger.
 *
 * @see mv_surveillance_subscribe_event_trigger()
 * @see mv_surveillance_unsubscribe_event_trigger()
 */
typedef void (*mv_surveillance_event_occurred_cb)(
        mv_surveillance_event_trigger_h trigger,
        mv_source_h source,
        int video_stream_id,
        mv_surveillance_result_h event_result,
        void *user_data);

/**
 * @brief Called to get the information once for each supported event type.
 *
 * @since_tizen 3.0
 * @remarks Don't release memory of @a event_type
 * @param [in] event_type    Character string containing name of the event type
 * @param [in] user_data     The user data passed from the
 *                           @ref mv_surveillance_foreach_supported_event_type()
 *                           function
 * @return @c true to continue with the next iteration of the loop, \n
 *         otherwise @c false to break out of the loop
 *
 * @pre mv_surveillance_foreach_supported_event_type() will invoke this callback
 * @see mv_surveillance_foreach_supported_event_type()
 */
typedef bool (*mv_surveillance_event_type_cb)(
        const char *event_type,
        void *user_data);

/**
 * @brief Called to get the result name from the triggered event.
 *
 * @since_tizen 3.0
 * @remarks Don't release memory of @a value_name
 * @param [in] name         Character string containing the name of value that
 *                          can be obtained from @ref mv_surveillance_result_h
 *                          handle by @ref mv_surveillance_get_result_value()
 *                          function
 * @param [in] user_data    The user data passed from the
 *                          @ref mv_surveillance_foreach_event_result_name()
 *                          function
 * @return @c true to continue with the next iteration of the loop, \n
 *         otherwise @c false to break out of the loop
 *
 * @pre mv_surveillance_foreach_event_result_name() will invoke this
 *      callback
 * @see mv_surveillance_foreach_event_result_name()
 */
typedef bool (*mv_surveillance_event_result_name_cb)(
        const char *name,
        void *user_data);

/**
 * @brief Creates surveillance event trigger handle.
 *
 * @since_tizen 3.0
 * @remarks List of supported event types can be obtained by
 *          @ref mv_surveillance_foreach_supported_event_type function
 * @remarks You must release @a trigger by using
 *          @ref mv_surveillance_event_trigger_destroy()
 * @param [in]  event_type    Name of the event type to be supported by the
 *                            @a trigger
 * @param [out] trigger       A new handle to the event trigger
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @see mv_surveillance_event_trigger_destroy()
 * @see mv_surveillance_foreach_supported_event_type()
 */
EXPORT_API int mv_surveillance_event_trigger_create(
        const char *event_type,
        mv_surveillance_event_trigger_h *trigger);

/**
 * @brief Destroys the surveillance event trigger handle and releases all its
 *        resources.
 *
 * @since_tizen 3.0
 * @param [in] trigger    The handle to the event trigger to be destroyed
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @see mv_surveillance_event_trigger_create
 */
EXPORT_API int mv_surveillance_event_trigger_destroy(
        mv_surveillance_event_trigger_h trigger);

/**
 * @brief Gets the surveillance event trigger type as character string.
 *
 * @since_tizen 3.0
 * @remarks The @a event_type should be freed using free()
 * @param [in]  trigger       The handle to the event trigger
 * @param [out] event_type    The pointer to the character string which will be
 *                            filled by textual name of the event type
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Event trigger has to be created by
 *      @ref mv_surveillance_event_trigger_create() function
 */
EXPORT_API int mv_surveillance_get_event_trigger_type(
        mv_surveillance_event_trigger_h trigger,
        char **event_type);

/**
 * @brief Sets ROI (Region Of Interest) to the event trigger.
 * @details When ROI is set for the event trigger, then event check for this
 *          @a triger will be performed only inside the polygonal region
 *          determined by @a roi parameter.
 *          If this method has been never called for the @a trigger, then event
 *          will be checked for the whole input frame (event check is performed
 *          for each @ref mv_surveillance_push_source() function call).
 *          It is possible to change the ROI between
 *          @ref mv_surveillance_push_source() calls.
 *
 * @since_tizen 3.0
 * @param [in] trigger             The handle to the event trigger
 * @param [in] number_of_points    The number of ROI points
 * @param [in] roi                 The input array with ROI points
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Event trigger has to be created by
 *      @ref mv_surveillance_event_trigger_create() function
 *
 * @see mv_surveillance_event_trigger_h
 * @see mv_surveillance_get_event_trigger_roi()
 */
EXPORT_API int mv_surveillance_set_event_trigger_roi(
        mv_surveillance_event_trigger_h trigger,
        int number_of_points,
        mv_point_s *roi);

/**
 * @brief Gets ROI (Region Of Interest) from the event trigger.
 *
 * @since_tizen 3.0
 * @remark If @ref mv_surveillance_set_event_trigger_roi() has been never
 *         called for @a trigger, then @a number_of_points output value will be
 *         zero and @a roi pointer will be not changed.
 * @param [in]  trigger             The handle to the event trigger
 * @param [out] number_of_points    The number of ROI points
 * @param [out] roi                 The output array with ROI points
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Event trigger has to be created by
 *      @ref mv_surveillance_event_trigger_create() function
 *
 * @post Memory for @a roi array must be released
 *
 * @see mv_surveillance_event_trigger_h
 * @see mv_surveillance_set_event_trigger_roi()
 */
EXPORT_API int mv_surveillance_get_event_trigger_roi(
        mv_surveillance_event_trigger_h trigger,
        int *number_of_points,
        mv_point_s **roi);

/**
 * @brief Subscribes @a trigger to process sources pushed from video identified
 *        by @a video_stream_id.
 * @details When @a trigger is subscribed, then each time when function
 *          @ref mv_surveillance_push_source() is called for @a video_stream_id,
 *          event occurrence is checked. If this check is successful,
 *          @a callback is invoked. Details on occurred event can be obtained
 *          using @ref mv_surveillance_result_h handle from @a callback.
 *
 * @since_tizen 3.0
 * @remarks Use @ref mv_surveillance_unsubscribe_event_trigger() function for
 *          the same @a trigger and @a video_stream_id parameters to stop
 *          subscription.
 * @param [in] trigger            The handle to the event trigger activating
 *                                calls of the @a callback function
 * @param [in] video_stream_id    The identifier of the video stream for which
 *                                event trigger activation will be checked
 * @param [in] engine_cfg         The engine configuration of the event
 * @param [in] callback           Callback to be called each time when event
 *                                occurrence is detected
 * @param [in] user_data          The user data to be passed to the @a callback
 *                                function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @post @ref mv_surveillance_event_occurred_cb will be called each time
 *       @a trigger is activated after @ref mv_surveillance_push_source() call
 *
 * @see mv_surveillance_event_trigger_h
 * @see mv_surveillance_unsubscribe_event_trigger()
 * @see mv_surveillance_push_source()
 */
EXPORT_API int mv_surveillance_subscribe_event_trigger(
        mv_surveillance_event_trigger_h trigger,
        int video_stream_id,
        mv_engine_config_h engine_cfg,
        mv_surveillance_event_occurred_cb callback,
        void *user_data);

/**
 * @brief Unsubscribes @a trigger from the event and stop calling @a callback.
 *
 * @since_tizen 3.0
 * @remarks To start handling trigger activation use
            @ref mv_surveillance_subscribe_event_trigger().
 * @param [in] trigger            The handle to the event trigger for which
 *                                subscription will be stopped
 * @param [in] video_stream_id    The identifier of the video source for which
 *                                subscription will be stopped
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre To stop subscription it has to be created earlier with
 *      @ref mv_surveillance_subscribe_event_trigger() function
 *
 * @see mv_surveillance_event_trigger_h
 * @see mv_surveillance_subscribe_event_trigger()
 */
EXPORT_API int mv_surveillance_unsubscribe_event_trigger(
        mv_surveillance_event_trigger_h trigger,
        int video_stream_id);

/**
 * @brief Pushes source to the surveillance system to detect events.
 * @details mv_surveillance_event_occurred_cb() will be called when any
 *          subscribing event detected.
 *
 * @since_tizen 3.0
 * @remarks @ref mv_surveillance_set_event_trigger_roi() function can be used
 *          to specify the polygon region where event can be detected only
 * @param [in] source             The handle to the media source
 * @param [in] video_stream_id    The identifier of video stream from which
 *                                @a source is coming
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre To receive surveillance results, some event triggers has to be
 *      subscribed by @ref mv_surveillance_subscribe_event_trigger() function
 *      before @ref mv_surveillance_push_source() calls
 * @pre Before calling of this method @a source has to be correctly filled.
 *      @ref mv_source_fill_by_media_packet(), @ref mv_source_fill_by_buffer()
 *      functions can be used to fill @a source
 *
 * @see mv_surveillance_event_trigger_h
 * @see mv_surveillance_event_occurred_cb
 * @see mv_surveillance_subscribe_event_trigger()
 * @see mv_surveillance_unsubscribe_event_trigger()
 */
EXPORT_API int mv_surveillance_push_source(
        mv_source_h source,
        int video_stream_id);

/**
 * @brief Starts traversing through list of supported event types.
 *
 * @since_tizen 3.0
 * @remarks Supported event types and their descriptions can be found in
 *          @ref CAPI_MEDIA_VISION_SURVEILLANCE_EVENT_TYPES documentation
 *          section
 * @param [in] callback     The callback function to be called for each
 *                          supported event type
 * @param [in] user_data    The user data to be passed to the @a callback
 *                          function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @see mv_surveillance_event_type_cb
 * @see mv_surveillance_foreach_event_result_name()
 */
EXPORT_API int mv_surveillance_foreach_supported_event_type(
        mv_surveillance_event_type_cb callback,
        void *user_data);

/**
 * @brief Starts traversing through list of supported event result value names.
 *
 * @since_tizen 3.0
 * @remarks Supported event types, event result value names and their
 *          descriptions can be found in
 *          @ref CAPI_MEDIA_VISION_SURVEILLANCE_EVENT_TYPES documentation
 *          section
 * @param [in] event_type    The name of the event type for which result value
 *                           names will be passed to the @a callback. Can be
 *                           set @c NULL. If set @c NULL then all supported
 *                           event result value names will be traversed
 * @param [in] callback      The callback function to be called for each
 *                           supported event result value name
 * @param [in] user_data     The user data to be passed to the @a callback
 *                           function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @see mv_surveillance_event_result_name_cb
 * @see mv_surveillance_foreach_supported_event_type()
 * @see mv_surveillance_get_result_value()
 */
EXPORT_API int mv_surveillance_foreach_event_result_name(
        const char *event_type,
        mv_surveillance_event_result_name_cb callback,
        void *user_data);

/**
 * @brief Gets result value.
 * @details See the output values names in the event types descriptions located
 *          in @ref CAPI_MEDIA_VISION_SURVEILLANCE_EVENT_TYPES documentation
 *          section.
 *
 * @since_tizen 3.0
 * @remarks The name can be obtained by
 *          @ref mv_surveillance_foreach_event_result_name function
 * @param [in] result        The handle to the event result
 * @param [in] name          The name of the value to be gotten
 * @param [in, out] value    The pointer to variable which will be filled
 *                           by result value. To find the type of @a value
 *                           please refer to the
 *                           @ref CAPI_MEDIA_VISION_SURVEILLANCE_EVENT_TYPES
 *                           documentation section
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED Not supported
 *
 * @pre Memory for value has to be allocated
 *
 * @see mv_surveillance_event_trigger_h
 * @see mv_surveillance_event_occurred_cb
 * @see mv_surveillance_subscribe_event_trigger()
 * @see mv_surveillance_unsubscribe_event_trigger()
 * @see mv_surveillance_foreach_supported_event_type()
 * @see mv_surveillance_foreach_event_result_name()
 */
EXPORT_API int mv_surveillance_get_result_value(
        mv_surveillance_result_h result,
        const char *name,
        void *value);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TIZEN_MEDIAVISION_SURVEILLANCE_H__ */
