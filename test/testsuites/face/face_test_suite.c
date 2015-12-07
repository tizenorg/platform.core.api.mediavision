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

#include <mv_common.h>
#include <mv_face.h>

#include <mv_testsuite_common.h>

#include <image_helper.h>
#include <mv_video_helper.h>

#include <mv_log_cfg.h>

#include "pthread.h"

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>

#define MIN_ALLOWED_LABEL 0
#define MAX_ALLOWED_LABEL 100

static bool Perform_eye_condition_recognize = false;
static bool Perform_facial_expression_recognize = false;

void eye_condition_cb(
		mv_source_h source,
		mv_engine_config_h engine_cfg,
		mv_rectangle_s face_location,
		mv_face_eye_condition_e eye_condition,
		void *user_data)
{
	switch (eye_condition) {
	case MV_FACE_EYES_NOT_FOUND:
		printf("Eyes not found");
		break;
	case MV_FACE_EYES_OPEN:
		printf("Eyes are open");
		break;
	case MV_FACE_EYES_CLOSED:
		printf("Eyes are closed");
		break;
	}
}

void face_expression_cb(
		mv_source_h source,
		mv_engine_config_h engine_cfg,
		mv_rectangle_s face_location,
		mv_face_facial_expression_e facial_expression,
		void *user_data)
{
	switch (facial_expression) {
	case MV_FACE_NEUTRAL:
		printf("Face expression is neutral");
		break;
	case MV_FACE_SMILE:
		printf("Face expression is smiling");
		break;
	case MV_FACE_UNKNOWN:
		printf("Face expression isn't recognized");
		break;
	}
}

void on_face_detected_cb(
		mv_source_h source,
		mv_engine_config_h engine_cfg,
		mv_rectangle_s *faces_locations,
		int number_of_faces,
		void *user_data)
{
	printf("%i faces were detected on the image.\n", number_of_faces);
	if (number_of_faces > 0) {
		int is_source_data_loaded = 0;

		char *file_name = NULL;
		unsigned char *out_buffer = NULL;
		unsigned int buf_size = 0;
		image_data_s image_data = { 0, 0, MEDIA_VISION_COLORSPACE_INVALID };
		if (MEDIA_VISION_ERROR_NONE != mv_source_get_buffer(source, &out_buffer, &buf_size) ||
			MEDIA_VISION_ERROR_NONE != mv_source_get_width(source, &(image_data.image_width)) ||
			MEDIA_VISION_ERROR_NONE != mv_source_get_height(source, &(image_data.image_height)) ||
			MEDIA_VISION_ERROR_NONE != mv_source_get_colorspace(source, &(image_data.image_colorspace)) ||
			user_data == NULL) {
			printf("ERROR: Creating out image is impossible.\n");
		} else {
			file_name = (char*)user_data;
			is_source_data_loaded = 1;
		}

		int i = 0;
		for (i = 0; i < number_of_faces; ++i) {
			printf("\Face %i : x - %i, y - %i, width - %i, height - %i ", i,
			faces_locations[i].point.x, faces_locations[i].point.y,
					faces_locations[i].width, faces_locations[i].height);

			if (Perform_eye_condition_recognize) {
				if (MEDIA_VISION_ERROR_NONE != mv_face_eye_condition_recognize(
													source,
													engine_cfg,
													faces_locations[i],
													eye_condition_cb,
													user_data)) {
					printf(TEXT_RED "\nEye condition recognition for %i face failed"
							TEXT_RESET "\n", i);
				}
			}

			if (Perform_facial_expression_recognize) {
				if (MEDIA_VISION_ERROR_NONE != mv_face_facial_expression_recognize(
													source,
													engine_cfg,
													faces_locations[i],
													face_expression_cb,
													user_data)) {
					printf(TEXT_RED "\nFacial expression recognition for %i "
							"face failed" TEXT_RESET "\n", i);
				}
			}

			printf("\n");

			if ((is_source_data_loaded == 1) && !Perform_eye_condition_recognize) {
				const int rectangle_thickness = 3;
				const int drawing_color[] = {255, 0, 0};
				if (MEDIA_VISION_ERROR_NONE != draw_rectangle_on_buffer(
						faces_locations[i].point.x,
						faces_locations[i].point.y,
						faces_locations[i].point.x + faces_locations[i].width,
						faces_locations[i].point.y + faces_locations[i].height,
						rectangle_thickness,
						drawing_color,
						&image_data,
						out_buffer)) {
					continue;
				}
			}
		}

		if (!Perform_eye_condition_recognize) {
			if (file_name != NULL &&
				MEDIA_VISION_ERROR_NONE == save_image_from_buffer(
												file_name,
												out_buffer,
												&image_data,
												100)) {
				printf("Image was generated as %s\n", file_name);
			} else {
				printf("ERROR: Failed to generate output file. Check file name and permissions. \n");
			}
		}

		printf("\n");
	}
}

void on_face_recognized_cb(
		mv_source_h source,
		mv_face_recognition_model_h recognition_model,
		mv_engine_config_h engine_cfg,
		mv_rectangle_s *face_location,
		const int *face_label,
		double confidence,
		void *user_data)
{
	if (NULL == face_location) {
		printf(TEXT_YELLOW "No faces were recognized in the source"
				TEXT_RESET "\n");
	} else {
		printf(TEXT_GREEN "Face labeled %i was recognized in the source with "
						"recognition confidence of %.2f"
						TEXT_RESET "\n", *face_label, confidence);
	}
}

int perform_detect()
{
	char *in_file_name = NULL;
	char *out_file_name = NULL;

	/* 1. Loading media source */
	while (input_string("Input file name to be analyzed:", 1024, &(in_file_name)) == -1)
		printf("Incorrect input! Try again.\n");

	mv_source_h source;
	int err = mv_create_source(&source);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(TEXT_RED
				"ERROR: Errors were occurred during creating the source!!! code: %i"
				TEXT_RESET "\n", err);

		free(in_file_name);

		return err;
	}

	err = load_mv_source_from_file(in_file_name, source);
	if (MEDIA_VISION_ERROR_NONE != err) {
		const int err2 = mv_destroy_source(source);
		if (MEDIA_VISION_ERROR_NONE != err2) {
			printf(TEXT_RED
					"ERROR: Errors were occurred during destroying the source!!! code: %i"
					TEXT_RESET "\n", err2);

			free(in_file_name);

			return err2;
		}

		free(in_file_name);

		return err;
	}

	free(in_file_name);

	/* 2. Select output file to be generated */
	while (input_string("Input file name to be generated:", 1024, &(out_file_name)) == -1)
		printf("Incorrect input! Try again.\n");

	/* 3. Select Haar cascade */
	const int options[3] = { 1, 2, 3 };
	const char *names[3] = { "haarcascade_frontalface_alt.xml",
								"haarcascade_frontalface_alt2.xml",
								"haarcascade_frontalface_alt_tree.xml"};

	const int haarcascade = show_menu("Select Haarcascade:", options, names, 3);

	mv_engine_config_h eng_config;
	err = mv_create_engine_config(&eng_config);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(TEXT_RED
				"ERROR: Errors were occurred during creating the engine config!!! code: %i"
				TEXT_RESET "\n", err);

		free(out_file_name);

		return err;
	}

	switch (haarcascade) {
	case 1:
		mv_engine_config_set_string_attribute(
						eng_config,
						MV_FACE_DETECTION_MODEL_FILE_PATH,
						"/usr/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml");
		break;
	case 2:
		mv_engine_config_set_string_attribute(
						eng_config,
						MV_FACE_DETECTION_MODEL_FILE_PATH,
						"/usr/share/OpenCV/haarcascades/haarcascade_frontalface_alt2.xml");
		break;
	case 3:
		mv_engine_config_set_string_attribute(
						eng_config,
						MV_FACE_DETECTION_MODEL_FILE_PATH,
						"/usr/share/OpenCV/haarcascades/haarcascade_frontalface_alt_tree.xml");
		break;
	default:
		printf(TEXT_YELLOW "Default Haar cascade was set.\n" TEXT_RESET);
	}

	/* 4. Perform detect */
	err = mv_face_detect(source, eng_config, on_face_detected_cb, out_file_name);

	free(out_file_name);

	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(TEXT_RED
				"ERROR: Errors were occurred during face detection!!! code: %i"
				TEXT_RESET "\n", err);

		int err2 = mv_destroy_source(source);
		if (MEDIA_VISION_ERROR_NONE != err2) {
			printf(TEXT_RED
					"ERROR: Errors were occurred during destroying the source!!! code: %i"
					TEXT_RESET "\n", err2);
			return err2;
		}

		err2 = mv_destroy_engine_config(eng_config);
		if (MEDIA_VISION_ERROR_NONE != err2) {
			printf(TEXT_RED
					"ERROR: Errors were occurred during destroying the engine config!!! code: %i"
					TEXT_RESET "\n", err2);
			return err2;
		}

		return err;
	}

	err = mv_destroy_source(source);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(TEXT_RED
				"ERROR: Errors were occurred during destroying the source!!! code: %i"
				TEXT_RESET "\n", err);
		return err;
	}

	err = mv_destroy_engine_config(eng_config);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(TEXT_RED
				"ERROR: Errors were occurred during destroying the engine config!!! code: %i"
				TEXT_RESET "\n", err);
		return err;
	}

	return err;
}

int perform_mv_face_recognize(mv_face_recognition_model_h model)
{
	char *in_file_name = NULL;

	mv_source_h source = NULL;
	int err = mv_create_source(&source);

	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(TEXT_RED
				"ERROR: Errors were occurred during creating the source!!! code: %i"
				TEXT_RESET "\n", err);
		return err;
	}

	printf(TEXT_GREEN "HINT:" TEXT_RESET "\n"
			TEXT_YELLOW "To achieve appropriate accuracy of recognition,\n"
						"choose images with only faces. I.e. face has to cover\n"
						"approximately 95-100%% of the image (passport photos\n"
						"are the best example :)). Note that if this value is\n"
						"less than 95%, accuracy can be significantly reduced.\n"
						"In real code such images can be achieved by cropping\n"
						"faces from images with face detection functionality.\n"
			TEXT_RESET);
	while (-1 == input_string(
						"Input file name with the face to be recognized:",
						1024,
						&(in_file_name))) {
		printf(TEXT_RED "Incorrect input! Try again." TEXT_RESET "\n");
	}

	err = load_mv_source_from_file(in_file_name, source);

	if (MEDIA_VISION_ERROR_NONE != err) {
		free(in_file_name);

		const int err2 = mv_destroy_source(source);
		if (MEDIA_VISION_ERROR_NONE != err2) {
			printf(TEXT_RED
					"ERROR: Errors were occurred during destroying the source!!! code: %i"
					TEXT_RESET "\n", err2);
			return err2;
		}

		return err;
	}

	err = mv_face_recognize(source, model, NULL, NULL, on_face_recognized_cb, NULL);

	if (MEDIA_VISION_ERROR_NONE != err) {
		free(in_file_name);

		printf(TEXT_RED
				"ERROR: Errors were occurred during face recognition!!! code: %i"
				TEXT_RESET "\n", err);

		int err2 = mv_destroy_source(source);
		if (MEDIA_VISION_ERROR_NONE != err2) {
			printf(TEXT_RED
					"ERROR: Errors were occurred during destroying the source!!! code: %i"
					TEXT_RESET "\n", err2);
			return err2;
		}

		return err;
	}

	free(in_file_name);

	return err;
}

int add_single_example(
		mv_face_recognition_model_h model, const char *in_file_name,
		mv_rectangle_s *roi, int *face_label)
{
	mv_source_h source;
	int err = mv_create_source(&source);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(TEXT_RED
				"ERROR: Errors were occurred during creating the source!!! code: %i"
				TEXT_RESET "\n", err);

		return err;
	}

	err = load_mv_source_from_file(in_file_name, source);
	if (MEDIA_VISION_ERROR_NONE != err) {
		const int err2 = mv_destroy_source(source);
		if (MEDIA_VISION_ERROR_NONE != err2) {
			printf(TEXT_RED
					"ERROR: Errors were occurred during destroying the source!!! code: %i"
					TEXT_RESET "\n", err2);
			return err2;
		}

		return err;
	}

	if (NULL != roi && !show_confirm_dialog("Do you want to use full image?")) {
		printf(TEXT_YELLOW "Specify the ROI as rectangle where face is located.\n"
							"Use negative values if you want to check correctness\n"
							"of error handling.\n"
				TEXT_RESET);

		while (-1 == input_int("Specify top left ROI x coordinate:",
				INT_MIN, INT_MAX, &(roi->point.x))) {
			printf("Incorrect input! Try again.\n");
		}

		while (-1 == input_int("Specify top left ROI y coordinate:",
				INT_MIN, INT_MAX, &(roi->point.y))) {
			printf("Incorrect input! Try again.\n");
		}

		while (-1 == input_int("Specify top left ROI width:",
				INT_MIN, INT_MAX, &(roi->width))) {
			printf("Incorrect input! Try again.\n");
		}

		while (-1 == input_int("Specify top left ROI height:",
				INT_MIN, INT_MAX, &(roi->height))) {
			printf("Incorrect input! Try again.\n");
		}
	} else {
		roi = NULL;
	}

	int real_label = 0;
	if (NULL == face_label) {
		printf(TEXT_YELLOW "Also, you has to assign label for the face in the\n"
							"image. You has assign the same labels for the same\n"
							"persons. For example, always assign label '1' for\n"
							"images with Alice's face; label '2' for Bob's faces,\n"
							"'3' for Ann's faces and so on...\n"
				TEXT_RESET);

		face_label = &real_label;
		while (-1 == input_int("Specify label as integer:",
								MIN_ALLOWED_LABEL,
								MAX_ALLOWED_LABEL,
								face_label)) {
			printf("Incorrect input! You can use %i-%i labels only. Try again.\n",
					MIN_ALLOWED_LABEL,
					MAX_ALLOWED_LABEL);
			}
	}

	err = mv_face_recognition_model_add(source, model, roi, *face_label);

	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(TEXT_RED
				"ERROR: Errors were occurred during adding the sample image from "
				"[%s] to the face recognition model!!! code: %i"
				TEXT_RESET "\n", in_file_name, err);
	}

	const int err2 = mv_destroy_source(source);
	if (MEDIA_VISION_ERROR_NONE != err2) {
		printf(TEXT_RED
				"ERROR: Errors were occurred during destroying the source!!! code: %i"
				TEXT_RESET "\n", err2);
	}

	return err;
}

int perform_mv_face_recognition_model_add_face_example(
		mv_face_recognition_model_h model,
		notification_type_e *notification_type)
{
	char *in_file_name = NULL;

	printf(TEXT_GREEN "HINT:" TEXT_RESET "\n"
			TEXT_YELLOW "To achieve appropriate accuracy of recognition,\n"
						"choose images with only faces. I.e. face has to cover\n"
						"approximately 95-100%% of the image (passport photos\n"
						"are the best example :)). Note that if this value is\n"
						"less than 95%, accuracy can be significantly reduced.\n"
						"In real code such images can be achieved by cropping\n"
						"faces from images with face detection functionality.\n"
			TEXT_RESET);

	const bool from_dir = show_confirm_dialog("Do add images from directory?");
	const char *input_path_msg =
			from_dir ? "Input path to the directory with the face images to be "
						"loaded to the model:"
						: "Input file name with the face to be loaded to the model:";

	while (-1 == input_string(input_path_msg, 1024, &(in_file_name)))
		printf(TEXT_RED "Incorrect input! Try again." TEXT_RESET "\n");

	int err = MEDIA_VISION_ERROR_NONE;

	if (from_dir) {
		*notification_type = FAIL_OR_DONE;
		int face_label = 0;
		while (-1 == input_int("Specify label as integer:",
								MIN_ALLOWED_LABEL,
								MAX_ALLOWED_LABEL,
								&face_label)) {
			printf("Incorrect input! You can use %i-%i labels only. Try again.\n",
					MIN_ALLOWED_LABEL,
					MAX_ALLOWED_LABEL);
		}

		DIR *dir;
		struct dirent *ent;
		if ((dir = opendir(in_file_name)) != NULL) {
			char file_path[1024] = "";

			/* Traverses all the files and directories within source directory */
			while ((ent = readdir(dir)) != NULL) {
				/* Determine current entry name */
				const char *file_name = ent->d_name;

				/* If current entry is directory, or hidden object, skip the step: */
				if (file_name[0] == '.')
					continue;

				snprintf(file_path, 1024, "%s/%s", in_file_name, file_name);
				err = add_single_example(model, file_path, NULL, &face_label);

				if (MEDIA_VISION_ERROR_NONE != err) {
					printf(TEXT_RED "Failed to add example from %s. "
									"Error code: %i\n" TEXT_RESET,
									file_path, err);
				} else {
					printf(TEXT_GREEN "Example labeled [%i] added from " TEXT_RESET
							TEXT_YELLOW "%s\n" TEXT_RESET, face_label, file_path);
				}
			}

			closedir(dir);
		} else {
			printf(TEXT_RED "Can't read from specified directory (%s)\n"
					TEXT_RESET, in_file_name);
		}
	} else {
		*notification_type = FAIL_OR_SUCCESSS;
		mv_rectangle_s roi;
		err = add_single_example(model, in_file_name, &roi, NULL);
	}

	free(in_file_name);

	return err;
}

int perform_mv_face_recognition_model_reset_face_examples(
		mv_face_recognition_model_h model,
		bool full_reset)
{
	printf(TEXT_GREEN "HINT:" TEXT_RESET "\n"
			TEXT_YELLOW "Reset of the examples will affect only examples has\n"
						"been collected via mv_face_recognition_model_add()\n"
						"function calls (i.e. through 'Add image example' menu\n"
						"item). Previously learned model will be not affected,\n"
						"so it is possible to recognize faces with this model\n"
						"after examples reset. Reset of the examples can be\n"
						"useful to erase a class of faces (i.e. all examples\n"
						"related to this class) before learning the model.\n"
						"Or, if it is needed to reset all collected previously\n"
						"examples as an alternative to the creating the new\n"
						"model.\n"
			TEXT_RESET);

	int err = MEDIA_VISION_ERROR_NONE;

	if (full_reset) {
		err = mv_face_recognition_model_reset(model, NULL);
		if (MEDIA_VISION_ERROR_NONE != err) {
			printf(TEXT_RED
					"ERROR: Errors were occurred during reset of all examples!!!"
					" code: %i" TEXT_RESET "\n", err);
			return err;
		}
	} else {
		int reset_label = 0;

		while (-1 == input_int("Specify label for the examples to be reset:",
								MIN_ALLOWED_LABEL,
								MAX_ALLOWED_LABEL,
								&reset_label)) {
			printf("Incorrect input! You can use %i-%i labels only. Try again.\n",
					MIN_ALLOWED_LABEL,
					MAX_ALLOWED_LABEL);
		}

		err = mv_face_recognition_model_reset(model, &reset_label);

		if (MEDIA_VISION_ERROR_NONE != err) {
			printf(TEXT_RED
					"ERROR: Errors were occurred during reset of examples labeled"
					" with  %i!!! code: %i" TEXT_RESET "\n", reset_label, err);
			return err;
		}
	}

	return err;
}

int perform_mv_face_recognition_model_save(mv_face_recognition_model_h model)
{
	char *out_file_name = NULL;

	while (input_string("Input file name to save the model:",
						1024, &(out_file_name)) == -1) {
		printf(TEXT_RED "Incorrect input! Try again." TEXT_RESET "\n");
	}

	const int err = mv_face_recognition_model_save(out_file_name, model);

	free(out_file_name);

	return err;
}

int perform_mv_face_recognition_model_load(mv_face_recognition_model_h *model)
{
	char *in_file_name = NULL;

	while (input_string("Input file name to load model from:",
						1024, &(in_file_name)) == -1) {
		printf(TEXT_RED "Incorrect input! Try again." TEXT_RESET "\n");
	}

	const int err = mv_face_recognition_model_load(in_file_name, model);

	free(in_file_name);

	return err;
}

int perform_mv_face_recognition_model_clone(
		mv_face_recognition_model_h model_to_clone)
{
	int err = MEDIA_VISION_ERROR_NONE;

	mv_face_recognition_model_h cloned_model = NULL;

	printf(TEXT_GREEN "Perform clone of the recognition model..."
			TEXT_RESET "\n");

	err = mv_face_recognition_model_clone(model_to_clone, &cloned_model);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(TEXT_RED "Errors were occurred during model clone. Error code %i"
				TEXT_RESET "\n", err);
		return err;
	}

	printf(TEXT_YELLOW "Model cloning is done." TEXT_RESET "\n");

	if (show_confirm_dialog("Save " TEXT_YELLOW "source model" TEXT_RESET
							" to file?")) {
		const int serr = perform_mv_face_recognition_model_save(model_to_clone);
		if (MEDIA_VISION_ERROR_NONE != serr) {
			printf(TEXT_RED
					"Errors were occurred when trying to save "
					"source model to file. Error code %i" TEXT_RESET "\n", serr);
		}
	}

	if (show_confirm_dialog("Save " TEXT_YELLOW "destination model" TEXT_RESET
							" to file?")) {
		const int serr = perform_mv_face_recognition_model_save(cloned_model);
		if (MEDIA_VISION_ERROR_NONE != serr) {
			printf(TEXT_RED
					"Errors were occurred when trying to save destination model "
					"to file. Error code %i" TEXT_RESET "\n", serr);
		}
	}

	if (cloned_model) {
		const int dest_err = mv_face_recognition_model_destroy(cloned_model);
		if (MEDIA_VISION_ERROR_NONE != dest_err) {
			printf(TEXT_RED
				"Errors were occurred when destroying destination model ."
				"Error code %i" TEXT_RESET "\n", dest_err);
		}
	}

	return err;
}

int perform_mv_face_recognition_model_learn(mv_face_recognition_model_h model)
{
	printf(TEXT_YELLOW "Learning the model has to be performed after\n"
						"adding some amount of examples to the model.\n"
						"If you learn without examples, you will get useless\n"
						"model, which will be unavailable to recognize. Anyway,\n"
						"you can add examples and launch this method again to\n"
						"get the appropriate recognition model suitable for\n"
						"recognition."
			TEXT_RESET "\n");

	printf(TEXT_GREEN "Start learning process..." TEXT_RESET "\n");

	const int err = mv_face_recognition_model_learn(NULL, model);

	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(TEXT_RED "Learning the model failed. Error code: %i. "
				"But you still can test with this model.\n"
				TEXT_RESET "\n", err);
	} else {
		printf(TEXT_YELLOW "Recognition model has been learned."
				TEXT_RESET "\n");
	}

	return err;
}

int perform_mv_face_recognition_model_query_labels(mv_face_recognition_model_h model)
{
	int *learned_labels = NULL;
	int learned_labels_n = 0;

	const int err = mv_face_recognition_model_query_labels(model, &learned_labels, &learned_labels_n);

	if (MEDIA_VISION_ERROR_NONE != err) {
		free(learned_labels);

		return err;
	}

	int i = 0;
	printf(TEXT_YELLOW "Recognition model had been learned for the following labels: "
			TEXT_RESET "\n" TEXT_GREEN);
	for (i = 0; i < learned_labels_n; ++i)
		printf("%i, ", learned_labels[i]);

	printf(TEXT_RESET "\n");

	free(learned_labels);

	return MEDIA_VISION_ERROR_NONE;
}

static int TP = 0;
static int FP = 0;
static int TN = 0;
static int FN = 0;
static double THRESHOLD = 0.75;

void evaluation_cb(
		mv_source_h source,
		mv_face_recognition_model_h recognition_model,
		mv_engine_config_h engine_cfg,
		mv_rectangle_s *face_location,
		const int *face_label,
		double confidence,
		void *user_data)
{
	if (NULL != user_data) {
		const int real_label = *((int*)user_data);
		const int rec_label = (NULL != face_label ? *face_label : -1);
		if (real_label == -1) {
			confidence >= THRESHOLD ? ++FP : ++TN;
		} else if (real_label == rec_label) {
			confidence >= THRESHOLD ? ++TP : ++FN;
		} else {
			if (confidence >= THRESHOLD)
				++FP;

			++FN;
		}
	}
}

int perform_model_evaluation(mv_face_recognition_model_h model)
{
	int *learned_labels = NULL;
	int learned_labels_n = 0;

	mv_face_recognition_model_query_labels(model, &learned_labels, &learned_labels_n);

	int i = 0;

	printf(TEXT_YELLOW "Evaluating model had been learned for the following labels: "
			TEXT_RESET "\n" TEXT_GREEN);
	for (i = 0; i < learned_labels_n; ++i)
		printf("%i, ", learned_labels[i]);

	printf(TEXT_RESET "\n");

	/* 100 directories are allowed: */
	const int max_dir_allowed = 100;
	char (*directories)[1024] = malloc(sizeof *directories * max_dir_allowed);
	int labels[max_dir_allowed];
	int unique_checks[MAX_ALLOWED_LABEL + 1];
	for (i = 0; i < MAX_ALLOWED_LABEL + 1; ++i)
		unique_checks[i] = 0;

	int dir_n = 0;
	int label_count = 0;
	while (show_confirm_dialog("Add test images directory?") &&
		dir_n < max_dir_allowed) {
		char *in_file_name = NULL;
		while (-1 == input_string("Specify path to the test images directory:", 1024, &(in_file_name)))
			printf(TEXT_RED "Incorrect input! Try again." TEXT_RESET "\n");

		DIR *dir;
		if ((dir = opendir(in_file_name)) == NULL) {
			printf(TEXT_RED "Incorrect input! Directory %s can't be read.\n"
					TEXT_RESET, in_file_name);
			free(in_file_name);
			in_file_name = NULL;
			continue;
		} else {
			closedir(dir);
		}

		int face_label = 0;
		if (-1 == input_int("Specify label as integer:",
								MIN_ALLOWED_LABEL,
								MAX_ALLOWED_LABEL,
								&face_label)) {
			printf(TEXT_RED "Incorrect input! You can use %i-%i labels only.\n"
					TEXT_RESET,
					MIN_ALLOWED_LABEL,
					MAX_ALLOWED_LABEL);
			free(in_file_name);
			in_file_name = NULL;
			continue;
		}

		bool known_label = false;
		for (i = 0; i < learned_labels_n; ++i) {
			if (learned_labels[i] == face_label) {
				known_label = true;
				break;
			}
		}

		if (!known_label) {
			printf(TEXT_YELLOW "Recognition model didn't learn with specified label.\n"
								"Images will be marked as unknown (-1)\n" TEXT_RESET);
		}

		labels[dir_n] = known_label ? face_label : -1;
		snprintf(directories[dir_n], 1024, "%s", in_file_name);
		label_count += (0 == unique_checks[face_label] ? 1 : 0);
		if (labels[dir_n] >= 0)
			unique_checks[labels[dir_n]] += 1;

		free(in_file_name);

		++dir_n;

		printf(TEXT_GREEN "Current test set for %i unique labels:\n" TEXT_RESET, label_count);
		for (i = 0; i < dir_n; ++i)
			printf(TEXT_YELLOW "Label %i: " TEXT_RESET "%s\n", labels[i], directories[i]);
	}

	free(learned_labels);

	int rec_threshold = 0;
	while (-1 == input_int("Specify recognition confidence threshold (0-100%):", 0, 100, &rec_threshold))
		printf(TEXT_RED "Incorrect input! You can use 0-100 values only." TEXT_RESET "\n");

	THRESHOLD = (double) rec_threshold / 100.0;

	TP = 0;
	FP = 0;
	TN = 0;
	FN = 0;

	mv_source_h source = NULL;
	int err = mv_create_source(&source);
	if (MEDIA_VISION_ERROR_NONE != err) {
		free(directories);
		printf(TEXT_RED
				"ERROR: Errors were occurred during creating the source!!! code: %i"
				TEXT_RESET "\n", err);
		return err;
	}

	for (i = 0; i < dir_n; ++i) {
		DIR *dir;
		struct dirent *ent;
		printf("Processing %s...\n", directories[i]);
		if ((dir = opendir(directories[i])) != NULL) {
			char file_path[1024] = "";

			/* Traverses all the files and directories within source directory */
			while ((ent = readdir(dir)) != NULL) {
				/* Determine current entry name */
				const char *file_name = ent->d_name;

				/* If current entry is directory, or hidden object, skip the step: */
				if (file_name[0] == '.')
					continue;

				snprintf(file_path, 1024, "%s/%s", directories[i], file_name);
				err = load_mv_source_from_file(file_path, source);
				if (MEDIA_VISION_ERROR_NONE != err) {
					printf(TEXT_RED "Failed to test on example from %s. "
									"Example will not affect the evaluation. "
									"Error code: %i.\n" TEXT_RESET,
									file_path, err);
				} else {
					err = mv_face_recognize(source, model, NULL, NULL, evaluation_cb, &(labels[i]));
				if (MEDIA_VISION_ERROR_NONE != err) {
					printf(TEXT_RED "Failed to recognize on example from %s. "
										"Example will not affect the evaluation. "
										"Error code: %i\n" TEXT_RESET,
										file_path, err);
					}
				}
			}

			closedir(dir);
		} else {
			printf(TEXT_RED "Can't read from directory [%s]\n"
					TEXT_RESET, directories[i]);
		}
	}

	int err2 = mv_destroy_source(source);
	if (MEDIA_VISION_ERROR_NONE != err2) {
		if (MEDIA_VISION_ERROR_NONE != err2) {
			printf(TEXT_RED
					"ERROR: Errors were occurred during destroying the source!!! code: %i"
					TEXT_RESET "\n", err2);
		}
	}

	double accuracy = (TP + TN) / (double) (TP + FP + TN + FN);
	double precision = TP / (double) (TP + FP);
	double recall = TP / (double) (TP + FN);
	double f1 = 2 * precision * recall / (precision + recall);

	printf(TEXT_GREEN "Evaluation results:\n" TEXT_RESET);
	printf(TEXT_YELLOW "\tTRUE POSITIVE  : " TEXT_RESET "%5i\n", TP);
	printf(TEXT_YELLOW "\tFALSE POSITIVE : " TEXT_RESET "%5i\n", FP);
	printf(TEXT_YELLOW "\tTRUE NEGATIVE  : " TEXT_RESET "%5i\n", TN);
	printf(TEXT_YELLOW "\tFALSE NEGATIVE : " TEXT_RESET "%5i\n", FN);
	printf(TEXT_YELLOW "\tAccuracy       : " TEXT_RESET "%f\n", accuracy);
	printf(TEXT_YELLOW "\tPrecision      : " TEXT_RESET "%f\n", precision);
	printf(TEXT_YELLOW "\tRecall         : " TEXT_RESET "%f\n", recall);
	printf(TEXT_YELLOW "\tF1 score       : " TEXT_RESET "%f\n", f1);

	free(directories);

	return err;
}

int perform_recognize()
{
	printf("\n" TEXT_YELLOW
			"Recognition model isn't now created.\n"
			"You may create it to perform positive \n"
			"testing, or don't create to check the \n"
			"functionality behaviour for uncreated model."
			TEXT_RESET
			"\n");

	int err = MEDIA_VISION_ERROR_NONE;
	mv_face_recognition_model_h recognition_model = NULL;
	const bool do_create = show_confirm_dialog("Do Create Recognition Model?");
	if (do_create) {
		printf(TEXT_GREEN "Creating recognition model..." TEXT_RESET "\n");

		err = mv_face_recognition_model_create(&recognition_model);
		if (MEDIA_VISION_ERROR_NONE != err) {
			printf(TEXT_RED "Creating the model failed. Error code: %i. "
					"But you still can test with uncreated model.\n"
					TEXT_RESET "\n", err);
		} else {
			printf(TEXT_YELLOW "Recognition model has been created."
					TEXT_RESET "\n");
		}
	}

	int sel_opt = 0;
	const int options[11] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
	const char *names[11] = { "Add image example",
								"Reset examples by id",
								"Reset all examples",
								"Clone the model",
								"Learn the model",
								"Show learned labels",
								"Save model to file",
								"Load model from file",
								"Recognize with model",
								"Evaluate the model",
								"Destroy model and exit" };

	while (!sel_opt) {
		sel_opt = show_menu("Select action:", options, names, 11);
		notification_type_e notification_type = FAIL_OR_SUCCESSS;

		switch (sel_opt) {
		case 1:
			err = perform_mv_face_recognition_model_add_face_example(recognition_model, &notification_type);
			break;
		case 2:
			err = perform_mv_face_recognition_model_reset_face_examples(recognition_model, false);
			break;
		case 3:
			err = perform_mv_face_recognition_model_reset_face_examples(recognition_model, true);
			break;
		case 4:
			err = perform_mv_face_recognition_model_clone(recognition_model);
			break;
		case 5:
			err = perform_mv_face_recognition_model_learn(recognition_model);
			break;
		case 6:
			err = perform_mv_face_recognition_model_query_labels(recognition_model);
			break;
		case 7:
			err = perform_mv_face_recognition_model_save(recognition_model);
			break;
		case 8:
			err = perform_mv_face_recognition_model_load(&recognition_model);
			break;
		case 9:
			err = perform_mv_face_recognize(recognition_model);
			break;
		case 10:
			err = perform_model_evaluation(recognition_model);
			break;
		case 11:
			if (do_create) {
				err = mv_face_recognition_model_destroy(recognition_model);
				if (MEDIA_VISION_ERROR_NONE != err) {
					printf(TEXT_RED
								"Error with code %i was occurred during destoy"
								TEXT_RESET "\n", err);
				}
			} else {
				err = MEDIA_VISION_ERROR_NONE;
			}
			break;
		default:
			sel_opt = 0;
			printf("ERROR: Incorrect option was selected.\n");
			continue;
		}

		print_action_result(names[sel_opt - 1], err, notification_type);

		if (sel_opt != 11) {
			sel_opt = 0;
		}
	}

	return err;
}

int perform_mv_face_tracking_model_save(mv_face_tracking_model_h model)
{
	char *out_file_name = NULL;

	while (input_string("Input file name to save the model:",
						1024, &(out_file_name)) == -1) {
		printf(TEXT_RED "Incorrect input! Try again." TEXT_RESET "\n");
	}

	const int err = mv_face_tracking_model_save(out_file_name, model);

	free(out_file_name);

	return err;
}

int perform_mv_face_tracking_model_load(mv_face_tracking_model_h *model)
{
	char *in_file_name = NULL;

	while (input_string("Input file name to load model from:",
						1024, &(in_file_name)) == -1) {
		printf(TEXT_RED "Incorrect input! Try again." TEXT_RESET "\n");
	}

	const int err = mv_face_tracking_model_load(in_file_name, model);

	free(in_file_name);

	return err;
}

int perform_mv_face_tracking_model_clone(
		mv_face_tracking_model_h model_to_clone)
{
	int err = MEDIA_VISION_ERROR_NONE;

	mv_face_tracking_model_h cloned_model = NULL;

	printf(TEXT_GREEN "Perform clone of the tracking model..."
			TEXT_RESET "\n");

	err = mv_face_tracking_model_clone(model_to_clone, &cloned_model);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(TEXT_RED "Errors were occurred during model clone. Error code %i"
				TEXT_RESET "\n", err);
		return err;
	}

	printf(TEXT_YELLOW "Model cloning is done." TEXT_RESET "\n");

	if (show_confirm_dialog("Save " TEXT_YELLOW "source model" TEXT_RESET
							" to file?")) {
		const int serr = perform_mv_face_tracking_model_save(model_to_clone);
		if (MEDIA_VISION_ERROR_NONE != serr) {
			printf(TEXT_RED
					"Errors were occurred when trying to save "
					"source model to file. Error code %i" TEXT_RESET "\n", serr);
		}
	}

	if (show_confirm_dialog("Save " TEXT_YELLOW "destination model" TEXT_RESET
							" to file?")) {
		const int serr = perform_mv_face_tracking_model_save(cloned_model);
		if (MEDIA_VISION_ERROR_NONE != serr) {
			printf(TEXT_RED
					"Errors were occurred when trying to save destination model "
					"to file. Error code %i" TEXT_RESET "\n", serr);
		}
	}

	if (cloned_model) {
		const int dest_err = mv_face_tracking_model_destroy(cloned_model);
		if (MEDIA_VISION_ERROR_NONE != dest_err) {
			printf(TEXT_RED
					"Errors were occurred when destroying destination model ."
					"Error code %i" TEXT_RESET "\n", dest_err);
		}
	}

	return err;
}

static volatile bool frame_read = false;

void video_1_sample_cb(
		char *buffer,
		unsigned int buffer_size,
		image_data_s image_data,
		void *user_data)
{
	if (!frame_read) {
		mv_source_h source = (mv_source_h)user_data;

		const int err = mv_source_fill_by_buffer(
							source,
							buffer,
							buffer_size,
							image_data.image_width,
							image_data.image_height,
							image_data.image_colorspace);

		if (MEDIA_VISION_ERROR_NONE != err) {
			printf(TEXT_RED "ERROR: Errors were occurred during filling the "
					"source based on the video frame! Error code: %i"
					TEXT_RESET, err);
		}

		frame_read = true;
	}
}

void face_detected_for_tracking_cb(
		mv_source_h source,
		mv_engine_config_h engine_cfg,
		mv_rectangle_s *faces_locations,
		int number_of_faces,
		void *user_data)
{
	if (number_of_faces < 1) {
		printf(TEXT_RED "Unfortunatly, no faces were detected on the\n"
						"preparation frame. You has to specify bounding\n"
						"quadrangles for tracking without advices."
				TEXT_RESET "\n");
		return;
	}

	printf(TEXT_YELLOW "%i face(s) were detected at the preparation frame.\n"
						"Following list includes information on faces bounding\n"
						"boxes coordinates:"
			TEXT_RESET "\n", number_of_faces);

	int idx = 0;
	while (idx < number_of_faces) {
		printf(TEXT_MAGENTA "Face %i bounding box: " TEXT_RESET "\n", ++idx);
		printf(TEXT_CYAN "\tTop left point:     x1: %4i;  y1: %4i\n" TEXT_RESET,
				faces_locations[idx - 1].point.x,
				faces_locations[idx - 1].point.y);
		printf(TEXT_CYAN "\tTop right point:    x2: %4i;  y2: %4i\n" TEXT_RESET,
				faces_locations[idx - 1].point.x + faces_locations[idx - 1].width,
				faces_locations[idx - 1].point.y);
		printf(TEXT_CYAN "\tBottom right point: x3: %4i;  y3: %4i\n" TEXT_RESET,
				faces_locations[idx - 1].point.x + faces_locations[idx - 1].width,
				faces_locations[idx - 1].point.y + faces_locations[idx - 1].height);
		printf(TEXT_CYAN "\tBottom right point: x4: %4i;  y4: %4i\n" TEXT_RESET,
				faces_locations[idx - 1].point.x,
				faces_locations[idx - 1].point.y + faces_locations[idx - 1].height);
	}
}

int load_source_from_first_video_frame(const char *video_file, mv_source_h source)
{
	mv_video_reader_h reader = NULL;
	int err = mv_create_video_reader(&reader);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(TEXT_RED "ERROR: Errors were occurred during creating the video "
				"reader! Error code: %i\n" TEXT_RESET, err);
				return err;
	}

	err = mv_video_reader_set_new_sample_cb(
					reader,
					video_1_sample_cb,
					source);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(TEXT_RED "ERROR: Errors were occurred during new sample "
				"callback set! Error code: %i\n" TEXT_RESET, err);

		const int err2 = mv_destroy_video_reader(reader);
		if (MEDIA_VISION_ERROR_NONE != err2) {
			printf(TEXT_RED "ERROR: Errors were occurred during video reader "
					"destroy! Error code: %i\n" TEXT_RESET, err);
		}

		return err;
	}

	frame_read = false;
	image_data_s video_info;
	unsigned int fps;
	err = mv_video_reader_load(reader, video_file, &video_info, &fps);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(TEXT_RED "ERROR: Errors were occurred during loading the video "
				"by reader! Error code: %i\n" TEXT_RESET, err);

		const int err2 = mv_destroy_video_reader(reader);
		if (MEDIA_VISION_ERROR_NONE != err2) {
			printf(TEXT_RED "ERROR: Errors were occurred during video reader "
					"destroy! Error code: %i\n" TEXT_RESET, err);
		}

		return err;
	}

	/* wait for the video reading thread */
	while (true) {
		if (frame_read) {
			int err2 = mv_video_reader_stop(reader);
			if (MEDIA_VISION_ERROR_NONE != err2) {
				printf(TEXT_RED "ERROR: Errors were occurred during attempt to "
						"stop video reader! Error code: %i\n" TEXT_RESET, err2);
			}

			err2 = mv_destroy_video_reader(reader);
			if (MEDIA_VISION_ERROR_NONE != err2) {
				printf(TEXT_RED "ERROR: Errors were occurred during video "
						"reader destroy! Error code: %i\n" TEXT_RESET, err2);
			}

			break;
		}
	}

	return MEDIA_VISION_ERROR_NONE;
}

int perform_mv_face_tracking_model_prepare(mv_face_tracking_model_h model)
{
	printf(TEXT_YELLOW "Before any tracking session the tracking model\n"
						"preparation is required. Exception is the case when\n"
						"the next tracking session will be performed with the\n"
						"video which is the direct continuation of the video\n"
						"has been used at the previous tracking session.\n"
						"Preparation has to be done with the first frame of\n"
						"the video or first image from continuous image\n"
						"sequence for which next tracking session plan to be\n"
						"performed.\nTracking model preparation includes\n"
						"specifying the location of the face to be tracked on\n"
						"the first frame. Face tracking algorithm will try to\n"
						"grab the face image significant features and\n"
						"optionally will try to determine the background.\n"
						"Actually, preparation is model-dependent and may\n"
						"differs in respect to used tracking algorithm."
			TEXT_RESET "\n");

	int sel_opt = 0;
	const int options[2] = { 1, 2 };
	const char *names[2] = { "Prepare with the video file",
								"Prepare with the image file" };
	bool is_video = false;

	while (!sel_opt) {
		sel_opt = show_menu("Select action:", options, names, 2);
		switch (sel_opt) {
		case 1:
			is_video = true;
			break;
		case 2:
			is_video = false;
			break;
		default:
			sel_opt = 0;
			continue;
		}
	}

	mv_source_h preparation_frame = NULL;
	int err = mv_create_source(&preparation_frame);

	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(TEXT_RED
				"ERROR: Errors were occurred during creating the source!!! code: %i"
				TEXT_RESET "\n", err);

		return err;
	}

	char *init_frame_file_name = NULL;
	const char *prompt_str =
			(is_video ? "Input video file name to prepare the model:"
						: "Input image file name to prepare the model:");

	while (input_string(prompt_str, 1024, &(init_frame_file_name)) == -1)
		printf(TEXT_RED "Incorrect input! Try again.\n" TEXT_RESET);

	if (is_video)
		err = load_source_from_first_video_frame(init_frame_file_name, preparation_frame);
	else
		err = load_mv_source_from_file(init_frame_file_name, preparation_frame);

	free(init_frame_file_name);

	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(TEXT_RED "ERROR: Errors were occurred during preparation "
				"frame/image load! Error code: %i\n" TEXT_RESET, err);

		int err2 = mv_destroy_source(preparation_frame);
		if (MEDIA_VISION_ERROR_NONE != err2) {
			printf(TEXT_RED "ERROR: Errors were occurred during destroying the "
					"source! Error code: %i\n" TEXT_RESET, err2);
		}

		return err;
	}

	mv_engine_config_h eng_config = NULL;
	err = mv_create_engine_config(&eng_config);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(TEXT_RED "ERROR: Errors were occurred during creating the "
				"engine config! Error code: %i\n" TEXT_RESET, err);
	} else {
		err = mv_engine_config_set_string_attribute(
					eng_config,
					MV_FACE_DETECTION_MODEL_FILE_PATH,
					"/usr/share/OpenCV/haarcascades/haarcascade_frontalface_alt2.xml");

		if (MEDIA_VISION_ERROR_NONE != err) {
			printf(TEXT_RED "ERROR: Errors were occurred during setting of the "
					"the 'MV_FACE_DETECTION_MODEL_FILE_PATH' attribute "
					"for engine configuration! Check media-vision-config.json "
					"file existence. Error code: %i" TEXT_RESET, err);
		}
	}

	err = mv_face_detect(
				preparation_frame,
				eng_config,
				face_detected_for_tracking_cb,
				NULL);

	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(TEXT_RED "ERROR: Errors were occurred during face detection! "
				"Error code: %i\n" TEXT_RESET, err);

		int err2 = mv_destroy_engine_config(eng_config);
		if (MEDIA_VISION_ERROR_NONE != err2) {
			printf(TEXT_RED "ERROR: Errors were occurred during destroying the "
					"engine configuration! Error code: %i\n" TEXT_RESET, err2);
		}

		return err;
	}

	mv_quadrangle_s roi;

	if (show_confirm_dialog("Do specify the face location?")) {
		printf(TEXT_YELLOW "Specify the coordinates of the quadrangle to be used\n"
							"for tracking model preparation:" TEXT_RESET "\n");
		int idx = 0;
		char str_prompt[100];
		while (idx < 4) {
			++idx;
			snprintf(str_prompt, 100, "Specify point %i x coordinate: x%i = ",
					idx - 1, idx);
			while (-1 == input_int(str_prompt, INT_MIN, INT_MAX,
									&(roi.points[idx - 1].x))) {
				printf("Incorrect input! Try again.\n");
			}
			snprintf(str_prompt, 100, "Specify point %i y coordinate: y%i = ",
								idx - 1, idx);
			while (-1 == input_int(str_prompt, INT_MIN, INT_MAX,
									&(roi.points[idx - 1].y))) {
				printf("Incorrect input! Try again.\n");
			}
		}

		err = mv_face_tracking_model_prepare(
					model, eng_config, preparation_frame, &roi);
	} else {
		err = mv_face_tracking_model_prepare(
					model, eng_config, preparation_frame, NULL);
	}

	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(TEXT_RED "ERROR: Errors were occurred during the tracking model "
				"preparation! Error code: %i\n" TEXT_RESET, err);
	}

	const int err2 = mv_destroy_source(preparation_frame);
	if (MEDIA_VISION_ERROR_NONE != err2) {
		printf(TEXT_RED "ERROR: Errors were occurred during destroying the "
				"source! Error code: %i\n" TEXT_RESET, err2);
	}

	return err;
}

static char *track_output_dir = NULL;

static int track_frame_counter = 0;

void track_cb(
		mv_source_h source,
		mv_face_tracking_model_h tracking_model,
		mv_engine_config_h engine_cfg,
		mv_quadrangle_s *location,
		double confidence,
		void *user_data)
{
	static bool track_catch_face = false;

	++track_frame_counter;

	unsigned char *out_buffer = NULL;
	unsigned int buf_size = 0;
	image_data_s image_data = { 0, 0, MEDIA_VISION_COLORSPACE_INVALID };
	if (MEDIA_VISION_ERROR_NONE !=
		mv_source_get_buffer(source, &out_buffer, &buf_size) ||
		MEDIA_VISION_ERROR_NONE !=
		mv_source_get_width(source, &(image_data.image_width)) ||
		MEDIA_VISION_ERROR_NONE !=
		mv_source_get_height(source, &(image_data.image_height)) ||
		MEDIA_VISION_ERROR_NONE !=
		mv_source_get_colorspace(source, &(image_data.image_colorspace))) {
		printf("ERROR: Creating out image is impossible.\n");

		return;
	}

	if (NULL != location) {
		if (!track_catch_face) {
			printf(TEXT_GREEN "Frame %i : Tracked object is appeared" TEXT_RESET "\n",
					track_frame_counter);
			track_catch_face = true;
		} else {
			printf(TEXT_YELLOW "Frame %i : Tracked object is tracked" TEXT_RESET "\n",
					track_frame_counter);
		}

		const int rectangle_thickness = 3;
		const int drawing_color[] = {255, 0, 0};

		printf(TEXT_YELLOW
				"Location: (%i,%i) -> (%i,%i) -> (%i,%i) -> (%i,%i)\n"
				TEXT_RESET,
				location->points[0].x,
				location->points[0].y,
				location->points[1].x,
				location->points[1].y,
				location->points[2].x,
				location->points[2].y,
				location->points[3].x,
				location->points[3].y);
		printf(TEXT_YELLOW "Track confidence: %f" TEXT_RESET "\n", confidence);

		const int err = draw_quadrangle_on_buffer(
								*location,
								rectangle_thickness,
								drawing_color,
								&image_data,
								out_buffer);

		if (MEDIA_VISION_ERROR_NONE != err) {
			printf(TEXT_RED "ERROR: Quadrangle wasn't drew on frame buffer! "
					"Error code: %i\n" TEXT_RESET, err);

			return;
		}
	} else {
		if (track_catch_face) {
			printf(TEXT_RED "Frame %i : Tracked object is lost" TEXT_RESET "\n",
					track_frame_counter);
			track_catch_face = false;
		} else {
			printf(TEXT_YELLOW "Frame %i : Tracked object isn't detected" TEXT_RESET "\n",
			track_frame_counter);
		}
	}

	char file_path[1024];
	snprintf(file_path, 1024, "%s/%05d.jpg", track_output_dir, track_frame_counter);
	if (MEDIA_VISION_ERROR_NONE == save_image_from_buffer(
					file_path, out_buffer, &image_data, 100)) {
		printf("Frame %i was outputted as %s\n", track_frame_counter, file_path);
	} else {
		printf(TEXT_RED "ERROR: Failed to generate output file %s. "
				"Check file name and permissions.\n" TEXT_RESET, file_path);
	}
}

void track_on_sample_cb(
		char *buffer,
		unsigned int buffer_size,
		image_data_s image_data,
		void *user_data)
{
	mv_source_h source = NULL;
	int err = mv_create_source(&source);

	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(TEXT_RED "ERROR: Errors were occurred during creating the source "
				"based on the video frame! Error code: %i\n" TEXT_RESET, err);

		return;
	}

	err = mv_source_fill_by_buffer(
						source,
						buffer,
						buffer_size,
						image_data.image_width,
						image_data.image_height,
						image_data.image_colorspace);

	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(TEXT_RED "ERROR: Errors were occurred during filling the source "
				"based on the video frame! Error code: %i\n" TEXT_RESET , err);

		return;
	}

	mv_face_tracking_model_h tracking_model =
			(mv_face_tracking_model_h)user_data;

	err = mv_face_track(source, tracking_model, NULL, track_cb, false, NULL);

	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(TEXT_RED "ERROR: Errors were occurred during tracking the face "
				TEXT_RESET "on the video frame! Error code: %i\n", err);

		return;
	}
}

/* end of stream callback */
void eos_cb(void *user_data)
{
	printf("Video was fully processed\n");
	if (NULL == user_data) {
		printf(TEXT_RED
				"ERROR: eos callback can't stop tracking process."TEXT_RESET);
		return;
	}

	pthread_mutex_unlock((pthread_mutex_t*)user_data);
}

int generate_image_sequence(
		mv_face_tracking_model_h tracking_model,
		const char *track_target_file_name)
{
	mv_video_reader_h reader = NULL;
	int err = mv_create_video_reader(&reader);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(TEXT_RED
				"ERROR: Errors were occurred during creating the video "
				"reader! Error code: %i" TEXT_RESET "\n", err);
		return err;
	}

	image_data_s video_info;
	unsigned int fps;
	/* init_frame_file_name */
	err = mv_video_reader_load(reader, track_target_file_name, &video_info, &fps);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(TEXT_RED "ERROR: Errors were occurred during loading the video "
				"by reader! Error code: %i" TEXT_RESET "\n", err);

		const int err2 = mv_destroy_video_reader(reader);
		if (MEDIA_VISION_ERROR_NONE != err2) {
			printf(TEXT_RED "ERROR: Errors were occurred during video reader "
					"destroy! Error code: %i" TEXT_RESET "\n", err);
		}

	return err;
	}

	err = mv_video_reader_set_new_sample_cb(
					reader,
					track_on_sample_cb,
					tracking_model);

	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(TEXT_RED
				"ERROR: Errors were occurred during new sample callback set!"
				" Error code: %i" TEXT_RESET "\n", err);

		const int err2 = mv_destroy_video_reader(reader);
		if (MEDIA_VISION_ERROR_NONE != err2) {
			printf(TEXT_RED "ERROR: Errors were occurred during video reader "
					"destroy! Error code: %i" TEXT_RESET "\n", err);
		}

		return err;
	}

	pthread_mutex_t block_during_tracking_mutex;
	pthread_mutex_init(&block_during_tracking_mutex, NULL);
	pthread_mutex_lock(&block_during_tracking_mutex);

	/* set end of stream callback */
	err = mv_video_reader_set_eos_cb(reader, eos_cb, &block_during_tracking_mutex);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(TEXT_RED
				"ERROR: Errors were occurred during setting the eos "
				"callback for reader! Error code: %i" TEXT_RESET "\n", err);

		const int err2 = mv_destroy_video_reader(reader);
		if (MEDIA_VISION_ERROR_NONE != err2) {
			printf(TEXT_RED
					"ERROR: Errors were occurred during video reader destroy!"
					" Error code: %i" TEXT_RESET "\n", err);
		}

		pthread_mutex_unlock(&block_during_tracking_mutex);
		pthread_mutex_destroy(&block_during_tracking_mutex);

		return err;
	}

	err = mv_video_reader_start(reader);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(TEXT_RED "ERROR: Errors were occurred during starting the "
				"video reader! Error code: %i" TEXT_RESET "\n", err);

		const int err2 = mv_destroy_video_reader(reader);
		if (MEDIA_VISION_ERROR_NONE != err2) {
			printf(TEXT_RED
					"ERROR: Errors were occurred during video reader destroy!"
					" Error code: %i" TEXT_RESET "\n", err);
		}

		pthread_mutex_unlock(&block_during_tracking_mutex);
		pthread_mutex_destroy(&block_during_tracking_mutex);

		return err;
	}

	/* wait for the video reading thread */

	pthread_mutex_lock(&block_during_tracking_mutex);
	pthread_mutex_unlock(&block_during_tracking_mutex);
	pthread_mutex_destroy(&block_during_tracking_mutex);

	err = mv_video_reader_stop(reader);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(TEXT_RED "ERROR: Errors were occurred during "
				"attempt to stop video reader! Error code: %i\n"
				TEXT_RESET, err);
	}

	err = mv_destroy_video_reader(reader);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf(TEXT_RED "ERROR: Errors were occurred during video "
				"reader destroy! Error code: %i\n" TEXT_RESET, err);
	}

	return MEDIA_VISION_ERROR_NONE;
}

int perform_mv_face_track(mv_face_tracking_model_h tracking_model)
{
	printf(TEXT_YELLOW "Before any tracking session the tracking model\n"
						"preparation is required. Exception is the case when\n"
						"the next tracking session will be performed with the\n"
						"video which is the direct continuation of the video\n"
						"has been used at the previous tracking session.\n"
						"If you want to test correct tracking case, don't\n"
						"forget to perform preparation before tracking."
			TEXT_RESET "\n");

	char *track_target_file_name = NULL;

	while (input_string("Input video file name to track on:",
						1024, &(track_target_file_name)) == -1) {
		printf(TEXT_RED "Incorrect input! Try again." TEXT_RESET "\n");
	}

	while (input_string("Input directory to save tracking results:",
						1024, &(track_output_dir)) == -1) {
		printf(TEXT_RED "Incorrect input! Try again." TEXT_RESET "\n");
	}

	track_frame_counter = 0;

	return generate_image_sequence(tracking_model, track_target_file_name);
}

int perform_track()
{
	printf("\n" TEXT_YELLOW
			"Tracking model isn't now created.\n"
			"You may create it to perform positive \n"
			"testing, or don't create to check the \n"
			"functionality behaviour for uncreated model."
			TEXT_RESET
			"\n");

	int err = MEDIA_VISION_ERROR_NONE;
	mv_face_tracking_model_h tracking_model = NULL;
	const bool do_create = show_confirm_dialog("Do Create Tracking Model?");
	if (do_create) {
		printf(TEXT_GREEN "Creating tracking model..." TEXT_RESET "\n");

		err = mv_face_tracking_model_create(&tracking_model);
		if (MEDIA_VISION_ERROR_NONE != err) {
			printf(TEXT_RED "Creating the model failed. Error code: %i. "
					"But you still can test with uncreated model.\n"
					TEXT_RESET "\n", err);
		} else {
			printf(TEXT_YELLOW "Tracking model has been created."
					TEXT_RESET "\n");
		}
	}

	int sel_opt = 0;
	const int options[6] = { 1, 2, 3, 4, 5, 6 };
	const char *names[6] = { "Prepare the model",
								"Clone the model",
								"Save model to file",
								"Load model from file",
								"Track with model",
								"Destroy model and exit" };

	while (!sel_opt) {
		sel_opt = show_menu("Select action:", options, names, 6);
		notification_type_e notification_type = FAIL_OR_SUCCESSS;

		switch (sel_opt) {
		case 1:
			err = perform_mv_face_tracking_model_prepare(tracking_model);
			break;
		case 2:
			err = perform_mv_face_tracking_model_clone(tracking_model);
			break;
		case 3:
			err = perform_mv_face_tracking_model_save(tracking_model);
			break;
		case 4:
			err = perform_mv_face_tracking_model_load(&tracking_model);
			break;
		case 5:
			err = perform_mv_face_track(tracking_model);
			notification_type = FAIL_OR_DONE;
			break;
		case 6:
			if (do_create) {
				err = mv_face_tracking_model_destroy(tracking_model);
				if (MEDIA_VISION_ERROR_NONE != err) {
					printf(TEXT_RED
							"Error with code %i was occurred during destroy"
							TEXT_RESET "\n", err);
				}
			} else {
				err = MEDIA_VISION_ERROR_NONE;
			}
			break;
		default:
			sel_opt = 0;
			printf("ERROR: Incorrect input.\n");
			continue;
		}

		print_action_result(names[sel_opt - 1], err, notification_type);

		if (sel_opt != 6) {
			sel_opt = 0;
		}
	}

	return err;
}

int perform_eye_condition_recognize()
{
	Perform_eye_condition_recognize = true;

	const int err = perform_detect();

	Perform_eye_condition_recognize = false;

	return err;
}

int perform_face_expression_recognize()
{
	Perform_facial_expression_recognize = true;

	const int err = perform_detect();

	Perform_facial_expression_recognize = false;

	return err;
}

int main(void)
{
	int err = MEDIA_VISION_ERROR_NONE;

	int sel_opt = 0;
	const int options[6] = { 1, 2, 3, 4, 5, 6 };
	const char *names[6] = { "Detect",
								"Track",
								"Recognize",
								"Eye condition",
								"Face expression",
								"Exit" };

	while (sel_opt == 0) {
		sel_opt = show_menu("Select action:", options, names, 6);
		switch (sel_opt) {
		case 1:
			err = perform_detect();
			break;
		case 2:
			err = perform_track();
			break;
		case 3:
			err = perform_recognize();
			break;
		case 4:
			err = perform_eye_condition_recognize();
			break;
		case 5:
			err = perform_face_expression_recognize();
			break;
		case 6:
			return 0;
		default:
			sel_opt = 0;
			printf("Invalid option.\n");
			continue;
		}

		int do_another = 0;

		if (err != MEDIA_VISION_ERROR_NONE)
			printf("ERROR: Action is finished with error code: %i\n", err);

		sel_opt = 0;
		const int options_last[2] = { 1, 2 };
		const char *names_last[2] = { "YES", "NO" };

		while (sel_opt == 0) {
			sel_opt = show_menu("Perform another action?", options_last, names_last, 2);

			switch (sel_opt) {
			case 1:
				do_another = 1;
				break;
			case 2:
				do_another = 0;
				break;
			default:
				sel_opt = 0;
				printf("Invalid option.\n");
				break;
			}
		}

		sel_opt = (do_another == 1 ? 0 : sel_opt);
	}

	return 0;
}
