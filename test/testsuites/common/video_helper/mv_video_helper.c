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

#include "mv_common.h"
#include "mv_video_helper.h"

#include "mv_log_cfg.h"

#include <string.h>
#include <stdio.h>

#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <gst/video/video.h>

#include <pthread.h>

typedef struct _mv_video_reader_s {
	/* Main bin */
	GstElement *pl;

	/* Pipeline structure */
	GstElement *filesrc;
	GstElement *decodebin;
	GstElement *videoconvert;
	GstElement *queue;
	GstElement *appsink;

	void *new_sample_cb_user_data;
	void *eos_cb_user_data;

	GstCaps *caps;
	gulong pad_probe_id;

	pthread_spinlock_t new_sample_cb_guard;
	pthread_spinlock_t eos_cb_guard;

	mv_video_reader_new_sample_cb new_sample_cb;
	mv_video_reader_eos_cb eos_cb;
} mv_video_reader_s;

typedef struct _mv_video_writer_s {
	/* Main bin */
	GstElement *pl;

	/* Pipeline structure */
	GstElement *appsrc;
	GstElement *capsfilter;
	GstElement *videoconvert;
	GstElement *encoder;
	GstElement *queue;
	GstElement *muxer;
	GstElement *filesink;

	image_data_s image_data;
	unsigned int fps;
	unsigned int buffer_size;
} mv_video_writer_s;

/* video reader internal funcitons */
static int _mv_video_reader_create_internals(mv_video_reader_s *reader);
static int _mv_video_reader_link_internals(mv_video_reader_s *reader);
static int _mv_video_reader_state_change(mv_video_reader_s *reader, GstState state);

/* video writer internal funciton */
static int _mv_video_writer_create_internals(mv_video_writer_s *writer);
static int _mv_video_writer_link_internals(mv_video_writer_s *writer);
static int _mv_video_writer_state_change(mv_video_writer_s *writer, GstState state);

static void appsink_eos(GstAppSink *appsink, gpointer user_data);
static GstFlowReturn appsink_newsample(GstAppSink *appsink, gpointer user_data);
static void cb_newpad(GstElement *decodebin, GstPad *new_pad, gpointer user_data);

static GstPadProbeReturn pad_probe_data_cb (GstPad *pad, GstPadProbeInfo *info, gpointer user_data);

/* video reader */
int mv_create_video_reader(
		mv_video_reader_h *reader)
{
	mv_video_reader_s *handle = NULL;
	int err = MEDIA_VISION_ERROR_NONE;

	if (reader == NULL) {
		LOGE("NULL pointer passed");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	gst_init(NULL, NULL);

	handle = (mv_video_reader_s *) malloc(sizeof(mv_video_reader_s));
	if (!handle) {
		LOGE("Not enough memory");
		return MEDIA_VISION_ERROR_INVALID_OPERATION;
	}
	memset(handle, 0, sizeof(mv_video_reader_s));

	err = _mv_video_reader_create_internals(handle);
	if (MEDIA_VISION_ERROR_NONE != err) {
		LOGE("Failed to create internals");
		free(handle);
		return err;
	}

	err = _mv_video_reader_link_internals(handle);
	if (MEDIA_VISION_ERROR_NONE != err) {
		LOGE("Failed to link internals");
		free(handle);
		return err;
	}

	*reader = (mv_video_reader_s *) handle;

	return err;
}

int mv_destroy_video_reader(
		mv_video_reader_h reader)
{
	mv_video_reader_s *handle = NULL;

	if (reader == NULL) {
		LOGE("NULL pointer passed");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	handle = (mv_video_reader_s *) reader;

	if (handle->caps && GST_OBJECT_REFCOUNT(handle->caps))
		gst_caps_unref(handle->caps);

	if (handle->pl)
		gst_object_unref(handle->pl);

	handle->pl = NULL;

	pthread_spin_destroy(&(handle->new_sample_cb_guard));
	pthread_spin_destroy(&(handle->eos_cb_guard));

	LOGD("video reader destroyed %p", handle);

	free(handle);

	return MEDIA_VISION_ERROR_NONE;
}

int mv_video_reader_load(
		mv_video_reader_h reader,
		const char *path,
		image_data_s *image_data,
		unsigned int *fps)
{
	mv_video_reader_s *handle = NULL;
	GstVideoInfo info;

	if (reader == NULL || path == NULL ||
		image_data == NULL || fps == NULL) {
		LOGE("NULL pointer passed");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	handle = (mv_video_reader_s *) reader;

	/* Set input file location from path */
	g_object_set(G_OBJECT(handle->filesrc),
				"location", path,
				NULL);

	/* Start playback */
	if (_mv_video_reader_state_change(handle, GST_STATE_PLAYING)) {
		LOGE("Unable to change state");
		return MEDIA_VISION_ERROR_INVALID_OPERATION;
	}

	if (_mv_video_reader_state_change(handle, GST_STATE_PAUSED)) {
		LOGE("Unable to change state");
		return MEDIA_VISION_ERROR_INVALID_OPERATION;
	}

	if (handle->caps == NULL) {
		LOGE("Unable to get caps from decodebin");
		return MEDIA_VISION_ERROR_INVALID_OPERATION;
	}

	gst_video_info_from_caps(&info, handle->caps);

	gst_caps_unref(handle->caps);

	*fps = info.fps_n/info.fps_d;

	/* Fill image data */
	image_data->image_width = info.width;
	image_data->image_height = info.height;

	/* Look to :
	 * http://gstreamer.freedesktop.org/data/doc/gstreamer/head/gst-plugins-base-libs/html/gst-plugins-base-libs-gstvideo.html#GstVideoFormat */
	switch (GST_VIDEO_FORMAT_INFO_FORMAT(info.finfo)) {
	case(GST_VIDEO_FORMAT_GRAY8):
		image_data->image_colorspace = MEDIA_VISION_COLORSPACE_Y800;
		break;
	case(GST_VIDEO_FORMAT_I420):
		image_data->image_colorspace = MEDIA_VISION_COLORSPACE_I420;
		break;
	case(GST_VIDEO_FORMAT_NV12):
		image_data->image_colorspace = MEDIA_VISION_COLORSPACE_NV12;
		break;
	case(GST_VIDEO_FORMAT_YV12):
		image_data->image_colorspace = MEDIA_VISION_COLORSPACE_YV12;
		break;
	case(GST_VIDEO_FORMAT_NV21):
		image_data->image_colorspace = MEDIA_VISION_COLORSPACE_NV21;
		break;
	case(GST_VIDEO_FORMAT_YUY2):
		image_data->image_colorspace = MEDIA_VISION_COLORSPACE_YUYV;
		break;
	case(GST_VIDEO_FORMAT_UYVY):
		image_data->image_colorspace = MEDIA_VISION_COLORSPACE_UYVY;
		break;
	case(GST_VIDEO_FORMAT_RGB):
		image_data->image_colorspace = MEDIA_VISION_COLORSPACE_RGB888;
		break;
	case(GST_VIDEO_FORMAT_RGBA):
		image_data->image_colorspace = MEDIA_VISION_COLORSPACE_RGBA;
		break;
	default:
		LOGE("Video pixel format is not supported\n");
		return MEDIA_VISION_ERROR_NOT_SUPPORTED_FORMAT;
	}

	return MEDIA_VISION_ERROR_NONE;
}

int mv_video_reader_start(
		mv_video_reader_h reader)
{
	mv_video_reader_s *handle = NULL;

	if (reader == NULL) {
		LOGE("NULL pointer passed");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	handle = (mv_video_reader_s *) reader;

	/* Start playback */
	if (_mv_video_reader_state_change(handle, GST_STATE_PLAYING)) {
		LOGE("Unable to change state");
		return MEDIA_VISION_ERROR_INVALID_OPERATION;
	}

	return MEDIA_VISION_ERROR_NONE;
}

int mv_video_reader_stop(
		mv_video_reader_h reader)
{
	mv_video_reader_s *handle = NULL;

	if (reader == NULL) {
		LOGE("NULL pointer passed");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	handle = (mv_video_reader_s *) reader;

	/* Stop playback (NULL or READY) */
	if (_mv_video_reader_state_change(handle, GST_STATE_NULL)) {
		LOGE("Unable to change state");
		return MEDIA_VISION_ERROR_INVALID_OPERATION;
	}

	return MEDIA_VISION_ERROR_NONE;
}

int mv_video_reader_set_new_sample_cb(
		mv_video_reader_h reader,
		mv_video_reader_new_sample_cb callback,
		void *user_data)
{
	mv_video_reader_s *handle = NULL;

	if (reader == NULL || callback == NULL) {
		LOGE("NULL pointer passed");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	handle = (mv_video_reader_s *) reader;

	pthread_spin_lock(&(handle->new_sample_cb_guard));
	handle->new_sample_cb = callback;
	handle->new_sample_cb_user_data = user_data;
	pthread_spin_unlock(&(handle->new_sample_cb_guard));

	return MEDIA_VISION_ERROR_NONE;
}

int mv_video_reader_set_eos_cb(
		mv_video_reader_h reader,
		mv_video_reader_eos_cb callback,
		void *user_data)
{
	mv_video_reader_s *handle = NULL;

	if (reader == NULL || callback == NULL) {
		LOGE("NULL pointer passed");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	handle = (mv_video_reader_s *) reader;

	pthread_spin_lock(&(handle->eos_cb_guard));
	handle->eos_cb = callback;
	handle->eos_cb_user_data = user_data;
	pthread_spin_unlock(&(handle->eos_cb_guard));

	return MEDIA_VISION_ERROR_NONE;
}

/* Video Writer */
int mv_create_video_writer(
		mv_video_writer_h *writer)
{
	mv_video_writer_s *handle = NULL;
	int err = MEDIA_VISION_ERROR_NONE;

	if (writer == NULL) {
		LOGE("NULL pointer passed");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	gst_init(NULL, NULL);

	handle = (mv_video_writer_s *) malloc(sizeof(mv_video_writer_s));
	if (!handle) {
		LOGE("Not enough memory");
		return MEDIA_VISION_ERROR_INVALID_OPERATION;
	}
	memset(handle, 0, sizeof(mv_video_writer_s));

	err = _mv_video_writer_create_internals(handle);
	if (MEDIA_VISION_ERROR_NONE != err) {
		LOGE("Failed to create internals");
		free(handle);
		return err;
	}

	*writer = (mv_video_writer_s *) handle;

	return err;
}

int mv_destroy_video_writer(
		mv_video_writer_h writer)
{
	mv_video_writer_s *handle = NULL;

	if (writer == NULL) {
		LOGE("NULL pointer passed");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	handle = (mv_video_writer_s *) writer;

	_mv_video_writer_state_change(writer, GST_STATE_NULL);

	if (handle->pl)
		gst_object_unref(handle->pl);

	handle->pl = NULL;

	LOGD("video writer destroyed %p", handle);

	free(handle);

	return MEDIA_VISION_ERROR_NONE;
}

int mv_video_writer_init(
		mv_video_writer_h writer,
		const char *path,
		image_data_s image_data,
		unsigned int fps)
{
	mv_video_writer_s *handle = NULL;
	unsigned int err = MEDIA_VISION_ERROR_NONE;

	if (writer == NULL) {
		LOGE("NULL pointer passed");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	handle = (mv_video_writer_s *) writer;

	handle->image_data.image_width = image_data.image_width;
	handle->image_data.image_height = image_data.image_height;
	handle->image_data.image_colorspace = image_data.image_colorspace;

	handle->fps = fps;

	g_object_set(G_OBJECT(handle->filesink),
					"location", path,
					NULL);

	err = _mv_video_writer_link_internals(handle);
	if (MEDIA_VISION_ERROR_NONE != err) {
		LOGE("Failed to link internals");
		return err;
	}

	return err;
}

int mv_video_writer_write_frame(
		mv_video_writer_h writer,
		unsigned char *frame)
{
	mv_video_writer_s *handle = NULL;
	GstMapInfo info;
	GstBuffer *buffer = NULL;

	if (writer == NULL || frame == NULL) {
		LOGE("NULL pointer passed");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	handle = (mv_video_writer_s *) writer;

	buffer =  gst_buffer_new_allocate(NULL, handle->buffer_size, NULL);
	if (!buffer) {
		LOGE("Unable to allocate buffer for frame");
		return MEDIA_VISION_ERROR_INVALID_OPERATION;
	}

	LOGD("Copying input frame to buffer and pushing to appsrc");
	gst_buffer_map(buffer, &info, GST_MAP_READWRITE);
	memcpy(info.data, frame, info.size);
	gst_buffer_unmap(buffer, &info);

	if (GST_FLOW_OK !=
		gst_app_src_push_buffer(handle->appsrc, buffer)) {
		LOGE("Failed to push buffer to appsrc");
		return MEDIA_VISION_ERROR_INVALID_OPERATION;
	}

	return MEDIA_VISION_ERROR_NONE;
}

/* Internal functions */
static int _mv_video_reader_create_internals(
		mv_video_reader_s *reader)
{
	pthread_spin_init(&(reader->new_sample_cb_guard), PTHREAD_PROCESS_SHARED);
	pthread_spin_init(&(reader->eos_cb_guard), PTHREAD_PROCESS_SHARED);

	reader->pl = gst_pipeline_new(NULL);

	reader->filesrc = gst_element_factory_make("filesrc", "filesrc");
	reader->decodebin = gst_element_factory_make("decodebin", "decoder");
	reader->videoconvert = gst_element_factory_make("videoconvert", "convert");
	reader->queue = gst_element_factory_make("queue", "queue");
	reader->appsink = gst_element_factory_make("appsink", "appsink");

	if ((!reader->pl) ||
		(!reader->filesrc) ||
		(!reader->decodebin) ||
		(!reader->videoconvert) ||
		(!reader->queue) ||
		(!reader->appsink)) {
		LOGE("Unable to create video read pipeline elements");
		return MEDIA_VISION_ERROR_INVALID_OPERATION;
	}

	gst_bin_add_many(GST_BIN(reader->pl),
					reader->filesrc,
					reader->decodebin,
					reader->videoconvert,
					reader->queue,
					reader->appsink,
					NULL);

	return MEDIA_VISION_ERROR_NONE;
}

static int _mv_video_reader_link_internals(
		mv_video_reader_s *reader)
{
	GstCaps *caps = NULL;
	GstPad *pad = NULL;

	if (!gst_element_link_many(reader->filesrc,
				reader->decodebin,
				NULL)) {
		LOGE("Unable to link filesrc to decodebin");
		return MEDIA_VISION_ERROR_INVALID_OPERATION;
	}

	/* Decodebin pad will be linked during state change */
	g_signal_connect(reader->decodebin,
					"pad-added",
					G_CALLBACK(cb_newpad),
					reader);

	if (!gst_element_link_many(reader->videoconvert,
			reader->queue,
			reader->appsink,
			 NULL)) {
		LOGE("Unable to link videocovnert-queue-appsink");
		return MEDIA_VISION_ERROR_INVALID_OPERATION;
	}

	caps = gst_caps_new_simple("video/x-raw",
					"format", G_TYPE_STRING, "RGB",
					NULL);

	gst_app_sink_set_caps(GST_APP_SINK(reader->appsink), caps);
	gst_caps_unref(caps);

	/* Configure appsink */
	gst_app_sink_set_emit_signals(GST_APP_SINK(reader->appsink), TRUE);
	g_signal_connect(reader->appsink,
					"new-sample",
					G_CALLBACK(appsink_newsample),
					reader);
	g_signal_connect(reader->appsink,
					"eos",
					G_CALLBACK(appsink_eos),
					reader);
	g_object_set(G_OBJECT(reader->appsink),
					"drop", TRUE,
					"enable-last-sample", TRUE,
					"sync", FALSE,
					NULL);


	/* pad probe */
	pad = gst_element_get_static_pad(reader->queue, "src");

	gst_pad_add_probe(pad, GST_PAD_PROBE_TYPE_BUFFER,
					(GstPadProbeCallback)pad_probe_data_cb, reader, NULL);
	gst_object_unref(pad);

	return MEDIA_VISION_ERROR_NONE;
}

static int _mv_video_reader_state_change(
		mv_video_reader_s *reader,
		GstState state)
{
	mv_video_reader_s *handle = (mv_video_reader_s *) reader;
	GstStateChangeReturn state_ret = GST_STATE_CHANGE_FAILURE;
	GstState pipeline_state = GST_STATE_NULL;

	state_ret = gst_element_set_state(handle->pl,
						state);

	if (GST_STATE_CHANGE_FAILURE == state_ret) {
		LOGE("Set state failure");
		return MEDIA_VISION_ERROR_INVALID_OPERATION;
	}

	LOGI("Set state [%d], change return [%d]",
		state, state_ret);

	state_ret = gst_element_get_state(handle->pl,
					&pipeline_state,
					NULL,
					GST_CLOCK_TIME_NONE);

	if (GST_STATE_CHANGE_FAILURE == state_ret) {
		LOGE("get state failure");
		return MEDIA_VISION_ERROR_INVALID_OPERATION;
	}

	return MEDIA_VISION_ERROR_NONE;
}

static int _mv_video_writer_create_internals(
		mv_video_writer_s *writer)
{
	writer->pl = gst_pipeline_new(NULL);

	writer->appsrc = gst_element_factory_make("appsrc", "appsrc");
	writer->capsfilter = gst_element_factory_make("capsfilter", NULL);
	writer->videoconvert = gst_element_factory_make("videoconvert", "videoconvert");
	writer->encoder = gst_element_factory_make("avenc_mpeg4", "encoder");
	writer->queue = gst_element_factory_make("queue", "queue");
	writer->muxer = gst_element_factory_make("avmux_avi", "muxer");
	writer->filesink = gst_element_factory_make("filesink", "filesink");

	if ((!writer->pl) ||
		(!writer->appsrc) ||
		(!writer->capsfilter) ||
		(!writer->videoconvert) ||
		(!writer->encoder) ||
		(!writer->queue) ||
		(!writer->muxer) ||
		(!writer->filesink)) {
		LOGE("Unable to create video read pipeline elements\n");
		return MEDIA_VISION_ERROR_INVALID_OPERATION;
	}

	gst_bin_add_many(GST_BIN(writer->pl),
			writer->appsrc,
			writer->capsfilter,
			writer->videoconvert,
			writer->encoder,
			writer->queue,
			writer->muxer,
			writer->filesink,
			NULL);

	return MEDIA_VISION_ERROR_NONE;
}

static int _mv_video_writer_link_internals(
		mv_video_writer_s *writer)
{
	GstVideoInfo vinfo;
	GstCaps *caps = NULL;
	char format[6] = {0};

	/* Convert from mv_colorspace to GstVideoFormat */
	switch (writer->image_data.image_colorspace) {
	case(MEDIA_VISION_COLORSPACE_Y800):
		strncpy(format, "GRAY8", 5);
		break;
	case(MEDIA_VISION_COLORSPACE_I420):
		strncpy(format, "I420", 4);
		break;
	case(MEDIA_VISION_COLORSPACE_NV12):
		strncpy(format, "NV12", 4);
		break;
	case(MEDIA_VISION_COLORSPACE_YV12):
		strncpy(format, "YV12", 4);
		break;
	case(MEDIA_VISION_COLORSPACE_NV21):
		strncpy(format, "NV21", 4);
		break;
	case(MEDIA_VISION_COLORSPACE_YUYV):
		strncpy(format, "YUY2", 4);
		break;
	case(MEDIA_VISION_COLORSPACE_UYVY):
		strncpy(format, "UYVY", 4);
		break;
	case(MEDIA_VISION_COLORSPACE_RGB888):
		strncpy(format, "RGB", 3);
		break;
	case(MEDIA_VISION_COLORSPACE_RGBA):
		strncpy(format, "RGBA", 4);
		break;
	default:
		LOGE("Selected format %d is not supported",
				writer->image_data.image_colorspace);
		return MEDIA_VISION_ERROR_NOT_SUPPORTED_FORMAT;
	}

	caps = gst_caps_new_simple("video/x-raw",
				"format", G_TYPE_STRING, format,
				"width", G_TYPE_INT, writer->image_data.image_width,
				"height", G_TYPE_INT, writer->image_data.image_height,
				"framerate", GST_TYPE_FRACTION, writer->fps, 1,
				NULL);

	if (NULL == caps) {
		LOGE("Failed to create new caps");
		return MEDIA_VISION_ERROR_INVALID_OPERATION;
	}

	/* This is the simpler way to get buffer size */
	if (!gst_video_info_from_caps(&vinfo, caps)) {
		LOGE("Unable to set buffer size");
		gst_caps_unref(caps);
		return MEDIA_VISION_ERROR_INVALID_OPERATION;
	}

	writer->buffer_size = vinfo.size;

	/* link appsrc and capsfilter */
	if ((!gst_element_link_filtered(writer->appsrc,
					writer->capsfilter,
					caps))) {
		LOGE("Failed to link appsrc to capsfilter");
		gst_caps_unref(caps);
		return MEDIA_VISION_ERROR_INVALID_OPERATION;
	}
	gst_caps_unref(caps);

	if (!gst_element_link_many(writer->capsfilter,
				writer->videoconvert,
				writer->encoder,
				writer->queue,
				writer->muxer,
				writer->filesink,
				NULL)) {
		LOGE("Unable to capsfilter to filesink");
		return MEDIA_VISION_ERROR_INVALID_OPERATION;
	}

	g_object_set(G_OBJECT(writer->appsrc),
				"max-bytes", 0,
				"blocksize", writer->buffer_size,
				"stream-type", 0,
				"format", GST_FORMAT_BYTES,
				NULL);

	if (_mv_video_writer_state_change(writer,
					GST_STATE_PLAYING)) {
			LOGE("Unable to change video writer state");
			return MEDIA_VISION_ERROR_INVALID_OPERATION;
	}

	return MEDIA_VISION_ERROR_NONE;
}

static int _mv_video_writer_state_change(
		mv_video_writer_s *writer,
		GstState state)
{
	mv_video_writer_s *handle = (mv_video_writer_s *) writer;
	GstStateChangeReturn state_ret = GST_STATE_CHANGE_FAILURE;
	GstState pipeline_state = GST_STATE_NULL;

	state_ret = gst_element_set_state(handle->pl,
								state);

	if (GST_STATE_CHANGE_FAILURE == state_ret) {
		LOGE("Set state failure");
		return MEDIA_VISION_ERROR_INVALID_OPERATION;
	}

	LOGI("Set state [%d], change return [%d]",
			state, state_ret);

	/* AppSrc can't go to PLAYING state before buffer is not pushed */

	return MEDIA_VISION_ERROR_NONE;
}

/* Callbacks */
static GstFlowReturn appsink_newsample(
		GstAppSink *appsink,
		gpointer user_data)
{
	mv_video_reader_s *handle = NULL;
	GstSample *sample = gst_app_sink_pull_sample(appsink);

	if (user_data == NULL) {
		LOGE("NULL pointer passed");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	if (sample != NULL) {
		handle = (mv_video_reader_s *) user_data;
		GstVideoInfo vinfo;
		GstMapInfo info = GST_MAP_INFO_INIT;
		GstBuffer *buf = gst_sample_get_buffer(sample);
		GstCaps *caps = gst_sample_get_caps(sample);
		image_data_s im_data;
		char *buffer = NULL;
		unsigned int buffer_size = 0;

		LOGD("Received sample from appsink");

		/* map buffer */
		gst_buffer_map(buf, &info, GST_MAP_READ);
		buffer = (char *) info.data;

		/* Fill image data */
		gst_video_info_from_caps(&vinfo, caps);
		im_data.image_width = vinfo.width;
		im_data.image_height = vinfo.height;

		/* Look to :
		 * http://gstreamer.freedesktop.org/data/doc/gstreamer/head/gst-plugins-base-libs/html/gst-plugins-base-libs-gstvideo.html#GstVideoFormat */
		switch (GST_VIDEO_FORMAT_INFO_FORMAT(vinfo.finfo)) {
		case(GST_VIDEO_FORMAT_GRAY8):
			im_data.image_colorspace = MEDIA_VISION_COLORSPACE_Y800;
			break;
		case(GST_VIDEO_FORMAT_I420):
			im_data.image_colorspace = MEDIA_VISION_COLORSPACE_I420;
			break;
		case(GST_VIDEO_FORMAT_NV12):
			im_data.image_colorspace = MEDIA_VISION_COLORSPACE_NV12;
			break;
		case(GST_VIDEO_FORMAT_YV12):
			im_data.image_colorspace = MEDIA_VISION_COLORSPACE_YV12;
			break;
		case(GST_VIDEO_FORMAT_NV21):
			im_data.image_colorspace = MEDIA_VISION_COLORSPACE_NV21;
			break;
		case(GST_VIDEO_FORMAT_YUY2):
			im_data.image_colorspace = MEDIA_VISION_COLORSPACE_YUYV;
			break;
		case(GST_VIDEO_FORMAT_UYVY):
			im_data.image_colorspace = MEDIA_VISION_COLORSPACE_UYVY;
			break;
		case(GST_VIDEO_FORMAT_RGB):
			im_data.image_colorspace = MEDIA_VISION_COLORSPACE_RGB888;
			break;
		case(GST_VIDEO_FORMAT_RGBA):
			im_data.image_colorspace = MEDIA_VISION_COLORSPACE_RGBA;
			break;
		default:
			LOGE("Video pixel format is not supported\n");
			gst_buffer_unmap(buf, &info);
			gst_sample_unref(sample);
			return GST_FLOW_ERROR;
		}

		pthread_spin_lock(&(handle->new_sample_cb_guard));
		if (handle->new_sample_cb != NULL) {
			handle->new_sample_cb(
				buffer,
				info.size,
				im_data,
				handle->new_sample_cb_user_data);
		}
		pthread_spin_unlock(&(handle->new_sample_cb_guard));

		gst_buffer_unmap(buf, &info);
		gst_sample_unref(sample);
	} else {
		LOGE("Failed to pull sample from appsink");
		return GST_FLOW_ERROR;
	}

	return GST_FLOW_OK;
}

static void appsink_eos(
		GstAppSink *appsink,
		gpointer user_data)
{
	if (user_data == NULL) {
		LOGE("NULL pointer passed");
		return;
	}

	mv_video_reader_s *handle = (mv_video_reader_s *) user_data;

	/* EOS callback to terminate reading */
	pthread_spin_lock(&(handle->eos_cb_guard));
	if (handle->eos_cb != NULL)
		handle->eos_cb(handle->eos_cb_user_data);

	pthread_spin_unlock(&(handle->eos_cb_guard));


	gst_pad_remove_probe(gst_element_get_static_pad(handle->queue, "src"), handle->pad_probe_id);
}

static void cb_newpad(
		GstElement *decodebin,
		GstPad *pad,
		gpointer user_data)
{
	mv_video_reader_s *reader = (mv_video_reader_s *) user_data;
	GstStructure *str = NULL;
	GstCaps *caps = NULL;
	GstPad *video_pad = NULL;

	LOGI("Received pad from decodebin. Linking");
	video_pad = gst_element_get_static_pad(reader->videoconvert, "sink");
	if (GST_PAD_IS_LINKED(video_pad)) {
		LOGI("Already linked");
		g_object_unref(video_pad);
		return;
	}

	/* Check for pad is video */
	caps = gst_pad_query_caps(pad, NULL);
	str = gst_caps_get_structure(caps, 0);
	if (!g_strrstr(gst_structure_get_name(str), "video")) {
		LOGI("Not a video pad");
		gst_object_unref(video_pad);
		return;
	}
	gst_caps_unref(caps);
	gst_pad_link(pad, video_pad);
	g_object_unref(video_pad);
}

static GstPadProbeReturn pad_probe_data_cb (
	GstPad *pad,
	GstPadProbeInfo *info,
	gpointer user_data)
{
	if (user_data == NULL) {
		return GST_PAD_PROBE_PASS;
	}
	mv_video_reader_s *reader = (mv_video_reader_s *) user_data;

	if (reader->caps == NULL) {
		reader->caps = gst_pad_get_current_caps(pad);
		reader->pad_probe_id = GST_PAD_PROBE_INFO_ID(info);
	}

	return GST_PAD_PROBE_OK;
}
