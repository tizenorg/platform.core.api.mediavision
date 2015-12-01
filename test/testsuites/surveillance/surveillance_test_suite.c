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

#include "mv_testsuite_common.h"
#include "image_helper.h"

#include "mv_log_cfg.h"

#include "mv_private.h"
#include "mv_surveillance.h"

#include <limits.h>

#define MAX_EVENTS_NUMBER 101

#define MAX_EVENT_TYPE_LEN 255

#define MIN_NUMBER_OF_ROI_POINTS 3
#define MAX_NUMBER_OF_ROI_POINTS 100

#define MIN_ROI_X_COORD 0
#define MAX_ROI_X_COORD 10000
#define MIN_ROI_Y_COORD 0
#define MAX_ROI_Y_COORD 10000

/*----------------------------------------------------*/

#define PRINT_R(MSG) printf(TEXT_RED MSG "\n" TEXT_RESET)
#define PRINT_Y(MSG) printf(TEXT_YELLOW MSG "\n" TEXT_RESET)
#define PRINT_G(MSG) printf(TEXT_GREEN MSG "\n" TEXT_RESET);

#define PRINT_E(MSG, ERR) printf(TEXT_RED MSG "\n" TEXT_RESET, ERR)
#define PRINT_W(MSG, WARN) printf(TEXT_YELLOW MSG "\n" TEXT_RESET, WARN)
#define PRINT_S(MSG, RES) printf(TEXT_GREEN MSG "\n" TEXT_RESET, RES)

/*----------------------------------------------------*/
/* static */
static mv_surveillance_event_trigger_h is_subscribed[MAX_EVENTS_NUMBER];
static int video_streams_ids[MAX_EVENTS_NUMBER];
static unsigned int trigger_id_cnt = 0;
static const int green_color[] = {0, 255, 0};
static const int red_color[] = {0, 0, 255};
static const int blue_color[] = {255, 0, 0};
static bool save_results_to_image = false;
/*----------------------------------------------------*/
/*functions*/

/* initializes is_subscribed by false */
void init_is_subscribed();

/* prints identificators of subscribed events */
void print_is_subscribed();

/* prints names of all available event types */
void print_supported_events();

/* select event name from all available event types and creates trigger handle */
int create_trigger_handle_by_event_name(mv_surveillance_event_trigger_h *handle);

/* subscribes event */
void subscribe_to_event();

/* adds ROI to event */
void add_roi_to_event(mv_surveillance_event_trigger_h event_trigger);

/* unsubscribes from event */
void unsubscribe_from_event();

/* unsubscribes from all event */
void unsubscribe_from_all_events();

/* pushes media source to event manager */
void push_source();

/* fills engine configuration for person recognized event */
bool fill_engine_cfg_person_recognized(mv_engine_config_h engine_cfg);

/* Turn on (off) saving event results to image file */
void turn_on_off_saving_to_image();

/*----------------------------------------------------*/
/* callbacks */

void detect_person_appeared_cb(
	mv_surveillance_event_trigger_h handle,
	mv_source_h source,
	int video_stream_id,
	mv_surveillance_result_h event_result,
	void *user_data);

void person_recognized_cb(
	mv_surveillance_event_trigger_h handle,
	mv_source_h source,
	int video_stream_id,
	mv_surveillance_result_h event_result,
	void *user_data);

void movement_detected_cb(
	mv_surveillance_event_trigger_h handle,
	mv_source_h source,
	int video_stream_id,
	mv_surveillance_result_h event_result,
	void *user_data);

/*----------------------------------------------------*/

int main(void)
{
	LOGI("Surveillance Media Vision Testsuite is launched.");

	PRINT_W("Maximal number of events is %d", MAX_EVENTS_NUMBER - 1);

	init_is_subscribed();

	const int options[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
	const char *names[8] = {
		"Get list of supported events",
		"Get identificators of subscribed events",
		"Subscribe to event",
		"Unsubscribe from event",
		"Unsubscribe from all events",
		"Push source",
		"Turn on (off) saving event result to image",
		"Exit"
	};

	while(1) {
		char exit = 'n';
		int sel_opt = show_menu("Select action:", options, names, 8);
		switch (sel_opt) {
		case 1: /* Get list of supported events */
			print_supported_events();
			break;
		case 2: /* Get identificators of subscribed events */
			print_is_subscribed();
			break;
		case 3: /* Subscribe to event */
			subscribe_to_event();
			break;
		case 4: /* Unsubscribe from event */
			unsubscribe_from_event();
			break;
		case 5: /* Unsubscribe from all events */
			unsubscribe_from_all_events();
			break;
		case 6: /* Push source */
			push_source();
			break;
		case 7: /* Save event results to image */
			turn_on_off_saving_to_image();
			break;
		case 8: /* Exit */
			exit = 'y';
			break;
		default:
			PRINT_R("Invalid option.");
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
					PRINT_R("Invalid option. Back to the main menu.");
					sel_opt = 0;
					break;
				}
			}

			if ('y' == exit) {
				unsubscribe_from_all_events();
				break;
			}
		}
	}

	LOGI("Surveillance Media Vision Testsuite is closed");

	return 0;
}

void init_is_subscribed()
{
	int i = 0;
	for (; i < MAX_EVENTS_NUMBER; ++i) {
		is_subscribed[i] = NULL;
		video_streams_ids[i] = -1;
	}
}

void print_is_subscribed()
{
	PRINT_Y("List of subscribed events identificators:");

	bool is_empty = true;

	int i = 0;
	for (; i < MAX_EVENTS_NUMBER; ++i) {
		if (NULL != is_subscribed[i]) {
			printf("%d ", i);
			is_empty = false;
		}
	}

	if (is_empty)
		PRINT_Y("List of subscribed events is empty");
}

static const char *EVENT_TYPES_NAMES[MAX_EVENT_TYPE_LEN] = {
	MV_SURVEILLANCE_EVENT_TYPE_MOVEMENT_DETECTED,
	MV_SURVEILLANCE_EVENT_TYPE_PERSON_APPEARED_DISAPPEARED,
	MV_SURVEILLANCE_EVENT_TYPE_PERSON_RECOGNIZED
};

static const unsigned int NUMBER_OF_TYPES = 3u;

bool foreach_event_result_value_name_cb(const char *value_name, void *user_data)
{
	if (NULL == value_name) {
		PRINT_R("\tError occurred. Value name is NULL");
		return true;
	}
	PRINT_W("%s", value_name);
	return true;
}

bool foreach_event_type_cb(const char *event_type, void *user_data)
{
	if (NULL == event_type) {
		PRINT_R("Error occurred. Event type name is NULL");
		return true;
	}

	PRINT_W("%s", event_type);
	PRINT_G("\tList of supported event result value names is:");

	const int error = mv_surveillance_foreach_event_result_name(
			event_type, foreach_event_result_value_name_cb, user_data);
	if (MEDIA_VISION_ERROR_NONE != error) {
		PRINT_E("Error occurred when trying to get value names for "
				"event named '%s'", event_type);
		return true;
	}
	return true;
}

void print_supported_events()
{
	PRINT_G("List of supported events is:");

	const int error = mv_surveillance_foreach_supported_event_type(
			foreach_event_type_cb, NULL);

	if (MEDIA_VISION_ERROR_NONE != error)
		PRINT_R("Error occurred when trying to get list of event type names \n");
}

int create_trigger_handle_by_event_name(
	mv_surveillance_event_trigger_h *handle)
{
	PRINT_G("\nSelect event type:");

	unsigned int i = 0u;
	for (; i < NUMBER_OF_TYPES; ++i)
		printf("#%d. %s\n", i, EVENT_TYPES_NAMES[i]);

	unsigned int event_id = 0u;
	while (input_size("Input event type (unsigned integer value):",
				NUMBER_OF_TYPES - 1, &event_id) == -1) {
		PRINT_R("Incorrect input! Try again.\n List of supported events is:");

		unsigned int i = 0u;
		for (; i < NUMBER_OF_TYPES; ++i)
			printf("%d\t%s\n", i, EVENT_TYPES_NAMES[i]);
	}

	const int error = mv_surveillance_event_trigger_create(
						EVENT_TYPES_NAMES[event_id], handle);
	if (MEDIA_VISION_ERROR_NONE != error) {
		PRINT_E("mv_surveillance_event_trigger_create() error!\n"
				"Error code: %i\n", error);
		return error;
	}

	return MEDIA_VISION_ERROR_NONE;
}

bool try_destroy_event_trigger(mv_surveillance_event_trigger_h trigger)
{
	const int error = mv_surveillance_event_trigger_destroy(trigger);
	if (MEDIA_VISION_ERROR_NONE != error) {
		PRINT_E("Error with code %d was occured when try to destroy "
				"event trigger.", error);
		return false;
	}
	return true;
}

void subscribe_to_event()
{
	if (++trigger_id_cnt >= MAX_EVENTS_NUMBER) {
		PRINT_R("Maximal value of event trigger id is reached. "
				"Subscription impossible");
		return;
	}

	mv_surveillance_event_trigger_h event_trigger = NULL;
	int error = create_trigger_handle_by_event_name(&event_trigger);
	if (MEDIA_VISION_ERROR_NONE != error) {
		PRINT_E("Error occurred when creating of event trigger. "
				"Error code: %i", error);
		try_destroy_event_trigger(event_trigger);
		return;
	}

	int video_stream_id = 0;

	while (input_int("Input video stream identificator (integer value):",
				INT_MIN,
				INT_MAX,
				&video_stream_id) == -1)
		PRINT_R("Incorrect input! Try again.");

	char *event_type = NULL;
	error = mv_surveillance_get_event_trigger_type(event_trigger, &event_type);
	if (MEDIA_VISION_ERROR_NONE != error) {
		PRINT_E("Error occurred when getting of event trigger type. "
				"Error code: %i", error);
		try_destroy_event_trigger(event_trigger);
		return;
	}

	if (show_confirm_dialog("Would you like to set ROI (Region Of Interest)?"))
		add_roi_to_event(event_trigger);

	if (strncmp(event_type,
			MV_SURVEILLANCE_EVENT_TYPE_PERSON_APPEARED_DISAPPEARED,
			MAX_EVENT_TYPE_LEN) == 0) {
		error = mv_surveillance_subscribe_event_trigger(
			event_trigger,
			video_stream_id,
			NULL,
			detect_person_appeared_cb,
 			NULL);
	} else if (strncmp(event_type,
					MV_SURVEILLANCE_EVENT_TYPE_PERSON_RECOGNIZED,
					MAX_EVENT_TYPE_LEN) == 0) {
			PRINT_Y("Please create and save face recognition models\n"
					"before subscribing to event. Use mv_face_test_suite.");

		mv_engine_config_h engine_cfg = NULL;
		error = mv_create_engine_config(&engine_cfg);

		if (error != MEDIA_VISION_ERROR_NONE) {
			PRINT_R("Failed to create engine configuration for event trigger.");
			try_destroy_event_trigger(event_trigger);
			free(event_type);
			return;
		}

		const bool is_filled = fill_engine_cfg_person_recognized(engine_cfg);

		if (!is_filled) {
			PRINT_R("Failed to fill engine configuration for event trigger.");
			try_destroy_event_trigger(event_trigger);
			if (mv_destroy_engine_config(engine_cfg) != MEDIA_VISION_ERROR_NONE)
				PRINT_E("Failed to destroy engine configuration for event trigger.",
						error);
			free(event_type);
			return;
		}

		error = mv_surveillance_subscribe_event_trigger(
					event_trigger,
					video_stream_id,
					engine_cfg,
					person_recognized_cb,
					NULL);

		if (error != MEDIA_VISION_ERROR_NONE) {
			PRINT_E("Subscription failed. Error code: %i.", error);
			try_destroy_event_trigger(event_trigger);
			if (mv_destroy_engine_config(engine_cfg) != MEDIA_VISION_ERROR_NONE)
				PRINT_E("Failed to destroy engine configuration for event trigger.",
						error);
			free(event_type);
			return;
		}

		if (mv_destroy_engine_config(engine_cfg) != MEDIA_VISION_ERROR_NONE)
			PRINT_E("Failed to destroy engine configuration for event trigger.",
					error);
	} else if (strncmp(event_type,
				MV_SURVEILLANCE_EVENT_TYPE_MOVEMENT_DETECTED,
				MAX_EVENT_TYPE_LEN) == 0) {
		error = mv_surveillance_subscribe_event_trigger(
					event_trigger,
					video_stream_id,
					NULL,
					movement_detected_cb,
					NULL);
	}

	free(event_type);

	if (MEDIA_VISION_ERROR_NONE != error) {
		PRINT_E("Error with code %d was occurred in subscribe event.", error);
		try_destroy_event_trigger(event_trigger);
		return;
	}

	is_subscribed[trigger_id_cnt] = event_trigger;
	video_streams_ids[trigger_id_cnt] = video_stream_id;
	PRINT_S("Event trigger %i has been successfully subscribed", trigger_id_cnt);
}

void add_roi_to_event(mv_surveillance_event_trigger_h event_trigger)
{
	int number_of_roi_points = 0;
	while (input_int("Input number of ROI points (integer value >2):",
					MIN_NUMBER_OF_ROI_POINTS,
					MAX_NUMBER_OF_ROI_POINTS,
					&number_of_roi_points) == -1)
		PRINT_R("Incorrect input! Try again.");

	mv_point_s* roi = (mv_point_s*) malloc(sizeof(mv_point_s) * number_of_roi_points);

	int x = 0;
	int y = 0;

	int i = 0;
	for (; i < number_of_roi_points; ++i) {
		printf("Point %d \n", i + 1);

		while (input_int("Input x (integer value):",
						MIN_ROI_X_COORD,
						MAX_ROI_X_COORD,
						&x) == -1)
			PRINT_R("Incorrect input! Try again.");

		while (input_int("Input y (integer value):",
						MIN_ROI_Y_COORD,
						MAX_ROI_Y_COORD,
						&y) == -1)
			PRINT_R("Incorrect input! Try again.");

		roi[i].x = x;
		roi[i].y = y;
	}

	const int error = mv_surveillance_set_event_trigger_roi(
						event_trigger,
						number_of_roi_points,
						roi);

	if (error == MEDIA_VISION_ERROR_NONE)
		PRINT_G("ROI was successfully set")
	else
		PRINT_R("Setting ROI failed. Please try again") ;

	if (roi != NULL)
		free(roi);
}

void unsubscribe_from_event()
{
	int trigger_id = 0;
	while (input_int("Input event identificator (1-100):",
					1,
					MAX_EVENTS_NUMBER - 1,
					&trigger_id) == -1)
		PRINT_R("Incorrect input! Try again.");

	mv_surveillance_event_trigger_h event_trigger = is_subscribed[trigger_id];
	if (NULL == event_trigger) {
		PRINT_E("Sorry, event trigger with %i identifier wasn't subscribed.",
				trigger_id);
		return;
	}

	const int error = mv_surveillance_unsubscribe_event_trigger(
			event_trigger,
			video_streams_ids[trigger_id]);
	if (error != MEDIA_VISION_ERROR_NONE) {
		PRINT_E("Error with code %d was occured in unsubscribe event.", error);
		return;
	}

	try_destroy_event_trigger(event_trigger);
	is_subscribed[trigger_id] = NULL;
	video_streams_ids[trigger_id] = -1;
	PRINT_S("Event with id %d was successfully unsubscribed", trigger_id);
}

void unsubscribe_from_all_events()
{
	int error = MEDIA_VISION_ERROR_NONE;
	unsigned int trigger_id = 0;
	int unsubscribed_number = 0;
	for (; trigger_id < MAX_EVENTS_NUMBER; ++trigger_id) {
		mv_surveillance_event_trigger_h event_trigger =
			is_subscribed[trigger_id];
		if (NULL != event_trigger) {
			error = mv_surveillance_unsubscribe_event_trigger(
					event_trigger,
					video_streams_ids[trigger_id]);
			if (error != MEDIA_VISION_ERROR_NONE) {
				PRINT_E("Error with code %d was occurred in unsubscribe event.",
						error);
				continue;
			}
			++unsubscribed_number;

			PRINT_S("Event with id %d was successfully unsubscribed", trigger_id);

			try_destroy_event_trigger(event_trigger);
			is_subscribed[trigger_id] = NULL;
			video_streams_ids[trigger_id] = -1;
		}
	}

	unsubscribed_number > 0 ?
	PRINT_S("%d event(s) was successfully unsubscribed", unsubscribed_number):
	PRINT_Y("\nThere are no triggers can be unsubscribed.");
}

void push_source()
{
	mv_source_h source;
	int error = mv_create_source(&source);
	if (MEDIA_VISION_ERROR_NONE != error) {
		PRINT_E("ERROR: Errors were occurred during source creating!!! Code %i" ,
				error);
		return;
	}

	char *path_to_image = NULL;

	while (input_string("Input file name with image to be analyzed:",
						1024, &path_to_image) == -1)
		PRINT_R("Incorrect input! Try again.");

	error = load_mv_source_from_file(path_to_image, source);
	if (MEDIA_VISION_ERROR_NONE != error) {
		PRINT_E("Errors were occurred during source loading, code %i", error);
		return;
	}

	if (path_to_image != NULL)
		free(path_to_image);

	int video_stream_id = 0;

	while (input_int("Input video stream identificator (integer value):",
				INT_MIN,
				INT_MAX,
				&video_stream_id) == -1)
		PRINT_R("Incorrect input! Try again.");

	error = mv_surveillance_push_source(source, video_stream_id);
	if (MEDIA_VISION_ERROR_NONE != error) {
		PRINT_E("Errors were occurred during source pushing, code %i", error);
		return;
	}

	error = mv_destroy_source(source);
	if (MEDIA_VISION_ERROR_NONE != error) {
		PRINT_E("Errors were occurred during source destroying, code %i", error);
		return;
	}

	PRINT_G("Media source was successfully pushed");
}

bool fill_engine_cfg_person_recognized(mv_engine_config_h engine_cfg)
{
	char *path_to_model = NULL;

	while (input_string("Input file name with face recognition model:",
						1024, &path_to_model) == -1)
		PRINT_R("Incorrect input! Try again.");

	const int error = mv_engine_config_set_string_attribute(
						engine_cfg,
						MV_SURVEILLANCE_FACE_RECOGNITION_MODEL_FILE_PATH,
						path_to_model);

	if (error != MEDIA_VISION_ERROR_NONE)
		PRINT_E("Setting path to face recognition model failed, code %i",
				error);
	else
		printf("\nModel path is %s \n", path_to_model);

	if (path_to_model != NULL)
		free(path_to_model);

	return true;
}

void turn_on_off_saving_to_image()
{
	save_results_to_image = !save_results_to_image;

	save_results_to_image ?
		PRINT_Y("Save event results to image files ON."):
		PRINT_Y("Save event results to image files OFF.");
}

void detect_person_appeared_cb(
	mv_surveillance_event_trigger_h handle,
	mv_source_h source,
	int video_stream_id,
	mv_surveillance_result_h event_result,
	void *user_data)
{
	PRINT_G("Person appeared / disappeared event was occured");
	if (save_results_to_image)
		PRINT_G("Output image will be saved to /tmp/person_app.jpg.\n"
				"Appeared locations - green;\n"
				"Tracked locations - blue;\n"
				"Disappeared locations - red.");

	unsigned char *out_buffer = NULL;
	unsigned int buf_size = 0;
	image_data_s image_data = { 0, 0, MEDIA_VISION_COLORSPACE_INVALID };

	if (save_results_to_image &&
		(mv_source_get_buffer(source, &out_buffer, &buf_size) ||
		 mv_source_get_width(source, &(image_data.image_width)) ||
		 mv_source_get_height(source, &(image_data.image_height)) ||
		 mv_source_get_colorspace(source, &(image_data.image_colorspace)) ||
		 out_buffer == NULL ||
		 buf_size == 0))
	{
		PRINT_R("ERROR: Creating out image is impossible.");

		return;
	}

	unsigned char *out_buffer_copy = NULL;
	if (save_results_to_image) {
		out_buffer_copy = (unsigned char *) malloc(buf_size);
		memcpy(out_buffer_copy, out_buffer, buf_size);
	}

	int number_of_appeared_persons = 0;
	int error = mv_surveillance_get_result_value(
					event_result,
					MV_SURVEILLANCE_PERSONS_APPEARED_NUMBER,
					&number_of_appeared_persons);

	if (error != MEDIA_VISION_ERROR_NONE) {
		PRINT_E("Error with code %d was occured in getting number of "
				"appeared persons.", error);
		if (out_buffer_copy != NULL)
			free(out_buffer_copy);

		return;
	}

	printf("\nNumber of appeared persons is %d \n", number_of_appeared_persons);

	mv_rectangle_s *appeared_locations =
		malloc(sizeof(mv_rectangle_s) * number_of_appeared_persons);

	error = mv_surveillance_get_result_value(
				event_result,
				MV_SURVEILLANCE_PERSONS_APPEARED_LOCATIONS,
				appeared_locations);

	if (error != MEDIA_VISION_ERROR_NONE) {
		PRINT_E("Error with code %d was occured in getting locations of "
				"appeared persons.", error);

		if (appeared_locations != NULL)
			free(appeared_locations);

		if (out_buffer_copy != NULL)
			free(out_buffer_copy);

		return;
	}

	int i = 0;
	for (; i < number_of_appeared_persons; ++i) {
		printf("Person #%d location is: x - %d, y - %d, w - %d, h - %d.\n",
				i,
				appeared_locations[i].point.x,
				appeared_locations[i].point.y,
				appeared_locations[i].width,
				appeared_locations[i].height);

		if (save_results_to_image)
			draw_rectangle_on_buffer(
						appeared_locations[i].point.x,
						appeared_locations[i].point.y,
						appeared_locations[i].point.x + appeared_locations[i].width,
						appeared_locations[i].point.y + appeared_locations[i].height,
						3,
						green_color,
						&image_data,
						out_buffer_copy);
	}

	int number_of_tracked_persons = 0;
	error = mv_surveillance_get_result_value(
				event_result,
				MV_SURVEILLANCE_PERSONS_TRACKED_NUMBER,
				&number_of_tracked_persons);

	if (error != MEDIA_VISION_ERROR_NONE) {
		PRINT_E("Error with code %d was occured in getting number of "
				"tracked persons.", error);

		if (appeared_locations != NULL)
			free(appeared_locations);

		if (out_buffer_copy != NULL)
			free(out_buffer_copy);

		return;
	}

	printf("\nNumber of tracked persons is %d \n", number_of_tracked_persons);

	mv_rectangle_s *tracked_locations =
		malloc(sizeof(mv_rectangle_s) * number_of_tracked_persons);

	error = mv_surveillance_get_result_value(
				event_result,
				MV_SURVEILLANCE_PERSONS_TRACKED_LOCATIONS,
				tracked_locations);

	if (error != MEDIA_VISION_ERROR_NONE) {
		PRINT_E("Error with code %d was occured in getting locations of "
				"tracked persons.", error);

		if (appeared_locations != NULL)
			free(appeared_locations);

		if (tracked_locations != NULL)
			free(tracked_locations);

		if (out_buffer_copy != NULL)
			free(out_buffer_copy);

		return;
	}

	for (i = 0; i < number_of_tracked_persons; ++i) {
		printf("Person #%d location is: x - %d, y - %d, w - %d, h - %d.\n",
				i,
				tracked_locations[i].point.x,
				tracked_locations[i].point.y,
				tracked_locations[i].width,
				tracked_locations[i].height);

		if (save_results_to_image)
			draw_rectangle_on_buffer(
						tracked_locations[i].point.x,
						tracked_locations[i].point.y,
						tracked_locations[i].point.x + tracked_locations[i].width,
						tracked_locations[i].point.y + tracked_locations[i].height,
						3,
						blue_color,
						&image_data,
						out_buffer_copy);
	}

	int number_of_disappeared_persons = 0;
	error = mv_surveillance_get_result_value(
				event_result,
				MV_SURVEILLANCE_PERSONS_DISAPPEARED_NUMBER,
				&number_of_disappeared_persons);

	if (error != MEDIA_VISION_ERROR_NONE) {
		PRINT_E("Error with code %d was occured in getting number of "
				"disappeared persons.", error);

		if (appeared_locations != NULL)
			free(appeared_locations);

		if (tracked_locations != NULL)
			free(tracked_locations);

		if (out_buffer_copy != NULL)
			free(out_buffer_copy);

		return;
	}

	printf("\nNumber of disappeared persons is %d \n", number_of_disappeared_persons);

	mv_rectangle_s *disappeared_locations =
		malloc(sizeof(mv_rectangle_s) * number_of_disappeared_persons);

	error = mv_surveillance_get_result_value(
				event_result,
				MV_SURVEILLANCE_PERSONS_DISAPPEARED_LOCATIONS,
				disappeared_locations);

	if (error != MEDIA_VISION_ERROR_NONE) {
		PRINT_E("Error with code %d was occured in getting locations of "
				"disappeared persons.", error);

		if (appeared_locations != NULL)
			free(appeared_locations);

		if (tracked_locations != NULL)
			free(tracked_locations);

		if (disappeared_locations != NULL)
			free(disappeared_locations);

		if (out_buffer_copy != NULL)
			free(out_buffer_copy);

		return;
	}

	for (i = 0; i < number_of_disappeared_persons; ++i) {
		printf("Person #%d location is: x - %d, y - %d, w - %d, h - %d.\n",
				i,
				disappeared_locations[i].point.x,
				disappeared_locations[i].point.y,
				disappeared_locations[i].width,
				disappeared_locations[i].height);

		if (save_results_to_image)
			draw_rectangle_on_buffer(
						disappeared_locations[i].point.x,
						disappeared_locations[i].point.y,
						disappeared_locations[i].point.x + disappeared_locations[i].width,
						disappeared_locations[i].point.y + disappeared_locations[i].height,
						3,
						red_color,
						&image_data,
						out_buffer_copy);
	}

	if (save_results_to_image)
		save_image_from_buffer("/tmp/person_app.jpg", out_buffer_copy, &image_data, 100);

	printf("\n");

	if (appeared_locations != NULL)
		free(appeared_locations);

	if (tracked_locations != NULL)
		free(tracked_locations);

	if (disappeared_locations != NULL)
		free(disappeared_locations);

	if (out_buffer_copy != NULL)
		free(out_buffer_copy);
}

void person_recognized_cb(
	mv_surveillance_event_trigger_h handle,
	mv_source_h source,
	int video_stream_id,
	mv_surveillance_result_h event_result,
	void *user_data)
{
	PRINT_G("Person recognized event was occurred");
	if (save_results_to_image)
		PRINT_G("Output image will be saved to /tmp/person_rec.jpg.\n"
				"Person recognized locations - red.");

	int number_of_persons = 0;
	int error = mv_surveillance_get_result_value(
					event_result,
					MV_SURVEILLANCE_PERSONS_RECOGNIZED_NUMBER,
					&number_of_persons);

	if (error != MEDIA_VISION_ERROR_NONE) {
		PRINT_E("Error with code %d was occured in getting number of persons.",
				error);
		return;
	}

	printf("\nNumber of persons is %d \n\n", number_of_persons);

	mv_rectangle_s *locations = malloc(sizeof(mv_rectangle_s) * number_of_persons);

	error = mv_surveillance_get_result_value(
				event_result,
				MV_SURVEILLANCE_PERSONS_RECOGNIZED_LOCATIONS,
				locations);

	if (error != MEDIA_VISION_ERROR_NONE) {
		PRINT_E("Error with code %d was occured in getting locations of persons.",
				error);

		if (locations != NULL)
			free(locations);

		return;
	}

	int *labels = malloc(sizeof(int) * number_of_persons);

	error = mv_surveillance_get_result_value(
				event_result,
				MV_SURVEILLANCE_PERSONS_RECOGNIZED_LABELS,
				labels);

	if (error != MEDIA_VISION_ERROR_NONE) {
		PRINT_E("Error with code %d was occured in getting labels of persons.",
				error);

		if (locations != NULL)
			free(locations);

		if (labels != NULL)
			free(labels);

		return;
	}

	double *confidences = malloc(sizeof(double) * number_of_persons);

	error = mv_surveillance_get_result_value(
				event_result,
				MV_SURVEILLANCE_PERSONS_RECOGNIZED_CONFIDENCES,
				confidences);

	if (error != MEDIA_VISION_ERROR_NONE) {
		PRINT_E("Error with code %d was occured in getting confidences of persons.",
				error);

		if (locations != NULL)
			free(locations);

		if (labels != NULL)
			free(labels);

		if (confidences != NULL)
			free(confidences);

		return;
	}

	unsigned char *out_buffer = NULL;
	unsigned int buf_size = 0;
	image_data_s image_data = { 0, 0, MEDIA_VISION_COLORSPACE_INVALID };

	if (save_results_to_image &&
		(mv_source_get_buffer(source, &out_buffer, &buf_size) ||
		 mv_source_get_width(source, &(image_data.image_width)) ||
		 mv_source_get_height(source, &(image_data.image_height)) ||
		 mv_source_get_colorspace(source, &(image_data.image_colorspace)) ||
		 out_buffer == NULL ||
		 buf_size == 0))
	{
		PRINT_R("ERROR: Creating out image is impossible.");

		return;
	}

	unsigned char *out_buffer_copy = NULL;
	if (save_results_to_image) {
		out_buffer_copy = (unsigned char *) malloc(buf_size);
		memcpy(out_buffer_copy, out_buffer, buf_size);
	}

	int i = 0;
	for (; i < number_of_persons; ++i) {
		printf("Person %d:\n", labels[i]);
		printf("Location is: x - %d, y - %d, w - %d, h - %d.\n",
				locations[i].point.x,
				locations[i].point.y,
				locations[i].width,
				locations[i].height);
		printf("Model confidence - %3.2f", confidences[i]);
		printf("\n");

		if (save_results_to_image)
			draw_rectangle_on_buffer(
						locations[i].point.x,
						locations[i].point.y,
						locations[i].point.x + locations[i].width,
						locations[i].point.y + locations[i].height,
						3,
						red_color,
						&image_data,
						out_buffer_copy);
	}

	printf("\n");

	if (save_results_to_image)
		save_image_from_buffer("/tmp/person_rec.jpg", out_buffer_copy, &image_data, 100);

	if (locations != NULL)
		free(locations);

	if (labels != NULL)
		free(labels);

	if (confidences != NULL)
		free(confidences);

	if (out_buffer_copy != NULL)
		free(out_buffer_copy);
}

void movement_detected_cb(
	mv_surveillance_event_trigger_h event_trigger,
	mv_source_h source,
	int video_stream_id,
	mv_surveillance_result_h event_result,
	void *user_data)
{
	PRINT_G("Movement detected event was occured");
	if (save_results_to_image)
		PRINT_G("Output image will be saved to /tmp/move_detect.jpg.\n"
				"Movement detected locations - blue.");

	int number_of_movement_regions = 0;
	int error = mv_surveillance_get_result_value(
					event_result,
					MV_SURVEILLANCE_MOVEMENT_NUMBER_OF_REGIONS,
					&number_of_movement_regions);

	if (error != MEDIA_VISION_ERROR_NONE) {
		PRINT_E("Error with code %d was occured in getting number of "
				"movement regions.", error);

		return;
	}

	printf("\nNumber of movement regions is %d \n", number_of_movement_regions);

	mv_rectangle_s *movement_regions =
		malloc(sizeof(mv_rectangle_s) * number_of_movement_regions);

	error = mv_surveillance_get_result_value(
				event_result,
				MV_SURVEILLANCE_MOVEMENT_REGIONS,
				movement_regions);

	if (error != MEDIA_VISION_ERROR_NONE) {
		PRINT_E("Error with code %d was occured in getting movement regions.",
				error);

		if (movement_regions != NULL)
			free(movement_regions);

		return;
	}

	unsigned char *out_buffer = NULL;
	unsigned int buf_size = 0;
	image_data_s image_data = { 0, 0, MEDIA_VISION_COLORSPACE_INVALID };

	if (save_results_to_image &&
		(mv_source_get_buffer(source, &out_buffer, &buf_size) ||
		 mv_source_get_width(source, &(image_data.image_width)) ||
		 mv_source_get_height(source, &(image_data.image_height)) ||
		 mv_source_get_colorspace(source, &(image_data.image_colorspace)) ||
		 out_buffer == NULL ||
		 buf_size == 0))
	{
		PRINT_R("ERROR: Creating out image is impossible.");

		if (movement_regions != NULL)
			free(movement_regions);

		return;
	}

	unsigned char *out_buffer_copy = NULL;
	if (save_results_to_image) {
		out_buffer_copy = (unsigned char *) malloc(buf_size);
		memcpy(out_buffer_copy, out_buffer, buf_size);
	}

	int i = 0;
	for (; i < number_of_movement_regions; ++i) {
		printf("Movement #%d region is: x - %d, y - %d, w - %d, h - %d.\n",
				i,
				movement_regions[i].point.x,
				movement_regions[i].point.y,
				movement_regions[i].width,
				movement_regions[i].height);

		if (save_results_to_image)
			draw_rectangle_on_buffer(
						movement_regions[i].point.x,
						movement_regions[i].point.y,
						movement_regions[i].point.x + movement_regions[i].width,
						movement_regions[i].point.y + movement_regions[i].height,
						3,
						blue_color,
						&image_data,
						out_buffer_copy);
	}

	printf("\n");

	if (save_results_to_image)
		save_image_from_buffer("/tmp/move_detect.jpg", out_buffer_copy, &image_data, 100);

	if (movement_regions != NULL)
		free(movement_regions);

	if (out_buffer_copy != NULL)
		free(out_buffer_copy);
}
