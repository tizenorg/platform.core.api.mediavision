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

#include <mv_barcode.h>

#include <image_helper.h>
#include <mv_private.h>

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavutil/pixfmt.h>

typedef struct {
	mv_barcode_type_e type;
	mv_barcode_qr_ecc_e ecc;
	mv_barcode_qr_mode_e mode;
	int version;
	size_t width;
	size_t height;
	mv_barcode_image_format_e out_image_format;
	mv_colorspace_e colorspace;
	char *message;
	char *file_name;
	char *out_file_name;
	unsigned char *out_buffer_ptr;
} barcode_model_s;

typedef enum {
	MV_TS_GENERATE_TO_IMAGE_FCN,
	MV_TS_GENERATE_TO_SOURCE_FCN
} generation_fcn_e;

int convert_rgb_to(unsigned char *src_buffer, unsigned char **dst_buffer,
		image_data_s image_data, mv_colorspace_e dst_colorspace,
		unsigned long *cvt_buffer_size)
{
	enum PixelFormat pixel_format = PIX_FMT_NONE;

	MEDIA_VISION_FUNCTION_ENTER();

	switch (dst_colorspace) {
	case MEDIA_VISION_COLORSPACE_Y800:
		pixel_format = PIX_FMT_GRAY8;
		break;
	case MEDIA_VISION_COLORSPACE_I420:
		pixel_format = PIX_FMT_YUV420P;
		break;
	case MEDIA_VISION_COLORSPACE_NV12:
		pixel_format = PIX_FMT_NV12;
		break;
	case MEDIA_VISION_COLORSPACE_YV12:
		/* the same as I420 with inversed U and V */
		pixel_format = PIX_FMT_YUV420P;
		break;
	case MEDIA_VISION_COLORSPACE_NV21:
		pixel_format = PIX_FMT_NV21;
		break;
	case MEDIA_VISION_COLORSPACE_YUYV:
		pixel_format = PIX_FMT_YUYV422;
		break;
	case MEDIA_VISION_COLORSPACE_UYVY:
		pixel_format = PIX_FMT_UYVY422;
		break;
	case MEDIA_VISION_COLORSPACE_422P:
		pixel_format = PIX_FMT_YUV422P;
		break;
	case MEDIA_VISION_COLORSPACE_RGB565:
		pixel_format = PIX_FMT_RGB565BE;
		break;
	case MEDIA_VISION_COLORSPACE_RGBA:
		pixel_format = PIX_FMT_RGBA;
		break;
	case MEDIA_VISION_COLORSPACE_RGB888:
		*cvt_buffer_size = image_data.image_width * image_data.image_height * 3;
		(*dst_buffer) = (unsigned char*)malloc(*cvt_buffer_size);
		memcpy(*dst_buffer, src_buffer, *cvt_buffer_size);

		MEDIA_VISION_FUNCTION_LEAVE();
		return MEDIA_VISION_ERROR_NONE;
	default:
		MEDIA_VISION_FUNCTION_LEAVE();
		return MEDIA_VISION_ERROR_NOT_SUPPORTED;
	}

	AVPicture src_picture;
	AVPicture dst_picture;

	avpicture_fill(&src_picture, (uint8_t*)src_buffer, PIX_FMT_RGB24,
			image_data.image_width, image_data.image_height);

	avpicture_alloc(&dst_picture, pixel_format,
			image_data.image_width, image_data.image_height);

	struct SwsContext *context = sws_getContext(
			image_data.image_width, image_data.image_height, PIX_FMT_RGB24,
			image_data.image_width, image_data.image_height, pixel_format,
			SWS_FAST_BILINEAR, 0, 0, 0);

	sws_scale(context, (const uint8_t * const *)src_picture.data,
			src_picture.linesize, 0, image_data.image_height,
			dst_picture.data, dst_picture.linesize);

	int picture_size = avpicture_get_size(pixel_format,
			image_data.image_width, image_data.image_height);
	if(picture_size < 0) {
		avpicture_free(&dst_picture);
		MEDIA_VISION_FUNCTION_LEAVE();
		return MEDIA_VISION_ERROR_OUT_OF_MEMORY;
	}
	*cvt_buffer_size = (unsigned long)picture_size;

	(*dst_buffer) = (unsigned char*)malloc(*cvt_buffer_size);
	memcpy(*dst_buffer, dst_picture.data[0], *cvt_buffer_size);

	avpicture_free(&dst_picture);

	MEDIA_VISION_FUNCTION_LEAVE();

	return MEDIA_VISION_ERROR_NONE;
}

int find_min_x(const mv_quadrangle_s *quadrangle, int *minX)
{
	MEDIA_VISION_FUNCTION_ENTER();

	if (NULL == quadrangle) {
		MEDIA_VISION_FUNCTION_LEAVE();
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	*minX = quadrangle->points[0].x;
	*minX = quadrangle->points[1].x < *minX ? quadrangle->points[1].x : *minX;
	*minX = quadrangle->points[2].x < *minX ? quadrangle->points[2].x : *minX;
	*minX = quadrangle->points[3].x < *minX ? quadrangle->points[3].x : *minX;

	MEDIA_VISION_FUNCTION_LEAVE();

	return MEDIA_VISION_ERROR_NONE;
}

int find_min_y(const mv_quadrangle_s *quadrangle, int *minY)
{
	MEDIA_VISION_FUNCTION_ENTER();

	if (NULL == quadrangle) {
		MEDIA_VISION_FUNCTION_LEAVE();
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	*minY = quadrangle->points[0].y;
	*minY = quadrangle->points[1].y < *minY ? quadrangle->points[1].y : *minY;
	*minY = quadrangle->points[2].y < *minY ? quadrangle->points[2].y : *minY;
	*minY = quadrangle->points[3].y < *minY ? quadrangle->points[3].y : *minY;

	MEDIA_VISION_FUNCTION_LEAVE();

	return MEDIA_VISION_ERROR_NONE;
}

int find_max_x(const mv_quadrangle_s *quadrangle, int *maxX)
{
	MEDIA_VISION_FUNCTION_ENTER();

	if (NULL == quadrangle) {
		MEDIA_VISION_FUNCTION_LEAVE();
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	*maxX = quadrangle->points[0].x;
	*maxX = quadrangle->points[1].x > *maxX ? quadrangle->points[1].x : *maxX;
	*maxX = quadrangle->points[2].x > *maxX ? quadrangle->points[2].x : *maxX;
	*maxX = quadrangle->points[3].x > *maxX ? quadrangle->points[3].x : *maxX;

	MEDIA_VISION_FUNCTION_LEAVE();

	return MEDIA_VISION_ERROR_NONE;
}

int find_max_y(const mv_quadrangle_s *quadrangle, int *maxY)
{
	MEDIA_VISION_FUNCTION_ENTER();

	if (NULL == quadrangle) {
		MEDIA_VISION_FUNCTION_LEAVE();
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	*maxY = quadrangle->points[0].y;
	*maxY = quadrangle->points[1].y > *maxY ? quadrangle->points[1].y : *maxY;
	*maxY = quadrangle->points[2].y > *maxY ? quadrangle->points[2].y : *maxY;
	*maxY = quadrangle->points[3].y > *maxY ? quadrangle->points[3].y : *maxY;

	MEDIA_VISION_FUNCTION_LEAVE();

	return MEDIA_VISION_ERROR_NONE;
}

bool _mv_engine_config_supported_attribute(mv_config_attribute_type_e attribute_type,
		const char *attribute_name, void *user_data)
{
	printf("Callback call for engine configuration attribute\n");

	if (user_data == NULL)
		return false;

	mv_engine_config_h mv_engine_config = (mv_engine_config_h *)user_data;

	int int_value = 0;
	double double_value = 0.0;
	bool bool_value = false;
	char str_value[1024];
	switch (attribute_type) {
	case MV_ENGINE_CONFIG_ATTR_TYPE_DOUBLE:
		if (MEDIA_VISION_ERROR_KEY_NOT_AVAILABLE ==
			mv_engine_config_get_double_attribute(
						mv_engine_config, attribute_name, &double_value)) {
			printf("Default double attribute %s wasn't set in engine\n",
					attribute_name);
			return false;
		}
		printf("Default double attribute %s was set to %f in engine\n",
					attribute_name, double_value);
		break;
	case MV_ENGINE_CONFIG_ATTR_TYPE_INTEGER:
		if (MEDIA_VISION_ERROR_KEY_NOT_AVAILABLE ==
			mv_engine_config_get_int_attribute(
						mv_engine_config, attribute_name, &int_value)) {
			printf("Default integer attribute %s wasn't set in engine\n",
					attribute_name);
			return false;
		}
		printf("Default interget attribute %s was set to %d in engine\n",
				attribute_name, int_value);
		break;
	case MV_ENGINE_CONFIG_ATTR_TYPE_BOOLEAN:
		if (MEDIA_VISION_ERROR_KEY_NOT_AVAILABLE ==
			mv_engine_config_get_bool_attribute(
						mv_engine_config, attribute_name, &bool_value)) {
			printf("Default bool attribute %s wasn't set in engine\n",
					attribute_name);
			return false;
		}
		printf("Default bool attribute %s was set to %s in engine\n",
					attribute_name,  bool_value ? "TRUE" : "FALSE");
		break;
	case MV_ENGINE_CONFIG_ATTR_TYPE_STRING:
		if (MEDIA_VISION_ERROR_KEY_NOT_AVAILABLE ==
			mv_engine_config_get_string_attribute(
						mv_engine_config, attribute_name, &str_value)) {
			printf("Default string ttribute %s wasn't set in engine\n",
					attribute_name);
			return false;
		}
		printf("Default string attribute %s was set to %s in engine\n",
				attribute_name, str_value);
		break;
	default:
		printf("Not supported attribute type\n");
		return false;
	}

	return true;
}

void barcode_detected_cb(
		mv_source_h source,
		mv_engine_config_h engine_cfg,
		const mv_quadrangle_s *barcodes_locations,
		const char *messages[],
		const mv_barcode_type_e *types,
		int number_of_barcodes,
		void *user_data)
{
	MEDIA_VISION_FUNCTION_ENTER();

	printf("%i barcodes were detected on the image.\n", number_of_barcodes);
	if (number_of_barcodes > 0) {
		int is_source_data_loaded = 0;

		char *file_name = NULL;
		unsigned char *out_buffer = NULL;
		unsigned char *draw_buffer = NULL;
		unsigned int buf_size = 0;
		image_data_s image_data = { 0, 0, MEDIA_VISION_COLORSPACE_INVALID };
		/* Check Media Vision source: */
		if (MEDIA_VISION_ERROR_NONE != mv_source_get_buffer(source, &out_buffer, &buf_size) ||
			MEDIA_VISION_ERROR_NONE != mv_source_get_width(source, &(image_data.image_width)) ||
			MEDIA_VISION_ERROR_NONE != mv_source_get_height(source, &(image_data.image_height)) ||
			MEDIA_VISION_ERROR_NONE != mv_source_get_colorspace(source, &(image_data.image_colorspace)) ||
			user_data == NULL) {
			printf("ERROR: Creating out image is impossible.\n");
		} else {
			file_name = ((barcode_model_s *)user_data)->out_file_name;
			draw_buffer = ((barcode_model_s *)user_data)->out_buffer_ptr;
			image_data.image_colorspace = MEDIA_VISION_COLORSPACE_RGB888;
			is_source_data_loaded = 1;
		}

		int i = 0;
		for (i = 0; i < number_of_barcodes; ++i) {
			const char *cur_message = messages[i];
			mv_barcode_type_e cur_type = types[i];
			const char *str_type = NULL;
			switch (cur_type) {
			case MV_BARCODE_QR:
				str_type = "QR";
				break;
			case MV_BARCODE_UPC_A:
				str_type = "UPC-A";
				break;
			case MV_BARCODE_UPC_E:
				str_type = "UPC-E";
				break;
			case MV_BARCODE_EAN_8:
			case MV_BARCODE_EAN_13:
				str_type = "EAN-8/13";
				break;
			case MV_BARCODE_CODE128:
				str_type = "CODE128";
				break;
			case MV_BARCODE_CODE39:
				str_type = "CODE39";
				break;
			case MV_BARCODE_I2_5:
				str_type = "I25";
				break;
			default:
				str_type = "Undetected";
				break;
			}
			printf("\tBarcode %i : type is %s\n", i, str_type);
			if (cur_message != NULL)
				printf("\t            message is %s\n", cur_message);
			else
				printf("\t            message wasn't detected\n");

			if (is_source_data_loaded == 1) {
				int minX = 0;
				int minY = 0;
				int maxX = 0;
				int maxY = 0;
				if (MEDIA_VISION_ERROR_NONE != find_min_x(&barcodes_locations[i], &minX) ||
					MEDIA_VISION_ERROR_NONE != find_min_y(&barcodes_locations[i], &minY) ||
					MEDIA_VISION_ERROR_NONE != find_max_x(&barcodes_locations[i], &maxX) ||
					MEDIA_VISION_ERROR_NONE != find_max_y(&barcodes_locations[i], &maxY)) {
					continue;
				}

				const int drawing_color[] = {255, 0, 0};

				if (MEDIA_VISION_ERROR_NONE != draw_rectangle_on_buffer(
					minX,
					minY,
					maxX,
					maxY,
					6,
					drawing_color,
					&image_data,
					draw_buffer)) {
					continue;
				}
			}
		}

		if (file_name != NULL &&
			MEDIA_VISION_ERROR_NONE == save_image_from_buffer(file_name, draw_buffer, &image_data, 100)) {
			printf("Image was generated as %s\n", file_name);
		} else {
			printf("ERROR: Failed to generate output file. Check file name and permissions. \n");
		}

		printf("\n");
	}

	MEDIA_VISION_FUNCTION_LEAVE();
}

int generate_barcode_to_image(barcode_model_s model)
{
	MEDIA_VISION_FUNCTION_ENTER();

	if (model.message   == NULL ||
		model.file_name == NULL) {
		MEDIA_VISION_FUNCTION_LEAVE();
		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	LOGI("Call the mv_barcode_generate_image() function");

	const int err = mv_barcode_generate_image(
				NULL,
				model.message,
				model.width,
				model.height,
				model.type,
				model.mode,
				model.ecc,
				model.version,
				model.file_name,
				model.out_image_format);

	MEDIA_VISION_FUNCTION_LEAVE();

	return err;
}

int generate_barcode_to_source(barcode_model_s model)
{
	MEDIA_VISION_FUNCTION_ENTER();

	if (model.message   == NULL ||
		model.file_name == NULL) {
		MEDIA_VISION_FUNCTION_LEAVE();

		return MEDIA_VISION_ERROR_INVALID_PARAMETER;
	}

	LOGI("mv_source_h creation started");

	mv_source_h source = NULL;
	int err = mv_create_source(&source);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf("ERROR: Error occurred when trying to create Media Vision "
				"source. Error code: %i\n", err);

			MEDIA_VISION_FUNCTION_LEAVE();

			return err;
	}

	LOGI("mv_source_h creation finished");

	LOGI("Call the mv_barcode_generate_source() function");

	mv_engine_config_h mv_engine_config;
	err = mv_create_engine_config(&mv_engine_config);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf("ERROR: Errors were occurred during creating the media engine "
				"config: %i\n", err);
	}

	err = mv_barcode_generate_source(
			mv_engine_config,
			model.message,
			model.type,
			model.mode,
			model.ecc,
			model.version,
			source);

	if (MEDIA_VISION_ERROR_NONE != err) {
		printf("ERROR: Error occurred during generation barcode to the "
				"Media Vision source. Error code: %i\n", err);

		const int err2 = mv_destroy_source(source);
		if (MEDIA_VISION_ERROR_NONE != err2) {
			printf("ERROR: Error occurred when try to destroy Media Vision source."
					"Error code: %i\n", err2);
		}

		const int err3 = mv_destroy_engine_config(mv_engine_config);
		if (MEDIA_VISION_ERROR_NONE != err3) {
			printf("ERROR: Errors were occurred during destroying the media engine "
					"config: %i\n", err3);
		}

		MEDIA_VISION_FUNCTION_LEAVE();

		return err;
	}

	unsigned char *data_buffer = NULL;
	unsigned int buffer_size = 0;
	unsigned int image_width = 0;
	unsigned int image_height = 0;
	mv_colorspace_e image_colorspace = MEDIA_VISION_COLORSPACE_INVALID;

	bool is_source_corrupted = false;
	err = mv_source_get_buffer(source, &data_buffer, &buffer_size);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf("ERROR: Error occurred when trying to get buffer from "
				"Media Vision source. Error code: %i\n", err);
		is_source_corrupted = true;
	}

	err = mv_source_get_width(source, &image_width);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf("ERROR: Error occurred when trying to get width of "
				"Media Vision source. Error code: %i\n", err);
		is_source_corrupted = true;
	}

	err = mv_source_get_height(source, &image_height);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf("ERROR: Error occurred when trying to get height of "
				"Media Vision source. Error code: %i\n", err);
		is_source_corrupted = true;
	}

	err = mv_source_get_colorspace(source, &image_colorspace);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf("ERROR: Error occurred when trying to get colorspace of "
				"Media Vision source. Error code: %i\n", err);
		is_source_corrupted = true;
	}

	if (is_source_corrupted) {
		err = mv_destroy_source(source);
		if (MEDIA_VISION_ERROR_NONE != err) {
			printf("ERROR: Error occurred when trying to destroy Media Vision "
					"source. Error code: %i\n", err);
		}

		err = mv_destroy_engine_config(mv_engine_config);
		if (MEDIA_VISION_ERROR_NONE != err) {
			printf("ERROR: Errors were occurred during destroying the media engine "
					"config: %i\n", err);
		}

		MEDIA_VISION_FUNCTION_LEAVE();

		return MEDIA_VISION_ERROR_INTERNAL;
	}

	const image_data_s image_data = { image_width, image_height, image_colorspace };

	char *jpeg_file_name = "";
	if (0 == strcmp(model.file_name + strlen(model.file_name) - 4, ".jpg") ||
		0 == strcmp(model.file_name + strlen(model.file_name) - 5, ".jpeg")) {
		jpeg_file_name = (char*)malloc(strlen(model.file_name) + 1);
		if (jpeg_file_name == NULL) {
			mv_destroy_source(source);
			mv_destroy_engine_config(mv_engine_config);
			MEDIA_VISION_FUNCTION_LEAVE();
			return MEDIA_VISION_ERROR_OUT_OF_MEMORY;
		}

		strncpy(jpeg_file_name, model.file_name, strlen(model.file_name) + 1);
		jpeg_file_name[strlen(model.file_name)] = '\0';
	} else {
		jpeg_file_name = (char*)malloc(strlen(model.file_name) + 5);
		if (jpeg_file_name == NULL) {
			mv_destroy_source(source);
			mv_destroy_engine_config(mv_engine_config);
			MEDIA_VISION_FUNCTION_LEAVE();
			return MEDIA_VISION_ERROR_OUT_OF_MEMORY;
		}

		strncpy(jpeg_file_name, model.file_name, strlen(model.file_name) + 5);
		strncpy(jpeg_file_name + strlen(model.file_name), ".jpg", 5);
		jpeg_file_name[strlen(model.file_name) + 4] = '\0';
	}

	save_image_from_buffer(jpeg_file_name, data_buffer, &image_data, 100);

	free(jpeg_file_name);

	const int err2 = mv_destroy_source(source);
	if (MEDIA_VISION_ERROR_NONE != err2) {
		printf("ERROR: Error occurred when try to destroy Media Vision source."
			"Error code: %i\n", err2);
	}

	const int err3 = mv_destroy_engine_config(mv_engine_config);
	if (MEDIA_VISION_ERROR_NONE != err3) {
		printf("ERROR: Errors were occurred during destroying the media engine "
			"config: %i\n", err);
	}

	MEDIA_VISION_FUNCTION_LEAVE();

	return err;
}

int detect_barcode(barcode_model_s model, mv_rectangle_s roi)
{
	MEDIA_VISION_FUNCTION_ENTER();

	unsigned char *data_buffer = NULL;
	unsigned long buffer_size = 0;
	image_data_s image_data;

	int err = load_image_to_buffer(
			model.file_name, &data_buffer, &buffer_size, &image_data);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf("ERROR: Errors were occurred during opening the file!!! code: %i\n", err);

		MEDIA_VISION_FUNCTION_LEAVE();

		return err;
	}

	unsigned char *converted_buffer = NULL;
	unsigned long converted_buffer_size = 0;
	err = convert_rgb_to(data_buffer, &converted_buffer, image_data, model.colorspace, &converted_buffer_size);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf("ERROR: Can't convert to the selected colorspace!!! code: %i\n", err);

		MEDIA_VISION_FUNCTION_LEAVE();

		return err;
	}

	model.out_buffer_ptr = data_buffer;

	mv_engine_config_h mv_engine_config;
	err = mv_create_engine_config(&mv_engine_config);
	if (MEDIA_VISION_ERROR_NONE != err)
		printf("ERROR: Errors were occurred during creating the media engine config: %i\n", err);

	mv_engine_config_foreach_supported_attribute(_mv_engine_config_supported_attribute, mv_engine_config);

	mv_engine_config_set_int_attribute(mv_engine_config, MV_BARCODE_DETECT_ATTR_TARGET, MV_BARCODE_DETECT_ATTR_TARGET_2D_BARCODE);

	mv_source_h source;
	err = mv_create_source(&source);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf("ERROR: Errors were occurred during creating the source!!! code: %i\n", err);

		MEDIA_VISION_FUNCTION_LEAVE();

		return err;
	}

	err = mv_source_fill_by_buffer(source, converted_buffer, converted_buffer_size,
			image_data.image_width, image_data.image_height, model.colorspace);
	if (MEDIA_VISION_ERROR_NONE != err) {
		printf("ERROR: Errors were occurred during filling the source!!! code: %i\n", err);

		MEDIA_VISION_FUNCTION_LEAVE();

		return err;
	}

	if (converted_buffer != NULL)
		free(converted_buffer);

	err = mv_barcode_detect(source, mv_engine_config, roi, barcode_detected_cb, &model);

	if (data_buffer != NULL)
		destroy_loaded_buffer(data_buffer);

	if (MEDIA_VISION_ERROR_NONE != err) {
		printf("ERROR: Errors were occurred during barcode detection!!! code: %i\n", err);

		MEDIA_VISION_FUNCTION_LEAVE();

		return err;
	}

	err = mv_destroy_source(source);
	if (MEDIA_VISION_ERROR_NONE != err)
		printf("ERROR: Errors were occurred during destroying the source!!! code: %i\n", err);

	err = mv_destroy_engine_config(mv_engine_config);
	if (MEDIA_VISION_ERROR_NONE != err)
		printf("ERROR: Error were occurred during destroying the source!!! code: %i\n", err);

	MEDIA_VISION_FUNCTION_LEAVE();

	return err;
}

int input_string(const char *prompt, size_t max_len, char **string)
{
	MEDIA_VISION_FUNCTION_ENTER();

	printf("\n");
	printf("%s ", prompt);

	if (scanf("\n") != 0) {
		MEDIA_VISION_FUNCTION_LEAVE();
		return -1;
	}

	char buffer[max_len];
	int last_char = 0;
	buffer[last_char] = '\0';
	buffer[sizeof(buffer) - 1] = ~'\0';
	if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
		MEDIA_VISION_FUNCTION_LEAVE();
		return -1;
	}
	size_t real_string_len = strlen(buffer);
	buffer[real_string_len - 1] = '\0';
	*string = (char*)malloc(real_string_len * sizeof(char));
	if (*string == NULL) {
		MEDIA_VISION_FUNCTION_LEAVE();
		return -1;
	}

	strncpy(*string, buffer, real_string_len);

	size_t str_len = strlen(*string);

	MEDIA_VISION_FUNCTION_LEAVE();

	return str_len;
}

int input_size(const char *prompt, size_t max_size, size_t *size)
{
	MEDIA_VISION_FUNCTION_ENTER();

	printf("\n");
	printf("%s ", prompt);

	if (scanf("%20zu", size) == 0) {
		if (scanf("%*[^\n]%*c") != 0) {
			printf("ERROR: Reading the input line error.\n");
			MEDIA_VISION_FUNCTION_LEAVE();
			return -1;
		}
		printf("ERROR: Incorrect input.\n");
		MEDIA_VISION_FUNCTION_LEAVE();
		return -1;
	}

	int ret = (*size > max_size ? -1 : 0);

	MEDIA_VISION_FUNCTION_LEAVE();

	return ret;
}

int input_int(const char *prompt, int min_value, int max_value, int *value)
{
	MEDIA_VISION_FUNCTION_ENTER();

	printf("\n");
	printf("%s ", prompt);

	if (scanf("%20i", value) == 0) {
		if (scanf("%*[^\n]%*c") != 0) {
			printf("ERROR: Reading the input line error.\n");
			MEDIA_VISION_FUNCTION_LEAVE();
			return -1;
		}
		printf("ERROR: Incorrect input.\n");
		MEDIA_VISION_FUNCTION_LEAVE();
		return -1;
	}

	int ret = (*value < min_value || *value > max_value ? -1 : 0);

	MEDIA_VISION_FUNCTION_LEAVE();

	return ret;
}

int show_menu(const char *title, const int *options, const char **names, int cnt)
{
	MEDIA_VISION_FUNCTION_ENTER();

	printf("***************************\n");
	printf("* %23s *\n", title);
	printf("*-------------------------*\n");
	int i = 0;
	for (i = 0; i < cnt; ++i)
		printf("* %2i. %19s *\n", options[i], names[i]);

	printf("***************************\n\n");
	int selection = 0;
	printf("Your choise: ");
	if (scanf("%20i", &selection) == 0) {
		if (scanf("%*[^\n]%*c") != 0) {
			printf("ERROR: Reading the input line error.\n");
			MEDIA_VISION_FUNCTION_LEAVE();
			return -1;
		}
		printf("ERROR: Incorrect input.\n");
	}

	MEDIA_VISION_FUNCTION_LEAVE();

	return selection;
}

mv_barcode_type_e select_type(void)
{
	mv_barcode_type_e selected_type = MV_BARCODE_UNDEFINED;
	int sel_opt = 0;
	const int options[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
	const char *names[8] = { "qr", "upca", "upce", "ean8", "ean13", "code39", "code128", "interleave25" };

	MEDIA_VISION_FUNCTION_ENTER();

	while (sel_opt == 0) {
		sel_opt = show_menu("Select barcode type:", options, names, 8);

		switch (sel_opt) {
		case 1:
			selected_type = MV_BARCODE_QR;
			break;
		case 2:
			selected_type = MV_BARCODE_UPC_A;
			break;
		case 3:
			selected_type = MV_BARCODE_UPC_E;
			break;
		case 4:
			selected_type = MV_BARCODE_EAN_8;
			break;
		case 5:
			selected_type = MV_BARCODE_EAN_13;
			break;
		case 6:
			selected_type = MV_BARCODE_CODE39;
			break;
		case 7:
			selected_type = MV_BARCODE_CODE128;
			break;
		case 8:
			selected_type = MV_BARCODE_I2_5;
			break;
		default:
			sel_opt = 0;
			break;
		}
	}

	MEDIA_VISION_FUNCTION_LEAVE();

	return selected_type;
}

mv_barcode_qr_mode_e select_mode(void)
{
	mv_barcode_qr_mode_e selected_mode = MV_BARCODE_QR_MODE_UNAVAILABLE;
	int sel_opt = 0;
	const int options[4] = { 1, 2, 3, 4 };
	const char *names[4] = { "numeric", "alphanumeric", "byte", "utf8" };

	MEDIA_VISION_FUNCTION_ENTER();

	while (sel_opt == 0) {
		sel_opt = show_menu("Select encoding mode:", options, names, 4);
		switch (sel_opt) {
		case 1:
			selected_mode = MV_BARCODE_QR_MODE_NUMERIC;
			break;
		case 2:
			selected_mode = MV_BARCODE_QR_MODE_ALPHANUMERIC;
			break;
		case 3:
			selected_mode = MV_BARCODE_QR_MODE_BYTE;
			break;
		case 4:
			selected_mode = MV_BARCODE_QR_MODE_UTF8;
			break;
		default:
			sel_opt = 0;
			break;
		}
	}

	MEDIA_VISION_FUNCTION_LEAVE();

	return selected_mode;
}

mv_barcode_qr_ecc_e select_ecc(void)
{
	mv_barcode_qr_ecc_e selected_ecc = MV_BARCODE_QR_ECC_UNAVAILABLE;
	int sel_opt = 0;
	const int options[4] = { 1, 2, 3, 4 };
	const char *names[4] = { "low", "medium", "quartile", "high" };

	MEDIA_VISION_FUNCTION_ENTER();

	while (sel_opt == 0) {
		sel_opt = show_menu("Select ECC level:", options, names, 4);
		switch (sel_opt) {
		case 1:
			selected_ecc = MV_BARCODE_QR_ECC_LOW;
			break;
		case 2:
			selected_ecc = MV_BARCODE_QR_ECC_MEDIUM;
			break;
		case 3:
			selected_ecc = MV_BARCODE_QR_ECC_QUARTILE;
			break;
		case 4:
			selected_ecc = MV_BARCODE_QR_ECC_HIGH;
			break;
		default:
			sel_opt = 0;
			break;
		}
	}

	MEDIA_VISION_FUNCTION_LEAVE();

	return selected_ecc;
}

int select_version(void)
{
	MEDIA_VISION_FUNCTION_ENTER();

	int sel_opt = 0;
	while (sel_opt == 0) {
		const int options[2] = {1, 40};
		const char *names[2] = { "1..", "..40" };
		sel_opt = show_menu("Select QR version:", options, names, 2);
		if (sel_opt < 1 || sel_opt > 40)
			sel_opt = 0;
	}

	MEDIA_VISION_FUNCTION_LEAVE();

	return sel_opt;
}

generation_fcn_e select_gen_function(void)
{
	generation_fcn_e ret_fcn_type = MV_TS_GENERATE_TO_IMAGE_FCN;
	int sel_opt = 0;
	const int options[2] = { 1, 2 };
	const char *names[2] = { "Generate to file", "Generate to source" };

	MEDIA_VISION_FUNCTION_ENTER();

	while (sel_opt == 0) {
		sel_opt = show_menu("Select API function:", options, names, 2);
		switch (sel_opt) {
		case 1:
			ret_fcn_type = MV_TS_GENERATE_TO_IMAGE_FCN;
			break;
		case 2:
			ret_fcn_type = MV_TS_GENERATE_TO_SOURCE_FCN;
			break;
		default:
			sel_opt = 0;
			break;
		}
	}

	MEDIA_VISION_FUNCTION_LEAVE();

	return ret_fcn_type;
}

mv_barcode_image_format_e select_file_format(void)
{
	mv_barcode_image_format_e image_format = MV_BARCODE_IMAGE_FORMAT_JPG;
	int sel_opt = 0;
	const int options[3] = { 1, 2, 3 };
	const char *names[3] = { "BMP", "JPG", "PNG" };

	MEDIA_VISION_FUNCTION_ENTER();

	while (sel_opt == 0) {
		sel_opt = show_menu("Select file format:", options, names, 3);
		switch (sel_opt) {
		case 1:
			image_format = MV_BARCODE_IMAGE_FORMAT_BMP;
			break;
		case 2:
			image_format = MV_BARCODE_IMAGE_FORMAT_JPG;
			break;
		case 3:
			image_format = MV_BARCODE_IMAGE_FORMAT_PNG;
			break;
		default:
			sel_opt = 0;
			break;
		}
	}

	MEDIA_VISION_FUNCTION_LEAVE();

	return image_format;
}

int perform_detect()
{
	MEDIA_VISION_FUNCTION_ENTER();

	barcode_model_s detect_model = {
		MV_BARCODE_UNDEFINED,
		MV_BARCODE_QR_ECC_UNAVAILABLE,
		MV_BARCODE_QR_MODE_UNAVAILABLE,
		0, 0, 0,
		MV_BARCODE_IMAGE_FORMAT_PNG,
		MEDIA_VISION_COLORSPACE_INVALID,
		NULL, NULL, NULL, NULL };

	while (input_string("Input file name to be analyzed:", 1024, &(detect_model.file_name)) == -1)
		printf("Incorrect input! Try again.\n");

	LOGI("Barcode input image has been specified");

	mv_rectangle_s roi = { {0, 0}, 0, 0 };

	while (input_int("Input x coordinate for ROI top left vertex:", 0, 10000, &(roi.point.x)) == -1)
		printf("Incorrect input! Try again.\n");

	while (input_int("Input y coordinate for ROI top left vertex:", 0, 10000, &(roi.point.y)) == -1)
		printf("Incorrect input! Try again.\n");

	while (input_int("Input ROI width:", 0, 10000, &(roi.width)) == -1)
		printf("Incorrect input! Try again.\n");

	while (input_int("Input ROI height:", 0, 10000, &(roi.height)) == -1)
		printf("Incorrect input! Try again.\n");

	LOGI("Region of interest (ROI) to detect barcode into has been specified");

	while (input_string("Input file name to be generated:", 1024, &(detect_model.out_file_name)) == -1)
		printf("Incorrect input! Try again.\n");

	LOGI("Barcode output image has been specified");

	const int options[11] = { MEDIA_VISION_COLORSPACE_Y800,
								MEDIA_VISION_COLORSPACE_I420,
								MEDIA_VISION_COLORSPACE_NV12,
								MEDIA_VISION_COLORSPACE_YV12,
								MEDIA_VISION_COLORSPACE_NV21,
								MEDIA_VISION_COLORSPACE_YUYV,
								MEDIA_VISION_COLORSPACE_UYVY,
								MEDIA_VISION_COLORSPACE_422P,
								MEDIA_VISION_COLORSPACE_RGB565,
								MEDIA_VISION_COLORSPACE_RGB888,
								MEDIA_VISION_COLORSPACE_RGBA };
	const char *names[11] = { "Y800", "I420", "NV12", "YV12", "NV21",
								"YUYV", "UYVY", "422P", "RGB565",
								"RGB888", "RGBA" };

	while (true) {
		int sel_opt = show_menu("Select colorspace to test detector on:", options, names, 11);
		if (sel_opt < MEDIA_VISION_COLORSPACE_Y800 ||
			sel_opt > MEDIA_VISION_COLORSPACE_RGBA) {
			continue;
		}
		detect_model.colorspace = (mv_colorspace_e)sel_opt;
		LOGI("User selection is %i", sel_opt);
		break;
	}

	int err = detect_barcode(detect_model, roi);

	if (detect_model.file_name != NULL)
		free(detect_model.file_name);

	if (detect_model.out_file_name != NULL)
		free(detect_model.out_file_name);

	if (err != MEDIA_VISION_ERROR_NONE)
		LOGE("Barcode detection failed with error code (0x%08x)", err);

	MEDIA_VISION_FUNCTION_LEAVE();

	return err;
}

int perform_generate(void)
{
	MEDIA_VISION_FUNCTION_ENTER();

	barcode_model_s generate_model = {
			MV_BARCODE_UNDEFINED,
			MV_BARCODE_QR_ECC_UNAVAILABLE,
			MV_BARCODE_QR_MODE_UNAVAILABLE,
			0, 0, 0,
			MV_BARCODE_IMAGE_FORMAT_PNG,
			MEDIA_VISION_COLORSPACE_INVALID,
			NULL, NULL, NULL, NULL };

	generation_fcn_e gen_fcn = select_gen_function();
	generate_model.type = select_type();
	LOGI("Barcode type has been selected");

	if (generate_model.type == MV_BARCODE_QR) {
		generate_model.mode = select_mode();
		LOGI("Barcode encoding mode has been selected");
		generate_model.ecc = select_ecc();
		LOGI("Barcode ecc level has been selected");
		generate_model.version = select_version();
		LOGI("Barcode version has been selected");
	}

	if (gen_fcn == MV_TS_GENERATE_TO_IMAGE_FCN) {
		generate_model.out_image_format = select_file_format();
		LOGI("Barcode output image format has been selected");
	}

	while (input_string("Input message:", 7089, &generate_model.message) == -1)
		printf("Incorrect input! Try again.\n");

	LOGI("Barcode message has been specified");

	while (input_string("Input file name:", 1024, &generate_model.file_name) == -1)
		printf("Incorrect input! Try again.\n");

	LOGI("Barcode output file name has been specified");

	if (gen_fcn == MV_TS_GENERATE_TO_IMAGE_FCN) {
		while (input_size("Input image width:", 10000, &generate_model.width) == -1)
			printf("Incorrect input! Try again.\n");

		LOGI("Barcode output file width has been specified");

		while (input_size("Input image height:", 10000, &generate_model.height) == -1)
			printf("Incorrect input! Try again.\n");

		LOGI("Barcode output file height has been specified");
	}

	const int err =
			gen_fcn == MV_TS_GENERATE_TO_IMAGE_FCN ?
			generate_barcode_to_image(generate_model) :
			generate_barcode_to_source(generate_model);

	if (generate_model.message != NULL)
		free(generate_model.message);

	if (generate_model.file_name != NULL)
		free(generate_model.file_name);

	if (err != MEDIA_VISION_ERROR_NONE) {
		LOGE("Barcode generation failed with error code (0x%08x)", err);
		printf("ERROR: Errors were occurred during barcode generation!!!\n");
		MEDIA_VISION_FUNCTION_LEAVE();
		return err;
	}

	LOGI("Barcode output file has been generated");
	printf("\nBarcode image was successfully generated.\n");

	MEDIA_VISION_FUNCTION_LEAVE();

	return 0;
}

int main(void)
{
	LOGI("Media Vision Testsuite is launched.");

	int err = MEDIA_VISION_ERROR_NONE;

	int sel_opt = 0;
	const int options[2] = { 1, 2 };
	const char *names[2] = { "Generate", "Detect" };

	while (sel_opt == 0) {
		sel_opt = show_menu("Select action:", options, names, 2);
		switch (sel_opt) {
		case 1:
			LOGI("Start the barcode generation flow");
			err = perform_generate();
			break;
		case 2:
			LOGI("Start the barcode detection flow");
			err = perform_detect();
			break;
		default:
			sel_opt = 0;
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
				break;
			}
		}
		LOGI("User selection is %i", sel_opt);

		sel_opt = (do_another == 1 ? 0 : sel_opt);
	}

	LOGI("Media Vision Testsuite is closed.");

	return err;
}
