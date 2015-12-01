/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __TIZEN_MEDIAVISION_DOC_H__
#define __TIZEN_MEDIAVISION_DOC_H__

/**
 * @ingroup CAPI_MEDIA_FRAMEWORK
 * @defgroup    CAPI_MEDIA_VISION_MODULE Media Vision
 * @brief Media Vision library providing following functionality:\n
 * * Face detection, recognition, and tracking;\n
 * * Barcode detection and generation;\n
 * * Flat Image detection, recognition and tracking;\n
 * * Flat Image features extraction;\n
 * * Surveillance: movement detection, person appearance/disappearance,
 *   person recognition.
 *
 * @defgroup    CAPI_MEDIA_VISION_COMMON_MODULE Media Vision Common
 * @ingroup     CAPI_MEDIA_VISION_MODULE
 * @brief  Common functions and enumerations used in
 *         @ref CAPI_MEDIA_VISION_FACE_MODULE,
 *         @ref CAPI_MEDIA_VISION_IMAGE_MODULE,
 *         @ref CAPI_MEDIA_VISION_SURVEILLANCE_MODULE and
 *         @ref CAPI_MEDIA_VISION_BARCODE_MODULE submodules.
 * @section CAPI_MEDIA_VISION_COMMON_MODULE_HEADER Required Header
 *      \#include <mv_common.h>
 *
 * @section CAPI_MEDIA_VISION_COMMON_MODULE_OVERVIEW Overview
 * @ref CAPI_MEDIA_VISION_COMMON_MODULE provides a set of functions that
 * are used in Media Vision API to correctly prepare and use included modules.
 * @ref mv_source_h is the handler that has to be created to keep information
 * on image or video frame data as row buffer. It can be created based on
 * the media data stored in memory or using the @ref media_packet_h handler.
 * @ref mv_source_h supported by the set of getters which allow to retrieve
 * such image parameters as its size or colorspace (see @ref mv_colorspace_e
 * enumeration). The handler is usually used as parameter for functions
 * performing computer vision tasks on the image data.\n
 * @ref mv_engine_config_h is the handler which provides dictionary
 * functionality. It means that it is possible to set (key, value) pairs to
 * the @ref mv_engine_config_h handlers and use them to transfer these values
 * to the engine part underlying Media Vision API. Information on which
 * attributes can be set is provided together with particular engines.
 *
 * @section CAPI_MEDIA_VISION_COMMON_MODULE_FEATURE Related Features
 * This API is related with the following features:\n
 *  - http://tizen.org/feature/vision.barcode_detection\n
 *  - http://tizen.org/feature/vision.barcode_generation\n
 *  - http://tizen.org/feature/vision.face_recognition\n
 *  - http://tizen.org/feature/vision.image_recognition\n
 *
 * It is recommended to design feature related codes in your application for
 * reliability.\n
 * You can check if a device supports the related features for this API by using
 * @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of
 * your application.\n
 * To ensure your application is only running on the device with specific
 * features, please define the features in your manifest file using the manifest
 * editor in the SDK.\n
 * More details on featuring your application can be found from
 * <a href="../org.tizen.mobile.native.appprogramming/html/ide_sdk_tools/feature_element.htm">
 *    <b>Feature Element</b>.
 * </a>
 *
 * @defgroup    CAPI_MEDIA_VISION_FACE_MODULE Media Vision Face
 * @ingroup     CAPI_MEDIA_VISION_MODULE
 * @brief Face detection, recognition, and tracking.
 * @section CAPI_MEDIA_VISION_FACE_MODULE_HEADER Required Header
 *      \#include <mv_face.h>
 *
 * @section CAPI_MEDIA_VISION_FACE_MODULE_OVERVIEW Overview
 *
 * @section CAPI_MEDIA_VISION_FACE_MODULE_FEATURE Related Features
 * This API is related with the following features:\n
 *  - http://tizen.org/feature/vision.face_recognition\n
 *
 * It is recommended to design feature related codes in your application for
 * reliability.\n
 * You can check if a device supports the related features for this API by using
 * @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of
 * your application.\n
 * To ensure your application is only running on the device with specific
 * features, please define the features in your manifest file using the manifest
 * editor in the SDK.\n
 * More details on featuring your application can be found from
 * <a href="../org.tizen.mobile.native.appprogramming/html/ide_sdk_tools/feature_element.htm">
 *   <b>Feature Element</b>.
 * </a>
 *
 * @ref CAPI_MEDIA_VISION_FACE_MODULE contains @ref mv_face_detect() function
 * to detect faces on @ref mv_source_h, and @ref mv_face_detected_cb callback
 * to process detected faces. Also it contains @ref mv_face_recognize() function
 * which performs face recognition on @ref mv_source_h for recognition model
 * @ref mv_face_recognition_model_h handle. Results of the recognition will be
 * passed to the @ref mv_face_recognized_cb. Results of the recognition consist
 * of unique face label and confidence of the recognition model that face was
 * recognized correctly. Unique face label is the integer identifier should be
 * introduced to the model before starting recognition, when learning examples
 * are added (see @ref mv_face_recognition_model_add() function
 * documentation for details about unique face labels).\n
 * Recognition model should be created with
 * @ref mv_face_recognition_model_create() and destroyed with
 * @ref mv_face_recognition_model_destroy(). Model can be cloned with
 * @ref mv_face_recognition_model_clone(), saved to the file with
 * @ref mv_face_recognition_model_save(), loaded with
 * @ref mv_face_recognition_model_load(). Model learning can be provided with
 * @ref mv_face_recognition_model_add(), and
 * @ref mv_face_recognition_model_learn() functions. These two methods has to be
 * called in the direct order: first, labeled face examples should be added to
 * the model using @ref mv_face_recognition_model_add(). It is
 * expected that images of the same face will be added specifying the same face
 * label. When examples were added, model has to be learned based on the collected
 * set of labeled face images. @ref mv_face_recognition_model_learn() function
 * will perform learning. If it is required to get the list of unique face
 * labels learned by the model, @ref mv_face_recognition_model_query_labels()
 * function can be used.\n
 * Module contains function
 * @ref mv_face_track() which performs tracking on @ref mv_source_h for
 * @ref mv_face_tracking_model_h and  @ref mv_face_on_tracked_cb which process
 * tracked face. Tracking model should be created with
 * @ref mv_face_tracking_model_create() and destroyed with
 * @ref mv_face_tracking_model_destroy(). Tracking model should be prepared
 * with @ref mv_face_tracking_model_prepare() before each session of tracking.
 * Model can be cloned with @ref mv_face_tracking_model_clone(), saved to the
 * file with @ref mv_face_tracking_model_save(), loaded with
 * @ref mv_face_tracking_model_load().\n
 * Module provides function for detecting eye-blink status -
 * @ref mv_face_eye_condition_recognize(), which provides detection on
 * @ref mv_source_h and face location - @ref mv_rectangle_s. Callback
 * @ref mv_face_eye_condition_recognized_cb handles result.\n
 * Also module contains @ref mv_face_facial_expression_recognize() function which
 * detects face expression on @ref mv_source_h, face location is determined by
 * @ref mv_rectangle_s. Result is handled with
 * @ref mv_face_facial_expression_recognized_cb.
 *
 * @defgroup    CAPI_MEDIA_VISION_IMAGE_MODULE Media Vision Image
 * @ingroup     CAPI_MEDIA_VISION_MODULE
 * @brief Flat Image recognition and tracking;\n
 * Flat image features extraction.
 * @section CCAPI_MEDIA_VISION_IMAGE_MODULE_HEADER Required Header
 *      \#include <mv_image.h>
 *
 * @section CAPI_MEDIA_VISION_IMAGE_MODULE_OVERVIEW Overview
 *
 * @section CAPI_MEDIA_VISION_IMAGE_MODULE_FEATURE Related Features
 * This API is related with the following features:\n
 *  - http://tizen.org/feature/vision.image_recognition\n
 *
 * It is recommended to design feature related codes in your application for
 * reliability.\n
 * You can check if a device supports the related features for this API by using
 * @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of
 * your application.\n
 * To ensure your application is only running on the device with specific
 * features, please define the features in your manifest file using the manifest
 * editor in the SDK.\n
 * More details on featuring your application can be found from
 * <a href="../org.tizen.mobile.native.appprogramming/html/ide_sdk_tools/feature_element.htm">
 *   <b>Feature Element</b>.
 * </a>
 *
 * @ref CAPI_MEDIA_VISION_IMAGE_MODULE contains @ref mv_image_recognize()
 * function to recognize images on @ref mv_source_h, and @ref
 * mv_image_recognized_cb callback to process recognition result. Module
 * also contains @ref mv_image_track() which performs tracking of image on
 * sequence of @ref mv_source_h handles (by using method for each @ref
 * mv_source_h) and callback @ref mv_image_tracked_cb to process tracking result.
 * In order to perform general functionality, module contains @ref
 * mv_image_object_h and @ref mv_image_tracking_model_h handles.
 * Image object should be created with @ref mv_image_object_create() and
 * destroyed with @ref mv_image_object_destroy(). @ref mv_image_object_h can be
 * constructed by calling @ref mv_image_object_fill() using image wrapped with
 * @ref mv_source_h. Object can be cloned with @ref mv_image_object_clone(),
 * saved to the file with @ref mv_image_object_save() and loaded from file with
 * @ref mv_image_object_load(). Image object can be evaluated with
 * @ref mv_image_object_get_recognition_rate().
 * Tracking model should be created with @ref mv_image_tracking_model_create()
 * and destroyed with @ref mv_image_tracking_model_destroy(). Tracking model
 * should be based on image object which will be tracked. Use
 * @ref mv_image_tracking_model_set_target() to assign @ref mv_image_object_h
 * to the corresponding @ref mv_image_tracking_model_h. It can be cloned with
 * @ref mv_image_tracking_model_clone(), saved to the file with @ref
 * mv_image_tracking_model_save() and loaded from file with @ref
 * mv_image_tracking_model_load().
 *
 * @defgroup    CAPI_MEDIA_VISION_BARCODE_MODULE Media Vision BarCode
 * @ingroup     CAPI_MEDIA_VISION_MODULE
 * @brief  Barcode module consists of barcode detection and barcode generation
 *         submodules.
 * @section CAPI_MEDIA_VISION_BARCODE_MODULE_HEADER Required Header
 *      \#include <mv_barcode.h>
 *
 * @section CAPI_MEDIA_VISION_BARCODE_MODULE_OVERVIEW Overview
 *
 * @section CAPI_MEDIA_VISION_BARCODE_MODULE_FEATURE Related Features
 * This API is related with the following features:\n
 *  - http://tizen.org/feature/vision.barcode_detection\n
 *  - http://tizen.org/feature/vision.barcode_generation\n
 *
 * It is recommended to design feature related codes in your application for
 * reliability.\n
 * You can check if a device supports the related features for this API by using
 * @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of
 * your application.\n
 * To ensure your application is only running on the device with specific
 * features, please define the features in your manifest file using the manifest
 * editor in the SDK.\n
 * More details on featuring your application can be found from
 * <a href="../org.tizen.mobile.native.appprogramming/html/ide_sdk_tools/feature_element.htm">
 *   <b>Feature Element</b>.
 * </a>
 *
 * @ref CAPI_MEDIA_VISION_BARCODE_MODULE contains two submodules:\n
 * * Detection submodule,
 * * Generation submodule.\n
 *
 * Detection submodule can be used for detecting barcodes on image sources,
 * reading encoded messages, getting barcode types.\n
 * This module contains
 * @ref mv_barcode_detect() function and @ref mv_barcode_detected_cb
 * callback. @ref mv_barcode_detect() function can be called for the image
 * determined by @ref mv_source_h handler to detect barcodes. If barcodes
 * were detected, then @ref mv_barcode_detected_cb callback is called.
 * Each detected barcode type, message and location can be processed by
 * this callback. @ref mv_barcode_detect() supports ROI (rectangular region of
 * interest) specification to allow barcode detection only for the region on
 * the image.
 *
 * Generation submodule can be used for generating the barcodes and QR codes.
 * Different encoding types (see @ref mv_barcode_qr_mode_e), error correction
 * codes (see @ref mv_barcode_qr_ecc_e) and code versions are supported for
 * QRCodes.\n This submodule contains @ref mv_barcode_generate_source() function
 * to generate handler to the buffer with row image of barcode, and
 * @ref mv_barcode_generate_image() function to generate image file with
 * barcode. Both functions support the set of parameters which allow API user to
 * configure output barcode. QR codes as well as Barcodes are supported
 * (see @ref mv_barcode_type_e enumeration for full list of supported barcodes).
 * For QR codes it is possible to specify error correction code and encoding
 * mode (see @ref mv_barcode_qr_mode_e). Generation to file supports several
 * formats (see @ref mv_barcode_image_format_e).
 *
 * @defgroup    CAPI_MEDIA_VISION_SURVEILLANCE_MODULE Media Vision Surveillance
 * @ingroup     CAPI_MEDIA_VISION_MODULE
 * @brief Video surveillance module.
 * @section CAPI_MEDIA_VISION_SURVEILLANCE_MODULE_HEADER Required Header
 *      \#include <mv_surveillance.h>
 *
 * @section CAPI_MEDIA_VISION_SURVEILLANCE_MODULE_FEATURE Related Features
 * This API is related with the following features:\n
 *  - http://tizen.org/feature/vision.image_recognition\n
 *  - http://tizen.org/feature/vision.face_recognition\n
 *
 * It is recommended to design feature related codes in your application for
 * reliability.\n
 * You can check if a device supports the related features for this API by using
 * @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of
 * your application.\n
 * To ensure your application is only running on the device with specific
 * features, please define the features in your manifest file using the manifest
 * editor in the SDK.\n
 * More details on featuring your application can be found from
 * <a href="https://developer.tizen.org/development/tools/native-tools/manifest-text-editor#feature">
 *   <b>Feature Element</b>.
 * </a>
 *
 * @section CAPI_MEDIA_VISION_SURVEILLANCE_MODULE_OVERVIEW Overview
 * @ref CAPI_MEDIA_VISION_SURVEILLANCE_MODULE provides functionality can be
 * utilized for creation of video surveillance systems. The main idea underlying
 * surveillance is event subscription model. By default, supported event types
 * are described in @ref CAPI_MEDIA_VISION_SURVEILLANCE_EVENT_TYPES section.
 * @ref mv_surveillance_subscribe_event_trigger() function has to be used to
 * create subscription to the particular event trigger. Triggers are handled by
 * @ref mv_surveillance_event_trigger_h type. Such type handlers can be created
 * with @ref mv_surveillance_event_trigger_create() function and destroyed with
 * @ref mv_surveillance_event_trigger_destroy() function. Once event trigger
 * subscription is created, corresponding @ref mv_surveillance_event_occurred_cb
 * callback will be invoked each time when event is detected, i.e. trigger is
 * activated. @ref mv_surveillance_result_h event detection result handler will
 * be passed to the callback together with identifier of the video stream where
 * event was detected and @ref mv_source_h handler containing frame in which
 * detection was performed. It is possible to retrieve specific to event type
 * result values using @ref mv_surveillance_result_h handler. In the
 * @ref mv_surveillance_get_result_value() function documentation can be found
 * detailed description of result values retrieving. Following table contains
 * general events and corresponding event detection results can be obtained by
 * this approach:
 * <table>
 * <tr>
 *     <td><b>Event</b></td>
 *     <td><b>Event result values</b></td>
 * </tr>
 * <tr>
 *     <td>@ref MV_SURVEILLANCE_EVENT_TYPE_MOVEMENT_DETECTED</td>
 *     <td>
 *         @ref MV_SURVEILLANCE_MOVEMENT_NUMBER_OF_REGIONS;<br>
 *         @ref MV_SURVEILLANCE_MOVEMENT_REGIONS
 *     </td>
 * </tr>
 * <tr>
 *     <td>@ref MV_SURVEILLANCE_EVENT_TYPE_PERSON_APPEARED_DISAPPEARED</td>
 *     <td>
 *         @ref MV_SURVEILLANCE_PERSONS_APPEARED_NUMBER;<br>
 *         @ref MV_SURVEILLANCE_PERSONS_DISAPPEARED_NUMBER;<br>
 *         @ref MV_SURVEILLANCE_PERSONS_TRACKED_NUMBER;<br>
 *         @ref MV_SURVEILLANCE_PERSONS_APPEARED_LOCATIONS;<br>
 *         @ref MV_SURVEILLANCE_PERSONS_DISAPPEARED_LOCATIONS;<br>
 *         @ref MV_SURVEILLANCE_PERSONS_TRACKED_LOCATIONS
 *     </td>
 * </tr>
 * <tr>
 *     <td>@ref MV_SURVEILLANCE_EVENT_TYPE_PERSON_RECOGNIZED</td>
 *     <td>
 *         @ref MV_SURVEILLANCE_PERSONS_RECOGNIZED_NUMBER;<br>
 *         @ref MV_SURVEILLANCE_PERSONS_RECOGNIZED_LOCATIONS;<br>
 *         @ref MV_SURVEILLANCE_PERSONS_RECOGNIZED_LABELS;<br>
 *         @ref MV_SURVEILLANCE_PERSONS_RECOGNIZED_CONFIDENCES
 *     </td>
 * </tr>
 * </table>
 * Before subscription of the event trigger with
 * @ref mv_surveillance_subscribe_event_trigger() call it is possible to create
 * @ref mv_engine_config_h handle and configurate following attributes:
 * - @ref MV_SURVEILLANCE_SKIP_FRAMES_COUNT to setup number of frames will be
     ignored by event trigger;
 * - @ref MV_SURVEILLANCE_MOVEMENT_DETECTION_THRESHOLD to specify sensitivity of
 *   the @ref MV_SURVEILLANCE_EVENT_TYPE_MOVEMENT_DETECTED event detection;
 * - @ref MV_SURVEILLANCE_FACE_RECOGNITION_MODEL_FILE_PATH to specify the file
 *   where face recognition model to be used for recognition is stored.
 *
 * Created engine config has to be used as a parameter of
 * @ref mv_surveillance_subscribe_event_trigger() to apply the configuration. If
 * NULL will be passed instead of valid @ref mv_engine_config_h handle, then
 * default attribute values will be used for subsriptions.
 * To make surveillance system work with video sequences
 * @ref mv_surveillance_push_source() function has to
 * be used for each frame in the sequence in the correct order. Multiple video
 * sources can be supported by the system. To distinguish different video
 * sources unique stream identifier has to be assigned to each subscription.
 * Then, particular identifier can be passed as a parameter to the
 * @ref mv_surveillance_push_source() function. After pushing the source to the
 * surveillance system, it will notify all triggers which were subscribed to
 * process frames coming from video stream which source has been pushed.
 * If trigger(s) is(are) activated on the source, then corresponding callback(s)
 * of @ref mv_surveillance_event_occurred_cb type will be called.
 * Additionally, region where event detection will be performed by the triggers
 * can be set with @ref mv_surveillance_set_event_trigger_roi() function and
 * gotten with @ref mv_surveillance_get_event_trigger_roi(). ROI is specified
 * independently for the each event trigger, so it is possible to detect events
 * of different types in the different parts of the incoming frames.
 * Event trigger subscription can be stopped any time using
 * @ref mv_surveillance_unsubscribe_event_trigger() function. Additionally,
 * @ref mv_surveillance_foreach_supported_event_type() and
 * @ref mv_surveillance_foreach_event_result_name() functions can be found
 * useful if it is required to obtain supported event types list or result
 * value names list dynamically.
 *
 * @defgroup    CAPI_MEDIA_VISION_SURVEILLANCE_EVENT_TYPES Media Vision Surveillance Event Types
 * @ingroup     CAPI_MEDIA_VISION_SURVEILLANCE_MODULE
 * @brief Event types supported by the Surveillance module.
 */

#endif  /* __TIZEN_MEDIAVISION_DOC_H__ */
