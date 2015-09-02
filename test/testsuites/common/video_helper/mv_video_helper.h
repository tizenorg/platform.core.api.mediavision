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

#ifndef __MV_VIDEO_HELPER_H__
#define __MV_VIDEO_HELPER_H__

#include "mv_common.h"
#include "image_helper.h"

/**
 * @brief The handle to the video reader.
 *
 * @since_tizen 3.0
 */
typedef void *mv_video_reader_h;

/**
 * @brief The handle to the video writer.
 *
 * @since_tizen 3.0
 */
typedef void *mv_video_writer_h;

/**
 * @brief Called when new sample is available from video reader.
 *
 * @since_tizen 3.0
 * @remarks You don't need release @a buffer independently
 * @param [in] buffer            Raw video buffer
 * @param [in] buffer_size       Size of video buffer
 * @param [in] image_data        Image data for corresponding video buffer
 * @param [in] user_data         User data
 *
 * @pre Create a reader handle by calling mv_video_reader_create()
 * @pre Set callback by calling mv_video_reader_set_new_sample_cb()
 *
 * @see mv_video_reader_set_new_sample_cb()
 */
typedef void (*mv_video_reader_new_sample_cb) (
        char *buffer,
        unsigned int buffer_size,
        image_data_s image_data,
        void *user_data);

/**
 * @brief Called when stream from video reader is finished.
 *
 * @since_tizen 3.0
 * @param [in] user_data         User data
 *
 * @pre Create a reader handle by calling mv_video_reader_create()
 * @pre Set callback by calling mv_video_reader_set_eos_cb()
 *
 * @see mv_video_reader_set_eos_cb()
 */
typedef void (*mv_video_reader_eos_cb) (
        void *user_data);

/**
 * @brief Creates a video reader handle.
 *
 * @since_tizen 3.0
 * @remarks You must release @a reader by using @ref mv_destroy_video_reader().
 * @param [out] reader    A new handle to the video reader
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_INVALID_OPERATION Invalid operation
 *
 * @see mv_destroy_video_reader()
 */
int mv_create_video_reader(
        mv_video_reader_h *reader);

/**
 * @brief Destroys the video reader handle and releases all its resources.
 *
 * @since_tizen 3.0
 * @param [in] reader    The handle to the video reader to be destroyed
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see mv_create_video_reader()
 */
int mv_destroy_video_reader(
        mv_video_reader_h reader);

/**
 * @brief Loads video from file.
 *
 * @since_tizen 3.0
 * @param [in]  reader      The handle to the video reader
 * @param [in]  path        Path to the video file to be loaded
 * @param [out] image_data  Image data for corresponding video buffer
 * @param [out] fps         Frame per second of corresponding video file
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_INVALID_OPERATION Invalid operation
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED_FORMAT Not supported video format
 *
 * @pre Create a video reader handle by calling @ref mv_create_video_reader()
 */
int mv_video_reader_load(
        mv_video_reader_h reader,
        const char *path,
        image_data_s *image_data,
        unsigned int *fps);

/**
 * @brief Starts reader playback.
 *
 * @since_tizen 3.0
 * @param [in] reader       The handle to the video reader
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_INVALID_OPERATION Invalid operation
 *
 * @pre Create a video reader handle by calling @ref mv_create_video_reader()
 *      and call @ref mv_video_reader_load()
 *
 * @post Stop reader playback by calling @ref mv_video_reader_stop()
 */
int mv_video_reader_start(
        mv_video_reader_h reader);

/**
 * @brief Stops reader playback.
 *
 * @since_tizen 3.0
 * @param [in] reader       The handle to the video reader
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_INVALID_OPERATION Invalid operation
 *
 * @pre Create a video reader handle by calling @ref mv_create_video_reader()
 *      and call @ref mv_video_reader_load()
 */
int mv_video_reader_stop(
        mv_video_reader_h reader);

/**
 * @brief Sets new sample callback to video reader.
 *
 * @since_tizen 3.0
 * @param [in] reader      The handle to the video reader
 * @param [in] callback    Callback that will be called
 * @param [in] user_data  User data
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @pre Create a video reader handle by calling @ref mv_create_video_reader()
 *      and load video with @ref mv_video_reader_load()
 *
 * @see mv_create_video_reader()
 * @see mv_video_reader_load()
 *
 */
int mv_video_reader_set_new_sample_cb(
    mv_video_reader_h reader,
    mv_video_reader_new_sample_cb callback,
    void *user_data);

/**
 * @brief Sets end of stream callback to video reader.
 *
 * @since_tizen 3.0
 * @param [in] reader     The handle to the video reader
 * @param [in] callback   Callback that will be called
 * @param [in] user_data  User data
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @pre Create a video reader handle by calling @ref mv_create_video_reader()
 *      and load video with @ref mv_video_reader_load()
 *
 * @see mv_create_video_reader()
 * @see mv_video_reader_load()
 *
 */
int mv_video_reader_set_eos_cb(
    mv_video_reader_h reader,
    mv_video_reader_eos_cb callback,
    void *user_data);

/**
 * @brief Creates a video writer handle.
 *
 * @since_tizen 3.0
 * @remarks You must release @a writer by using @ref mv_destroy_video_writer().
 * @param [out] writer    A new handle to the video writer
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_INVALID_OPERATION Invalid operation
 *
 * @see mv_destroy_video_writer()
 */
int mv_create_video_writer(
        mv_video_writer_h *writer);

/**
 * @brief Destroys the video writer handle and releases all its resources.
 *
 * @since_tizen 3.0
 * @param [in] writer    The handle to the video writer to be destroyed
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_INVALID_OPERATION Invalid operation
 *
 * @see mv_create_video_writer()
 */
int mv_destroy_video_writer(
        mv_video_writer_h writer);

/**
 * @brief Sets path and frame size for video file to be stored.
 *
 * @since_tizen 3.0
 * @param [in] writer      The handle to the video writer
 * @param [in] path        Path to the video file to be stored
 * @param [in] image_data  Image data for corresponding video buffer
 * @param [in] fps         Frame per second of resulted video file
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_INVALID_OPERATION Invalid operation
 * @retval #MEDIA_VISION_ERROR_NOT_SUPPORTED_FORMAT Not supported video format
 *
 * @pre Create a video writer handle by calling @ref mv_create_video_writer()
 */
int mv_video_writer_init(
        mv_video_writer_h writer,
        const char *path,
        image_data_s image_data,
        unsigned int fps);

/**
 * @brief   Writes consequently video frame to the file.
 * @details After video writer was initialized this function consequently
 *          writes frames to the file.
 *
 * @since_tizen 3.0
 * @param [in] writer      The handle to the video writer
 * @param [in] frame       Raw image buffer
 * @return @c 0 on success, otherwise a negative error value
 * @retval #MEDIA_VISION_ERROR_NONE Successful
 * @retval #MEDIA_VISION_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MEDIA_VISION_ERROR_INVALID_OPERATION Invalid operation
 *
 * @pre Create a video writer handle by calling @ref mv_create_video_writer()
 *      and initialize video with @ref mv_video_writer_init()
 */
int mv_video_writer_write_frame(
    mv_video_writer_h writer,
    unsigned char *frame);

#endif /* __MV_VIDEO_HELPER_H__ */
