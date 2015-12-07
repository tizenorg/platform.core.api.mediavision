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

#include "mv_image.h"
#include "mv_private.h"

#include "image_helper.h"
#include "mv_video_helper.h"
#include "mv_testsuite_common.h"

#include <glib-2.0/glib.h>
#include <string.h>

#include <pthread.h>

typedef enum {
	SOURCE_TYPE_GENERATION,
	SOURCE_TYPE_LOADING,
	SOURCE_TYPE_CLONING,
	SOURCE_TYPE_EMPTY,
	SOURCE_TYPE_INVALID
} source_type_e;

typedef enum {
	OBJECT_TYPE_IMAGE_OBJECT,
	OBJECT_TYPE_IMAGE_TRACKING_MODEL,
	OBJECT_TYPE_INVALID
} testing_object_type_e;

#define testing_object_maximum_label_length 300

typedef struct testing_object_s {
	void *entity;

	char origin_label[testing_object_maximum_label_length];

	char actual_label[testing_object_maximum_label_length];

	testing_object_type_e object_type;

	source_type_e source_type;

	int cloning_counter;
} testing_object;

typedef testing_object *testing_object_h;

void testing_object_create(testing_object_h *result)
{
	(*result) = malloc(sizeof(testing_object));

	(*result)->entity = (void*)NULL;
	(*result)->object_type = OBJECT_TYPE_INVALID;
	(*result)->source_type = SOURCE_TYPE_INVALID;
	(*result)->cloning_counter = 0;
	(*result)->origin_label[0] = '\0';
	(*result)->actual_label[0] = '\0';
}

void testing_object_fill(
		testing_object_h target,
		void *entity,
		testing_object_type_e object_type,
		source_type_e source_type,
		void *source)
{
	target->entity = entity;
	target->object_type = object_type;
	target->source_type = source_type;
	target->cloning_counter = 0;

	switch (source_type) {
	case SOURCE_TYPE_GENERATION: {
		if (OBJECT_TYPE_IMAGE_OBJECT == object_type) {
			snprintf(
						target->origin_label,
						testing_object_maximum_label_length,
						"generated from \"%s\"",
						(char*)source);
		} else if (OBJECT_TYPE_IMAGE_TRACKING_MODEL == object_type) {
			snprintf(
						target->origin_label,
						testing_object_maximum_label_length,
						"generated from image object which is %s",
						((testing_object_h)source)->actual_label);
		} else {
			snprintf(
						target->origin_label,
						testing_object_maximum_label_length,
						"generated unknown type of testing object");
		}

		strncpy(target->actual_label, target->origin_label, testing_object_maximum_label_length);
		break;
	}
	case SOURCE_TYPE_LOADING: {
		snprintf(target->origin_label, testing_object_maximum_label_length, "loaded from \"%s\"", (char*)source);
		strncpy(target->actual_label, target->origin_label, testing_object_maximum_label_length);
		break;
	}
	case SOURCE_TYPE_CLONING: {
		testing_object_h source_object = (testing_object_h)source;
		strncpy(target->origin_label, source_object->origin_label, testing_object_maximum_label_length);
		target->cloning_counter = source_object->cloning_counter + 1;

		char number_of_cloning[10];
		number_of_cloning[0] = '\0';
		if (1 < target->cloning_counter) {
			snprintf(number_of_cloning, 10, "%s%i%s",
						"(x", target->cloning_counter, ")");
		}

		char type_name[20];
		if (OBJECT_TYPE_IMAGE_OBJECT == object_type)
			snprintf(type_name, 20, "image object");
		else if (OBJECT_TYPE_IMAGE_TRACKING_MODEL == object_type)
			snprintf(type_name, 20, "tracking model");
		else
			snprintf(type_name, 20, "unknown object");

		snprintf(target->actual_label, testing_object_maximum_label_length,
						"%s%s%s%s%s%s",
						"cloned ", number_of_cloning,
						" from ", type_name,
						" which is ", target->origin_label);
		break;
	}
	case SOURCE_TYPE_EMPTY: {
		strncpy(target->origin_label, "created an empty", testing_object_maximum_label_length);
		strncpy(target->actual_label, target->origin_label, testing_object_maximum_label_length);
		break;
	}
	default: {
		strncpy(target->origin_label, "having unknown source", testing_object_maximum_label_length);
		break;
	}
	}
}

void testing_object_destroy(testing_object_h *target)
{
	switch ((*target)->object_type) {
	case OBJECT_TYPE_IMAGE_OBJECT: {
		int err = mv_image_object_destroy((mv_image_object_h)((*target)->entity));
		if (MEDIA_VISION_ERROR_NONE != err) {
			printf("\nERROR: Errors were occurred during image object "
						"destroying; code %i\n", err);
		}
		break;
	}
	case OBJECT_TYPE_IMAGE_TRACKING_MODEL: {
		int err = mv_image_tracking_model_destroy(
					(mv_image_tracking_model_h)((*target)->entity));
		if (MEDIA_VISION_ERROR_NONE != err) {
			printf("\nERROR: Errors were occurred during image tracking "
						"model destroying; code %i\n", err);
		}
		break;
	}
	}
	free(*target);
	(*target) = NULL;
}

typedef struct {
	mv_quadrangle_s **locations;
	unsigned int locations_size;
	unsigned int currently_number;
} recognition_result;

void destroy_recognition_result(recognition_result *result)
{
	if (result->locations_size == 0)
		return;

	int i = 0;
	for (; i < result->locations_size; ++i) {
		if (NULL != result->locations[i])
			free(result->locations[i]);
	}
	free(result->locations);
}

void recognized_cb(
		mv_source_h source,
		mv_engine_config_h engine_cfg,
		const mv_image_object_h *image_objects,
		mv_quadrangle_s **locations,
		unsigned int number_of_objects,
		void *user_data)
{
	MEDIA_VISION_FUNCTION_ENTER();

	if (NULL == user_data)
		return;

	recognition_result *result = (recognition_result*)user_data;

	int object_num = 0;
	for (; object_num < number_of_objects; ++object_num) {
		if (result->currently_number >= result->locations_size)
			return;

		if (NULL == locations[object_num]) {
			result->locations[result->currently_number] = NULL;
		} else {
			result->locations[result->currently_number] = malloc(sizeof(mv_quadrangle_s));
			*(result->locations[result->currently_number]) = *(locations[object_num]);
		}

		++result->currently_number;
	}

	MEDIA_VISION_FUNCTION_LEAVE();
}

void handle_recognition_result(
		const recognition_result *result,
		int number_of_objects,
		mv_source_h *source,
		char *file_name)
{
	int is_source_data_loaded = 0;

	unsigned char *out_buffer = NULL;
	int buffer_size = 0;
	image_data_s image_data = { 0, 0, MEDIA_VISION_COLORSPACE_INVALID };

	if (MEDIA_VISION_ERROR_NONE != mv_source_get_buffer(source, &(out_buffer), &buffer_size) ||
		MEDIA_VISION_ERROR_NONE != mv_source_get_width(source, &(image_data.image_width)) ||
		MEDIA_VISION_ERROR_NONE != mv_source_get_height(source, &(image_data.image_height)) ||
		MEDIA_VISION_ERROR_NONE != mv_source_get_colorspace(source, &(image_data.image_colorspace)) ||
		NULL == file_name) {
			printf("ERROR: Creating out image is impossible.\n");
	} else {
		is_source_data_loaded = 1;
	}

	int object_num = 0;

	for (; object_num < number_of_objects; ++object_num) {
		if (NULL == result->locations[object_num]) {
			printf("\nImage #%i is not recognized\n", object_num);
			continue;
		}

		printf("\nImage #%i is recognized\n", object_num);
		printf("Recognized image coordinates:\n");

		int point_num = 0;
		for (; point_num < 4; ++point_num) {
			printf("%d point - x = %d, y = %d\n", point_num + 1,
					result->locations[object_num]->points[point_num].x,
					result->locations[object_num]->points[point_num].y);
		}

		if (is_source_data_loaded) {
			const int thickness = 2;
			const int color[] = {0, 255, 0};

			const int err = draw_quadrangle_on_buffer(
					*(result->locations[object_num]),
					thickness,
					color,
					&image_data,
					out_buffer);

			if (MEDIA_VISION_ERROR_NONE != err)
				printf("ERROR: Impossible to draw quadrangle\n");
		}
	}

	if (save_image_from_buffer(file_name, out_buffer,
			&image_data, 100) != MEDIA_VISION_ERROR_NONE) {
		printf("\nERROR: Failed to generate output file\n");
	} else {
		printf("\nImage was generated as %s\n", file_name);
	}
}

int generate_image_object_from_file(const char *path_to_image,
						bool roi_selected,
						mv_rectangle_s roi,
						mv_image_object_h *result)
{
	MEDIA_VISION_FUNCTION_ENTER();

	mv_source_h source;
	int err = mv_create_source(&source);
	if (MEDIA_VISION_ERROR_NONE != err)
		printf("ERROR: Errors were occurred during source creating!!! code %i\n", err);

	err = load_mv_source_from_file(path_to_image, source);

	if (MEDIA_VISION_ERROR_NONE != err) {
		printf("ERROR: image is not loaded; code %i\n", err);

		int err2 = mv_destroy_source(source);
		if (MEDIA_VISION_ERROR_NONE != err2) {
			printf("\nERROR: Errors were occurred during source "
					"destroying; code %i\n", err2);
		}

		MEDIA_VISION_FUNCTION_LEAVE();

		return err;
	}

	mv_engine_config_h config;
	err = mv_create_engine_config(&config);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf("ERROR: engine configuration is not created; code %i\n", err);

		int err2 = mv_destroy_source(source);
		if (MEDIA_VISION_ERROR_NONE != err2) {
			printf("\nERROR: Errors were occurred during source "
					"destroying; code %i\n", err2);
		}

		MEDIA_VISION_FUNCTION_LEAVE();

		return err;
	}

	err = mv_image_object_create(result);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf("ERROR: Errors were occurred during creating image object; "
				"code %i\n", err);

		int err2 = mv_destroy_source(source);
		if (MEDIA_VISION_ERROR_NONE != err2) {
		printf("\nERROR: Errors were occurred during source "
					"destroying; code %i\n", err2);
		}

		err2 = mv_destroy_engine_config(config);
		if (MEDIA_VISION_ERROR_NONE != err2) {
			printf("\nERROR: Errors were occurred during engine config "
					"destroying; code %i\n", err2);
		}

		MEDIA_VISION_FUNCTION_LEAVE();

		return err;
	}

	if (roi_selected)
		err = mv_image_object_fill(*result, config, source, &roi);
	else
		err = mv_image_object_fill(*result, config, source, NULL);

	if (MEDIA_VISION_ERROR_NONE != err) {
		printf("ERROR: Errors were occurred during filling image object; "
				"code %i\n", err);

		int err2 = mv_destroy_source(source);
		if (MEDIA_VISION_ERROR_NONE != err2) {
			printf("\nERROR: Errors were occurred during source "
					"destroying; code %i\n", err2);
		}

		err2 = mv_image_object_destroy(*result);
		if (MEDIA_VISION_ERROR_NONE != err2) {
			printf("\nERROR: Errors were occurred during image object "
					"destroying; code %i\n", err2);
		}

		err2 = mv_destroy_engine_config(config);
		if (MEDIA_VISION_ERROR_NONE != err2) {
			printf("\nERROR: Errors were occurred during engine config "
					"destroying; code %i\n", err2);
		}

		*result = NULL;

		MEDIA_VISION_FUNCTION_LEAVE();

		return err;
	}

	err = mv_destroy_source(source);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf("\nERROR: Errors were occurred during source "
				"destroying; code %i\n", err);

		int err2 = mv_destroy_engine_config(config);
		if (MEDIA_VISION_ERROR_NONE != err2) {
			printf("\nERROR: Errors were occurred during engine config "
					"destroying; code %i\n", err2);
		}

		MEDIA_VISION_FUNCTION_LEAVE();

		return err;
	}

	err = mv_destroy_engine_config(config);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf("\nERROR: Errors were occurred during engine config "
				"destroying; code %i\n", err);

		MEDIA_VISION_FUNCTION_LEAVE();

		return err;
	}

	MEDIA_VISION_FUNCTION_LEAVE();
	return err;
}

int recognize_image(const char *path_to_image,
		const char *path_to_generated_image,
		mv_image_object_h *targets,
		int number_of_targets)
{
	MEDIA_VISION_FUNCTION_ENTER();

	if (NULL == targets) {
		printf("\nYou must create at least one model for recognition\n");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	mv_source_h source;
	int err = mv_create_source(&source);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf("\nERROR: Errors were occurred during source creating; code %i\n", err);
		return err;
	}

	err = load_mv_source_from_file(path_to_image, source);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf("ERROR: image is not loaded; code %i\n", err);
		int err2 = mv_destroy_source(source);
		if (MEDIA_VISION_ERROR_NONE != err2) {
			printf("\nERROR: Errors were occurred during source destroying; "
						"code %i\n", err2);
		}
		MEDIA_VISION_FUNCTION_LEAVE();
		return err;
	}

	recognition_result result;
	result.currently_number = 0;
	if (0 < number_of_targets) {
		result.locations = malloc(sizeof(mv_quadrangle_s*) * number_of_targets);
		result.locations_size = number_of_targets;
	} else {
		result.locations = NULL;
		result.locations_size = 0;
	}

	mv_engine_config_h config;
	err = mv_create_engine_config(&config);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf("ERROR: engine configuration is not created; code %i\n", err);
		int err2 = mv_destroy_source(source);
		if (MEDIA_VISION_ERROR_NONE != err2) {
			printf("\nERROR: Errors were occurred during source destroying;"
					"code %i\n", err2);
		}
		MEDIA_VISION_FUNCTION_LEAVE();
		return err;
	}

	err = mv_image_recognize(source, targets, number_of_targets, config,
								recognized_cb, &result);

	if (MEDIA_VISION_ERROR_NONE != err) {
		printf("\nERROR: Image is not recognized; code %i\n", err);

		destroy_recognition_result(&result);

		int err2 = mv_destroy_source(source);
		if (MEDIA_VISION_ERROR_NONE != err2) {
			printf("\nERROR: Errors were occurred during source "
					"destroying; code %i\n", err2);
		}
		err2 = mv_destroy_engine_config(config);
		if (MEDIA_VISION_ERROR_NONE != err2) {
			printf("\nERROR: Errors were occurred during engine config "
					"destroying; code %i\n", err2);
		}

		MEDIA_VISION_FUNCTION_LEAVE();

		return err;
	}

	handle_recognition_result(&result, number_of_targets, source,
			path_to_generated_image);

	destroy_recognition_result(&result);

	err = mv_destroy_source(source);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf("\nERROR: Errors were occurred during source destroying; code %i\n",
				err);

		int err2 = mv_destroy_engine_config(config);
		if (MEDIA_VISION_ERROR_NONE != err2) {
			printf("\nERROR: Errors were occurred during engine config "
					"destroying; code %i\n", err2);
		}

		MEDIA_VISION_FUNCTION_LEAVE();

		return err;
	}

	err = mv_destroy_engine_config(config);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf("\nERROR: Errors were occurred during engine config destroying; "
				"code %i\n", err);

		MEDIA_VISION_FUNCTION_LEAVE();

		return err;
	}

	MEDIA_VISION_FUNCTION_LEAVE();

	return err;
}

int perform_get_confidence(mv_image_object_h target)
{
	if (NULL == target)
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;

	double confidence = 0;
	const int err = mv_image_object_get_recognition_rate(target, &confidence);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf("\nError: confidence hasn't been received with error code %i\n", err);
		return err;
	}

	printf("\nConfidence has been successfully received. Its value equal %f.\n", confidence);

	return MEDIA_VISION_ERROR_NONE;
}

int perform_set_label(mv_image_object_h target)
{
	if (NULL == target)
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;

	int label = 0;

	while (input_int("Input label (int):", INT_MIN, INT_MAX, &label) == -1)
		printf("Incorrect input! Try again.\n");

	const int err = mv_image_object_set_label(target, label);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf("\nError: the label hasn't been set with error code %i\n", err);
		return err;
	}

	printf("\nLabel has been successfully set.\n");

	return MEDIA_VISION_ERROR_NONE;
}

int perform_get_label(mv_image_object_h target)
{
	if (NULL == target)
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;

	int label = 0;
	const int err = mv_image_object_get_label(target, &label);
	if (MEDIA_VISION_ERROR_NO_DATA == err) {
		printf("\nSelected image object haven't label.\n");
		return MEDIA_VISION_ERROR_NONE;
	} else if (MEDIA_VISION_ERROR_NONE != err) {
		printf("\nError: label hasn't been received with error code %i\n", err);
		return err;
	}

	printf("\nLabel has been successfully received. Its equal %i.\n", label);

	return MEDIA_VISION_ERROR_NONE;
}

int perform_recognize(mv_image_object_h *targets, int number_of_targets)
{
	MEDIA_VISION_FUNCTION_ENTER();

	char *path_to_image = NULL;
	char *path_to_generated_image = NULL;

	while (input_string("Input file name with image for recognizing:",
							1024, &path_to_image) == -1) {
		printf("Incorrect input! Try again.\n");
	}

	while (input_string("Input file name for generated image:",
							1024, &path_to_generated_image) == -1) {
		printf("Incorrect input! Try again.\n");
	}

	const int err = recognize_image(path_to_image, path_to_generated_image, targets,
			number_of_targets);

	free(path_to_image);
	free(path_to_generated_image);

	MEDIA_VISION_FUNCTION_LEAVE();

	return err;
}

int perform_load_image_object(char **path_to_object, mv_image_object_h *result)
{
	MEDIA_VISION_FUNCTION_ENTER();

	if (NULL != (*result)) {
		mv_image_object_destroy(*result);
		*result = NULL;
	}

	while (input_string("Input file name with image object to be loaded:",
							1024, path_to_object) == -1) {
		printf("Incorrect input! Try again.\n");
	}

	int err = mv_image_object_load(result, *path_to_object);

	if (MEDIA_VISION_ERROR_NONE != err && NULL != (*result)) {
		printf("Error: object isn't loaded with error code %i\n", err);
		return err;
	}

	printf("\nObject successfully loaded\n");

	MEDIA_VISION_FUNCTION_LEAVE();
	return err;
}

int perform_save_image_object(mv_image_object_h object)
{
	MEDIA_VISION_FUNCTION_ENTER();

	int err = MEDIA_VISION_ERROR_NONE;
	char *path_to_object = NULL;

	while (input_string("Input file name to be generated for image object storing:",
							1024, &path_to_object) == -1) {
		printf("Incorrect input! Try again.\n");
	}

	err = mv_image_object_save(path_to_object, object);

	if (MEDIA_VISION_ERROR_NONE != err) {
		printf("\nError during saving the image object. Error code is %i\n", err);
		free(path_to_object);
		return err;
	}

	printf("\nObject successfully saved\n");

	free(path_to_object);

	MEDIA_VISION_FUNCTION_LEAVE();

	return err;
}

int perform_generate_image_object(mv_image_object_h *result, char **path_to_image)
{
	MEDIA_VISION_FUNCTION_ENTER();

	if (NULL == path_to_image || NULL == result)
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;

	while (input_string("Input file name with image to be analyzed:",
							1024, path_to_image) == -1) {
		printf("Incorrect input! Try again.\n");
	}

	mv_rectangle_s roi;
	const bool sel_roi = show_confirm_dialog("Select if you want to set ROI");
	if (sel_roi) {
		printf("\nInput ROI coordinates\n");
		while (input_int("Input x coordinate:", INT_MIN, INT_MAX,
				&(roi.point.x)) == -1) {
			printf("Incorrect input! Try again.\n");
		}

		while (input_int("Input y coordinate:", INT_MIN, INT_MAX,
				&(roi.point.y)) == -1) {
			printf("Incorrect input! Try again.\n");
		}

		while (input_int("Input ROI width:", INT_MIN, INT_MAX,
				&(roi.width)) == -1) {
			printf("Incorrect input! Try again.\n");
		}

		while (input_int("Input ROI height:", INT_MIN, INT_MAX,
				&(roi.height)) == -1) {
			printf("Incorrect input! Try again.\n");
		}

	}

	int err = generate_image_object_from_file(*path_to_image, sel_roi, roi, result);

	if (MEDIA_VISION_ERROR_NONE != err) {
		printf("\nError in generation image object. Error code is %i\n", err);

		if (NULL != (*result)) {
			mv_image_object_destroy(*result);
			(*result) = NULL;
		}

		return err;
	}

	printf("\nObject successfully generated\n");

	MEDIA_VISION_FUNCTION_LEAVE();

	return err;
}

int perform_clone_image_object(mv_image_object_h src, mv_image_object_h *result)
{
	int err = mv_image_object_clone(src, result);

	if (MEDIA_VISION_ERROR_NONE != err) {
		printf("\nError: object isn't cloned with error code %i\n", err);

		int err2 = mv_image_object_destroy(*result);
		if (MEDIA_VISION_ERROR_NONE != err2) {
			printf("\nERROR: Errors were occurred during image object "
					"destroying; code %i\n", err);
		}

		(*result) = NULL;

		return err;
	}

	printf("\nObject successfully cloned\n");

	return err;
}

int handle_tracking_result(
		mv_video_writer_h writer,
		mv_source_h frame,
		int frame_number,
		mv_quadrangle_s *location)
{
	unsigned char *data_buffer = NULL;
	unsigned int buffer_size = 0;
	image_data_s image_data;

	int err = mv_source_get_buffer(frame, &data_buffer, &buffer_size);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(
				"ERROR: Errors were occurred during getting buffer from the "
				"source; code %i\n",
				err);
		return err;
	}

	err = mv_source_get_width(frame, &image_data.image_width);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(
				"ERROR: Errors were occurred during getting width from the "
				"source; code %i\n",
				err);
		return err;
	}

	err = mv_source_get_height(frame, &image_data.image_height);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(
				"ERROR: Errors were occurred during getting height from the "
				"source; code %i\n",
				err);
		return err;
	}

	if (location) {
		printf(
				"Frame #%i: object is found."
				"Location: {%i, %i}; {%i, %i}; {%i, %i}; {%i, %i}.\n",
				frame_number,
				location->points[0].x,
				location->points[0].y,
				location->points[1].x,
				location->points[1].y,
				location->points[2].x,
				location->points[2].y,
				location->points[3].x,
				location->points[3].y);
		const int thickness = 2;
		const int color[] = {0, 255, 0};

		err = draw_quadrangle_on_buffer(
				*location,
				thickness,
				color,
				&image_data,
				data_buffer);
		if (MEDIA_VISION_ERROR_NONE != err) {
			printf(
					"ERROR: Errors were occurred during drawing quadrangle on "
					"the frame; code %i\n",
					err);
			return err;
		}
	} else {
		usleep(1000000);
		printf("Frame #%i: object isn't found.\n", frame_number);
	}

	err = mv_video_writer_write_frame(writer, data_buffer);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(
				"ERROR: Errors were occurred during writing frame to the "
				"result video file; code %i\n",
				err);
		return err;
	}

	return err;
}

typedef struct {
	mv_image_tracking_model_h target;
	mv_video_writer_h writer;
	int frame_number;
} tracking_cb_data;

void tracked_cb(
		mv_source_h source,
		mv_image_object_h image_object,
		mv_engine_config_h engine_cfg,
		mv_quadrangle_s *location,
		void *user_data)
{
	MEDIA_VISION_FUNCTION_ENTER();

	if (NULL == user_data)
		return;

	tracking_cb_data *cb_data = (tracking_cb_data*)user_data;

	handle_tracking_result(cb_data->writer, source, cb_data->frame_number, location);

	MEDIA_VISION_FUNCTION_LEAVE();
}

void new_frame_cb(
		char *buffer,
		unsigned int buffer_size,
		image_data_s image_data,
		void *user_data)
{
	if (NULL == user_data)
		return;

	mv_source_h frame = NULL;

#define release_resources() \
	if (frame) { \
		const int err2 = mv_destroy_source(frame); \
		if (MEDIA_VISION_ERROR_NONE != err2) { \
			printf( \
					"\nERROR: Errors were occurred during source destroying; " \
					"code %i\n", \
					err2); \
		} \
	}

	tracking_cb_data *cb_data = (tracking_cb_data*)user_data;

	++(cb_data->frame_number);

	int err = mv_create_source(&frame);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(
				"\nERROR: Errors were occurred during source creating; "
				"code %i\n",
				err);
		release_resources();
		return;
	}

	err = mv_source_fill_by_buffer(
			frame,
			buffer,
			buffer_size,
			image_data.image_width,
			image_data.image_height,
			image_data.image_colorspace);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf("ERROR: mv_source_h for frame is not filled; code %i\n", err);
		release_resources();
		return;
	}

	err = mv_image_track(
			frame,
			cb_data->target,
			NULL,
			tracked_cb,
			cb_data);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(
				"ERROR: Errors were occurred during tracking object on "
				"the video; code %i\n",
				err);
		release_resources();
		return;
	}

	release_resources()

#undef release_resources()
}

void eos_frame_cb(
		void *user_data)
{
	if (NULL == user_data) {
		printf("ERROR: eos callback can't stop tracking process.");
		return;
	}

	pthread_mutex_unlock((pthread_mutex_t*)user_data);
}

int perform_track(mv_image_tracking_model_h target)
{
	if (NULL == target) {
		printf("\nTarget is invalid. It is impossible to track of this target.\n");
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	MEDIA_VISION_FUNCTION_ENTER();

	mv_video_reader_h reader = NULL;
	mv_video_writer_h writer = NULL;
	char *path_to_video = NULL;
	char *path_to_generated_video = NULL;
	image_data_s image_data = {0};
	unsigned int fps = 0;

#define release_resources() \
	int err2 = MEDIA_VISION_ERROR_NONE; \
	if (reader) { \
		err2 = mv_destroy_video_reader(reader); \
		if (MEDIA_VISION_ERROR_NONE != err2) { \
			printf( \
					"\nERROR: Errors were occurred during video reader destroying; " \
					"code %i\n", \
					err2); \
		} \
	} \
	if (writer) { \
		err2 = mv_destroy_video_writer(writer); \
		if (MEDIA_VISION_ERROR_NONE != err2) { \
			printf( \
					"\nERROR: Errors were occurred during video writer destroying; " \
					"code %i\n", \
					err2); \
		} \
	} \
	if (path_to_video) { \
		free(path_to_video); \
	} \
	if (path_to_generated_video) { \
		free(path_to_generated_video); \
	}

	while (input_string("Input file name with video for tracking:",
							1024, &path_to_video) == -1) {
		printf("Incorrect input! Try again.\n");
	}

	while (input_string("Input file name for generated video:",
							1024, &path_to_generated_video) == -1) {
		printf("Incorrect input! Try again.\n");
	}

	int err = mv_create_video_reader(&reader);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf("\nERROR: Errors were occurred during video reader creating; "
				"code %i\n", err);
		release_resources();
		MEDIA_VISION_FUNCTION_LEAVE();
		return err;
	}

	err = mv_create_video_writer(&writer);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(
				"\nERROR: Errors were occurred during video writer creating; "
				"code %i\n",
				err);
		release_resources();
		MEDIA_VISION_FUNCTION_LEAVE();
		return err;
	}

	err = mv_video_reader_load(
		reader,
		path_to_video,
		&image_data,
		&fps);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf("\nERROR: Errors were occurred during video loading; code %i\n", err);
		release_resources();
		MEDIA_VISION_FUNCTION_LEAVE();
		return err;
	}

	printf("Receive frame metadata: wxh - %ux%u, fps - %u, format - %d\n",
		image_data.image_width, image_data.image_height, fps, image_data.image_colorspace);

	/* Temporary we accept only RGB888 */
	image_data.image_colorspace = MEDIA_VISION_COLORSPACE_RGB888;

	err = mv_video_writer_init(
			writer,
			path_to_generated_video,
			image_data,
			fps);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(
				"\nERROR: Errors were occurred during video writer initializing; "
				"code %i\n",
				err);
		release_resources();
		MEDIA_VISION_FUNCTION_LEAVE();
		return err;
	}

	tracking_cb_data cb_data;
	cb_data.target = target;
	cb_data.writer = writer;
	cb_data.frame_number = 0;
	err = mv_video_reader_set_new_sample_cb(reader, new_frame_cb, &cb_data);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(
				"\nERROR: Errors were occurred during set new frame callback; "
				"code %i\n",
				err);
		release_resources();
		MEDIA_VISION_FUNCTION_LEAVE();
		return err;
	}

	pthread_mutex_t block_during_tracking_mutex;
	pthread_mutex_init(&block_during_tracking_mutex, NULL);
	pthread_mutex_lock(&block_during_tracking_mutex);
	err = mv_video_reader_set_eos_cb(reader, eos_frame_cb, &block_during_tracking_mutex);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(
				"\nERROR: Errors were occurred during set new frame callback; "
				"code %i\n",
				err);
			release_resources();
			pthread_mutex_unlock(&block_during_tracking_mutex);
			pthread_mutex_destroy(&block_during_tracking_mutex);
			MEDIA_VISION_FUNCTION_LEAVE();
			return err;
	}

	err = mv_video_reader_start(reader);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(
				"\nERROR: Errors were occurred during video reading starts; "
				"code %i\n",
				err);
		release_resources();
		pthread_mutex_unlock(&block_during_tracking_mutex);
		pthread_mutex_destroy(&block_during_tracking_mutex);
		MEDIA_VISION_FUNCTION_LEAVE();
		return err;
	}

	pthread_mutex_lock(&block_during_tracking_mutex);
	pthread_mutex_unlock(&block_during_tracking_mutex);
	pthread_mutex_destroy(&block_during_tracking_mutex);
	release_resources();

#undef release_resources()

	printf("\nTracking process is finished\n");

	MEDIA_VISION_FUNCTION_LEAVE();

	return err;
}

int perform_save_image_tracking_model(mv_image_tracking_model_h model)
{
	MEDIA_VISION_FUNCTION_ENTER();

	int err = MEDIA_VISION_ERROR_NONE;
	char *path_to_file = NULL;

	while (input_string(
			"Input file name to be generated for image tracking model storing:",
			1024, &path_to_file) == -1) {
		printf("Incorrect input! Try again.\n");
	}

	err = mv_image_tracking_model_save(path_to_file, model);

	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(
				"\nError during saving the image tracking model. "
				"Error code is %i\n",
				err);
		free(path_to_file);
		return err;
	}

	printf("\nTracking model successfully saved\n");

	free(path_to_file);

	MEDIA_VISION_FUNCTION_LEAVE();

	return err;
}

int perform_load_image_tracking_model(
		char **path_to_file, mv_image_tracking_model_h *result)
{
	MEDIA_VISION_FUNCTION_ENTER();

	while (input_string(
			"Input file name with image tracking model to be loaded:",
			1024, path_to_file) == -1) {
			printf("Incorrect input! Try again.\n");
	}

	int err = mv_image_tracking_model_load(*path_to_file, result);

	if (MEDIA_VISION_ERROR_NONE != err && NULL != (*result)) {
		printf("Error: tracking model isn't loaded with error code %i\n", err);

		MEDIA_VISION_FUNCTION_LEAVE();

		return err;
	}

	printf("\nTracking model successfully loaded\n");

	MEDIA_VISION_FUNCTION_LEAVE();

	return err;
}

int perform_clone_image_tracking_model(
		mv_image_tracking_model_h src,
		mv_image_tracking_model_h *result)
{
	MEDIA_VISION_FUNCTION_ENTER();

	int err = mv_image_tracking_model_clone(src, result);

	if (MEDIA_VISION_ERROR_NONE != err) {
		printf("\nError: tracking model isn't cloned with error code %i\n", err);

		int err2 = mv_image_tracking_model_destroy(*result);
		if (MEDIA_VISION_ERROR_NONE != err2) {
			printf("\nERROR: Errors were occurred during tracking model "
				"destroying; code %i\n", err);
		}

		(*result) = NULL;

		MEDIA_VISION_FUNCTION_LEAVE();

		return err;
	}

	printf("\nTracking model successfully cloned\n");

	MEDIA_VISION_FUNCTION_LEAVE();

	return err;
}

int perform_refresh_image_tracking_model(mv_image_tracking_model_h target)
{
	if (NULL == target)
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;

	const int err = mv_image_tracking_model_refresh(target, NULL);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf("\nError: tracking model isn't refreshed with error code %i\n", err);
		return err;
	}

	printf("\nTracking model is successfully refreshed.\n");

	return MEDIA_VISION_ERROR_NONE;
}

void show_testing_objects(const char *title, GArray *testing_objects)
{
	printf("\n");
	int i = 0;
	if (1 > testing_objects->len) {
		printf("There are no created objects.\n");
	} else {
		printf("%s:\n", title);
		printf("-------------------------------------------------------------------------------------\n");
		for (i = 0; i < testing_objects->len; ++i) {
			testing_object_h temp = g_array_index(testing_objects, testing_object_h, i);
			if (OBJECT_TYPE_IMAGE_OBJECT == temp->object_type)
				printf("Image object ");
			else if (OBJECT_TYPE_IMAGE_TRACKING_MODEL == temp->object_type)
				printf("Image tracking model ");
			else
				printf("Unknown testing object ");

			printf("#%i. %s\n", i, temp->actual_label);
		}
		printf("-------------------------------------------------------------------------------------\n");
	}
}

int select_testing_object(GArray *testing_objects, testing_object_h *result, char *title)
{
	if (0 == testing_objects->len) {
		printf("Firstly you must create at least one testing object.\n");
		return MEDIA_VISION_ERROR_INVALID_OPERATION;
	}

	show_testing_objects(title, testing_objects);
	int sel_index = 0;
	while (input_int("Input number of element:", 0,
			testing_objects->len - 1, &sel_index) == -1) {
		printf("Incorrect input! Try again.\n");
	}
	(*result) = g_array_index(testing_objects, testing_object_h, sel_index);
	return MEDIA_VISION_ERROR_NONE;
}

int select_testing_object_index(GArray *testing_objects, guint *result_index, char *title)
{
	if (0 == testing_objects->len) {
		printf("Firstly you must create at least one testing object.\n");
		return MEDIA_VISION_ERROR_INVALID_OPERATION;
	}

	show_testing_objects(title, testing_objects);

	int sel_index = 0;
	while (input_int("Input number of element:", 0,
		testing_objects->len - 1, &sel_index) == -1) {
		printf("Incorrect input! Try again.\n");
	}
	(*result_index) = sel_index;
	return MEDIA_VISION_ERROR_NONE;
}

int add_testing_object(GArray *testing_objects, testing_object_h object)
{
	if (NULL == object)
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;

	g_array_append_val(testing_objects, object);
	return MEDIA_VISION_ERROR_NONE;
}

int remove_testing_object(GArray *testing_objects, guint index)
{
	if (index >= testing_objects->len)
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;

	g_array_remove_index(testing_objects, index);
	return MEDIA_VISION_ERROR_NONE;
}

void perform_recognition_cases(GArray *image_objects)
{
	const char *names[] = {
			"Show created set of image objects",
			"Generate new image object from source image  (mv_image_object_fill )",
			"Load existed image object from file          (mv_image_object_load)",
			"Clone existed image object                   (mv_image_object_clone)",
			"Create empty image object                    (mv_image_object_create)",
			"Save existed image object to the file        (mv_image_object_save)",
			"Remove image object from created set         (mv_image_object_destroy)",
			"Get confidence from existed image object     (mv_image_object_get_recognition_rate)",
			"Recognize all image objects on image         (mv_image_recognize)",
			"Set label for existed image object           (mv_image_set_label_of_object)",
			"Get label from existed image object          (mv_image_get_label_of_object)",
			"Back to the main menu"};

	int number_of_options = sizeof(names) / sizeof(names[0]);
	int options[number_of_options];
	int index = 0;
	for (; index < number_of_options; ++index)
		options[index] = index + 1;

	while (1) {
		int err = MEDIA_VISION_ERROR_NONE;

		int sel_opt = show_menu("Select action:", options, names, number_of_options);

		switch (sel_opt) {
		case 1: {
		/* Show created set of image objects */
			show_testing_objects("Set of image objects", image_objects);
			break;
		}
		case 2: {
		/* Generate new image object from source image (mv_image_object_fill) */
			mv_image_object_h temporary = NULL;
			char *path_to_image = NULL;

			err = perform_generate_image_object(&temporary, &path_to_image);
			if (MEDIA_VISION_ERROR_NONE != err) {
				printf("Generation failed (error code - %i)\n", err);
				if (NULL != path_to_image)
					free(path_to_image);

				break;
			}

			testing_object_h added_object;
			testing_object_create(&added_object);
			testing_object_fill(added_object, temporary,
			OBJECT_TYPE_IMAGE_OBJECT, SOURCE_TYPE_GENERATION, path_to_image);

			if (NULL != path_to_image)
				free(path_to_image);

			add_testing_object(image_objects, added_object);
			break;
		}
		case 3: {
			/* Load existed image object from file (mv_image_object_load) */
			mv_image_object_h temporary_image_object = NULL;
			char *path_to_object = NULL;

			err = perform_load_image_object(
						&path_to_object, &temporary_image_object);

			if (MEDIA_VISION_ERROR_NONE != err) {
				printf("Loading failed (error code - %i)\n", err);
				break;
			}

			testing_object_h added_object = NULL;
			testing_object_create(&added_object);
			testing_object_fill(
						added_object,
						temporary_image_object,
						OBJECT_TYPE_IMAGE_OBJECT,
						SOURCE_TYPE_LOADING,
						path_to_object);

			free(path_to_object);

			add_testing_object(image_objects, added_object);
			break;
		}
		case 4: {
			/* Clone existed image object (mv_image_object_clone) */
			if (image_objects->len <= 0) {
				printf("\nFirstly you must create at least one image object.\n");
				break;
			}

			testing_object_h temporary_testing_object = NULL;
			select_testing_object(
						image_objects,
						&temporary_testing_object,
						"Select the object you want to clone");

			mv_image_object_h temporary_image_object = NULL;
			perform_clone_image_object(
						temporary_testing_object->entity,
						&temporary_image_object);

			testing_object_h added_object = NULL;
			testing_object_create(&added_object);
			testing_object_fill(
						added_object,
						temporary_image_object,
						OBJECT_TYPE_IMAGE_OBJECT,
						SOURCE_TYPE_CLONING,
						temporary_testing_object);

			add_testing_object(image_objects, added_object);
			break;
		}
		case 5: {
			/* Create empty image object (mv_image_object_create) */
			mv_image_object_h temporary_image_object = NULL;
			int err = mv_image_object_create(&temporary_image_object);

			if (MEDIA_VISION_ERROR_NONE != err) {
				printf("ERROR: image object creation is failed with code %i\n", err);
				break;
			}

			testing_object_h added_object = NULL;
			testing_object_create(&added_object);
			testing_object_fill(
						added_object,
						temporary_image_object,
						OBJECT_TYPE_IMAGE_OBJECT,
						SOURCE_TYPE_EMPTY,
						NULL);

			add_testing_object(image_objects, added_object);
			printf("\nImage object successfully created\n");
			break;
		}
		case 6: {
			/* Save existed image object to the file (mv_image_object_save) */
			if (image_objects->len <= 0) {
				printf("\nFirstly you must create at least one image object.\n");
				break;
			}

			testing_object_h temporary_testing_object = NULL;
			select_testing_object(image_objects, &temporary_testing_object,
						"Select the object you want to save");
			perform_save_image_object(temporary_testing_object->entity);
			break;
		}
		case 7: {
			/* Remove image object from created set (mv_image_object_destroy) */
			if (image_objects->len <= 0) {
				printf("\nFirstly you must create at least one image object.\n");
				break;
			}

			guint selected_index;
			int err = select_testing_object_index(
						image_objects,
						&selected_index,
						"Select the object you want to remove");
			if (MEDIA_VISION_ERROR_NONE == err) {
				remove_testing_object(image_objects, selected_index);
				printf("\nImage object successfully removed\n");
			}
			break;
		}
		case 8: {
			/* Get confidence from existed image object (mv_image_object_get_recognition_rate) */
			if (image_objects->len <= 0) {
				printf("\nFirstly you must create at least one image object.\n");
				break;
			}

			testing_object_h temporary_testing_object = NULL;
			select_testing_object(image_objects, &temporary_testing_object,
						"Select the object from which you want getting confidence");
			perform_get_confidence(temporary_testing_object->entity);
			break;
		}
		case 9: {
			/* Recognize all image objects on image (mv_image_recognize) */
			if (image_objects->len <= 0) {
				printf("\nFirstly you must create at least one image object.\n");
				break;
			}

			mv_image_object_h *objects_pool = malloc(sizeof(mv_image_object_h) * image_objects->len);
			int index = 0;
			for (; index < image_objects->len; ++index)
				objects_pool[index] = g_array_index(image_objects, testing_object_h, index)->entity;

			perform_recognize(objects_pool, image_objects->len);
			free(objects_pool);
			break;
		}
		case 10: {
			/* Set label for existed image object (mv_image_object_set_label) */
			if (image_objects->len <= 0) {
				printf("\nFirstly you must create at least one image object.\n");
				break;
			}

			testing_object_h temporary_testing_object = NULL;
			select_testing_object(image_objects, &temporary_testing_object,
						"Select the object for which you want setting label");
			perform_set_label(temporary_testing_object->entity);
			break;
			}
		case 11: {
			/* Get label from existed image object (mv_image_object_get_label) */
			if (image_objects->len <= 0) {
				printf("\nFirstly you must create at least one image object.\n");
				break;
			}

			testing_object_h temporary_testing_object = NULL;
			select_testing_object(image_objects, &temporary_testing_object,
						"Select the object from which you want getting label");
			perform_get_label(temporary_testing_object->entity);
			break;
		}
		case 12: {
			/* Back to the main menu */
			return;
		}
		default:
			printf("Invalid option.\n");
		}
	}
}

void perform_tracking_cases(GArray *image_objects, GArray *image_tracking_models)
{
	const char *names[] = {
			"Show created set of tracking models",
			"Create empty tracking model              (mv_image_tracking_model_create)",
			"Generate model based on image object     (mv_image_tracking_model_set_target)",
			"Load existed tracking model from file    (mv_image_tracking_model_load)",
			"Clone existed tracking model             (mv_image_tracking_model_clone)",
			"Save existed tracking model to the file  (mv_image_tracking_model_save)",
			"Remove tracking model from created set   (mv_image_tracking_model_destroy)",
			"Refresh tracking model                   (mv_image_tracking_model_refresh)",
			"Track                                    (mv_image_track)",
			"Back to the main menu"};

	int number_of_options = sizeof(names) / sizeof(names[0]);
	int options[number_of_options];
	int index = 0;
	for (; index < number_of_options; ++index)
		options[index] = index + 1;

	while (1) {
		int err = MEDIA_VISION_ERROR_NONE;

		int sel_opt = show_menu("Select action:", options, names, number_of_options);

		switch (sel_opt) {
			case 1: {
				/* Show created set of tracking models */
				show_testing_objects("Set of image tracking models", image_tracking_models);
				break;
			}
			case 2: {
				/* Create empty tracking model (mv_image_tracking_model_create) */
				mv_image_tracking_model_h temporary_image_tracking_model = NULL;

				int err = mv_image_tracking_model_create(&temporary_image_tracking_model);

				if (MEDIA_VISION_ERROR_NONE != err) {
					printf("ERROR: tracking model creation is failed with code %i\n", err);
					break;
				}

				testing_object_h added_object = NULL;
				testing_object_create(&added_object);
				testing_object_fill(
						added_object,
						temporary_image_tracking_model,
						OBJECT_TYPE_IMAGE_TRACKING_MODEL,
						SOURCE_TYPE_EMPTY,
						NULL);

				add_testing_object(image_tracking_models, added_object);
				printf("\nTracking model successfully created\n");
				break;
			}
			case 3: {
				/* Generate model based on image object (mv_image_tracking_model_set_target) */
				if (image_objects->len <= 0) {
					printf("\nFirstly you must create at least one image object.\n");
					break;
				}

				mv_image_tracking_model_h temporary_image_tracking_model = NULL;
				err = mv_image_tracking_model_create(&temporary_image_tracking_model);
				if (MEDIA_VISION_ERROR_NONE != err) {
					printf("Error: tracking model isn't created with error code %i\n", err);
					break;
				}

				testing_object_h temporary_testing_object = NULL;
				select_testing_object(
						image_objects,
						&temporary_testing_object,
						"Select the image object for tracking");

				err = mv_image_tracking_model_set_target(
						(mv_image_object_h)(temporary_testing_object->entity),
						temporary_image_tracking_model);
				if (MEDIA_VISION_ERROR_NONE != err) {
					printf("Error: target isn't set with error code %i\n", err);
					break;
				}

				testing_object_h added_object = NULL;
				testing_object_create(&added_object);
				testing_object_fill(
						added_object,
						temporary_image_tracking_model,
						OBJECT_TYPE_IMAGE_TRACKING_MODEL,
						SOURCE_TYPE_GENERATION,
						temporary_testing_object);

				add_testing_object(image_tracking_models, added_object);
				printf("\nTracking model successfully generated\n");
				break;
			}
			case 4: {
				/* Load existed tracking model from file (mv_image_tracking_model_load) */
				mv_image_tracking_model_h temporary_image_tracking_model = NULL;
				char *path_to_object = NULL;

				err = perform_load_image_tracking_model(
						&path_to_object, &temporary_image_tracking_model);

				if (MEDIA_VISION_ERROR_NONE != err) {
					printf("Loading failed (error code - %i)\n", err);
					break;
				}

				testing_object_h added_object = NULL;
				testing_object_create(&added_object);
				testing_object_fill(
						added_object,
						temporary_image_tracking_model,
						OBJECT_TYPE_IMAGE_TRACKING_MODEL,
						SOURCE_TYPE_LOADING,
						path_to_object);

				free(path_to_object);

				add_testing_object(image_tracking_models, added_object);
				break;
			}
			case 5: {
				/* Clone existed tracking model (mv_image_tracking_model_clone) */
				if (image_tracking_models->len <= 0) {
					printf(
						"\nFirstly you must create at least one image "
						"tracking model.\n");
					break;
				}

				testing_object_h temporary_testing_object = NULL;
				select_testing_object(
						image_tracking_models,
						&temporary_testing_object,
						"Select the tracking model you want to clone");

				mv_image_tracking_model_h temporary_image_tracking_model = NULL;
				perform_clone_image_tracking_model(
						temporary_testing_object->entity,
						&temporary_image_tracking_model);

				testing_object_h added_object = NULL;
				testing_object_create(&added_object);
				testing_object_fill(
						added_object,
						temporary_image_tracking_model,
						OBJECT_TYPE_IMAGE_TRACKING_MODEL,
						SOURCE_TYPE_CLONING,
						temporary_testing_object);

				add_testing_object(image_tracking_models, added_object);
				break;
			}
			case 6: {
				/* Save existed tracking model to the file (mv_image_tracking_model_save) */
				if (image_tracking_models->len <= 0) {
					printf(
							"\nFirstly you must create at least one image "
							"tracking model.\n");
							break;
				}

				testing_object_h temporary_testing_object = NULL;
				select_testing_object(
						image_tracking_models,
						&temporary_testing_object,
						"Select the tracking model you want to save");

				perform_save_image_tracking_model(temporary_testing_object->entity);
				break;
			}
			case 7: {
				/* Remove tracking model from created set (mv_image_tracking_model_destroy) */
				if (image_tracking_models->len <= 0) {
					printf(
							"\nFirstly you must create at least one image "
							"tracking model.\n");
					break;
				}

				guint selected_index;
				err = select_testing_object_index(
						image_tracking_models,
						&selected_index,
						"Select the object you want to remove");

				if (MEDIA_VISION_ERROR_NONE == err) {
					remove_testing_object(image_tracking_models, selected_index);
					printf("\nTracking model successfully removed\n");
				}
				break;
			}
			case 8: {
				/* Refresh tracking model (mv_image_tracking_model_refresh) */
				if (image_tracking_models->len <= 0) {
					printf(
							"\nFirstly you must create at least one image "
							"tracking model.\n");
							break;
				}

				testing_object_h temporary_testing_object = NULL;
				select_testing_object(
						image_tracking_models,
						&temporary_testing_object,
						"Select the tracking model you want to refresh");

				perform_refresh_image_tracking_model(temporary_testing_object->entity);
				break;
			}
			case 9: {
				/* Track (mv_image_track) */
				if (image_tracking_models->len <= 0) {
					printf(
							"\nFirstly you must create at least one image "
							"tracking model.\n");
					break;
				}

				testing_object_h temporary_testing_object = NULL;
				err = select_testing_object(
						image_tracking_models,
						&temporary_testing_object,
						"Select the object which you want to track on video");

				if (MEDIA_VISION_ERROR_NONE == err)
					perform_track(temporary_testing_object->entity);

				break;
			}
			 case 10: {
				/* Back to the main menu */
				return;
			}
		}
	}
}

int main(void)
{
	LOGI("Image Media Vision Testsuite is launched.");

	GArray *image_objects = g_array_new(FALSE, FALSE, sizeof(testing_object_h));
	GArray *image_tracking_models = g_array_new(FALSE, FALSE, sizeof(testing_object_h));

	const int options[3] = { 1, 2, 3 };
	const char *names[3] = {
			"Recognition cases",
			"Tracking cases",
			"Exit" };

	mv_image_object_h current_object = NULL;

	while (1) {
		char exit = 'n';
		int sel_opt = show_menu("Select action:", options, names, 3);
		switch (sel_opt) {
		case 1:
			/* Recognition cases */
			perform_recognition_cases(image_objects);
			break;
		case 2:
			/* Tracking cases */
			perform_tracking_cases(image_objects, image_tracking_models);
			break;
		case 3:
			/* Exit */
			exit = 'y';
			break;
		default:
			printf("Invalid option.\n");
			sel_opt = 0;
			continue;
		}

		if ('y' == exit) {
			sel_opt = 0;
			const int options_last[2] = { 1, 2 };
			const char *names_last[2] = { "No", "Yes" };

			while (sel_opt == 0) {
				sel_opt = show_menu("Are you sure?",
										options_last, names_last, 2);
				switch (sel_opt) {
				case 1:
					exit = 'n';
					break;
				case 2:
					exit = 'y';
					break;
				default:
					printf("Invalid option. Back to the main menu.");
					sel_opt = 0;
					break;
				}
			}

			if ('y' == exit)
				break;
		}
	}

	guint i = 0;
	for (i = 0; i < image_objects->len; ++i) {
		testing_object_h temp = g_array_index(
				image_objects,
				testing_object_h, i);
		testing_object_destroy(&temp);
	}
	g_array_free(image_objects, TRUE);

	for (i = 0; i < image_tracking_models->len; ++i) {
		testing_object_h temp = g_array_index(
				image_tracking_models,
				testing_object_h, i);
		testing_object_destroy(&temp);
	}
	g_array_free(image_tracking_models, TRUE);

	LOGI("Image Media Vision Testsuite is closed");

	return 0;
}
