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

#include "BarcodeGenerator.h"

#include <mv_private.h>

#include <zint.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <cstring>
#include <vector>
#include <unistd.h>

namespace MediaVision {
namespace Barcode {

namespace {
int getFormatEncodingInfo(
		BarcodeImageFormat imageFormat,
		std::vector<std::string>& extensions,
		std::vector<int>& compressionParams)
{
	static const int PNGCompressionLevel = 3;

	compressionParams.clear();
	extensions.clear();

	switch (imageFormat) {
	case BARCODE_IMAGE_PNG:
		compressionParams.push_back(CV_IMWRITE_PNG_COMPRESSION);
		compressionParams.push_back(PNGCompressionLevel);
		extensions.push_back(".png");
		break;
	case BARCODE_IMAGE_JPG:
		extensions.push_back(".jpg");
		extensions.push_back(".jpeg");
		extensions.push_back(".jpe");
		break;
	case BARCODE_IMAGE_BMP:
		extensions.push_back(".bmp");
		extensions.push_back(".dib");
		break;
	default:
		return BARCODE_ERROR_INVALID_OPTION;
	}
	return BARCODE_ERROR_NONE;
}

int createBarcode(
		const std::string& message,
		BarcodeType type,
		BarcodeQREncodingMode encodingMode,
		BarcodeQRErrorCorrectionLevel correctionLevel,
		int qrVersion,
		int showText,
		char *fgcolour,
		char *bgcolour,
		zint_symbol *symbol)
{
	/* set input values */
	symbol->symbology = type;
	symbol->input_mode = encodingMode;
	symbol->option_1 = correctionLevel;
	symbol->option_2 = qrVersion;
	symbol->scale = 1;
	symbol->show_hrt = showText;

	/* set default values */
	if (fgcolour) {
		std::strncpy(symbol->fgcolour, fgcolour, 10);
		if (strlen(fgcolour) > 9) {
			symbol->fgcolour[9] = '\0';
		}
	} else {
		std::strncpy(symbol->fgcolour, "000000", 10);
	}

	if (bgcolour) {
		std::strncpy(symbol->bgcolour, bgcolour, 10);
		if (strlen(bgcolour) > 9) {
			symbol->bgcolour[9] = '\0';
		}
	} else {
		std::strncpy(symbol->bgcolour, "ffffff", 10);
	}

	LOGI("Check colors: front %s, back %s", symbol->fgcolour, symbol->bgcolour);

	symbol->border_width = 1;
	symbol->height = 50;

	if (type == BARCODE_QR) {
		symbol->whitespace_width = 0;
	} else {
		symbol->whitespace_width = 10;
	}

	/* create barcode */
	const int rotationAngle = 0;
	int error = ZBarcode_Encode_and_Buffer(
					symbol,
					(unsigned char*)(message.c_str()),
					message.length(),
					rotationAngle);

	return error;
}

int writeBufferToImageFile(
		zint_symbol *symbol,
		const std::string& imageFileName,
		BarcodeImageFormat imageFormat,
		const int imageWidth,
		const int imageHeight)
{
	if (imageWidth <= 0 || imageHeight <= 0) {
		LOGE("Barcode image size is invalid: %i x %i. Terminate write to "
			"the image operation", imageWidth, imageHeight);
		return BARCODE_ERROR_INVALID_DATA;
	}

	/* find directory */
	std::string prefix_imageFileName = imageFileName.substr(0, imageFileName.find_last_of('/'));
	LOGD("prefix_path: %s", prefix_imageFileName.c_str());

	/* check the directory is available */
	if (access(prefix_imageFileName.c_str(), F_OK)) {
		LOGE("Can't save barcode image to the path. The path[%s] doesn't existed.", prefix_imageFileName.c_str());
		return BARCODE_ERROR_INVALID_PATH;
	}

	/* check current extension */
	std::vector<std::string> expectedExtensions;
	std::vector<int> compressionParams;

	int error = getFormatEncodingInfo(imageFormat,
				expectedExtensions, compressionParams);

	if (BARCODE_ERROR_NONE != error || expectedExtensions.empty()) {
		LOGE("Image format is incorrectly specified or not supported");
		return error;
	}

	bool rightExtensionFlag = false;

	std::string resultFilePath(imageFileName);

	for (size_t extNum = 0; extNum < expectedExtensions.size(); ++extNum) {
		if (resultFilePath.size() >= expectedExtensions[extNum].size()) {
			std::string givenExtension = resultFilePath.substr(
				resultFilePath.length() - expectedExtensions[extNum].size(),
				expectedExtensions[extNum].size());

			std::transform(
				givenExtension.begin(), givenExtension.end(),
				givenExtension.begin(), ::tolower);

			if (givenExtension == expectedExtensions[extNum]) {
				rightExtensionFlag = true;
				break;
			}
		}
	}

	if (!rightExtensionFlag) {
		resultFilePath += expectedExtensions[0];
	}

	cv::Mat image(symbol->bitmap_height, symbol->bitmap_width, CV_8UC3, symbol->bitmap);
	cv::resize(image, image, cv::Size(imageWidth, imageHeight), 0, 0, cv::INTER_AREA);

	error = cv::imwrite(resultFilePath, image, compressionParams) ?
			BARCODE_ERROR_NONE : BARCODE_ERROR_INVALID_DATA;

	if (BARCODE_ERROR_NONE != error) {
		LOGE("Write barcode image to file %s operation failed.",
		resultFilePath.c_str());
		return error;
	}

	return error;
}

} /* anonymous namespace */

int BarcodeGenerator::generateBarcodeToImage(
		const std::string& imageFileName,
		BarcodeImageFormat imageFormat,
		const int imageWidth,
		const int imageHeight,
		const std::string& message,
		BarcodeType type,
		BarcodeQREncodingMode encodingMode,
		BarcodeQRErrorCorrectionLevel correctionLevel,
		int qrVersion,
		int showText,
		char *fgcolour,
		char *bgcolour)
{
	zint_symbol *symbol = ZBarcode_Create();

	if(symbol == NULL) {
		LOGE("ZBarcode creation failed");
		return BARCODE_ERROR_ENCODING_PROBLEM;
	}

	int error = createBarcode(
					message,
					type,
					encodingMode,
					correctionLevel,
					qrVersion,
					showText,
					fgcolour,
					bgcolour,
					symbol);

	if (error != BARCODE_ERROR_NONE) {
		LOGE("Barcode creation failed, clean memory");
		ZBarcode_Delete(symbol);
		return error;
	}

	error = writeBufferToImageFile(
				symbol,
				imageFileName,
				imageFormat,
				imageWidth,
				imageHeight);
	if (error != BARCODE_ERROR_NONE) {
		LOGE("Barcode [%s] file write fail, clean memory", imageFileName.c_str());
	} else {
		LOGI("Barcode image [%s] is successfully generated, clean memory", imageFileName.c_str());
	}

	ZBarcode_Delete(symbol);

	return error;
}

int BarcodeGenerator::generateBarcodeToBuffer(
		unsigned char **imageBuffer,
		unsigned int *imageWidth,
		unsigned int *imageHeight,
		unsigned int *imageChannels,
		const std::string& message,
		BarcodeType type,
		BarcodeQREncodingMode encodingMode,
		BarcodeQRErrorCorrectionLevel correctionLevel,
		int qrVersion,
		int showText,
		char *fgcolour,
		char *bgcolour)
{
	zint_symbol *symbol = ZBarcode_Create();

	if(symbol == NULL) {
		LOGE("ZBarcode creation failed");

		return BARCODE_ERROR_ENCODING_PROBLEM;
	}

	int error = createBarcode(
					message,
					type,
					encodingMode,
					correctionLevel,
					qrVersion,
					showText,
					fgcolour,
					bgcolour,
					symbol);

	if (error != BARCODE_ERROR_NONE) {
		LOGE("Barcode creation failed, clean memory");
		ZBarcode_Delete(symbol);
		return error;
	}

	/* fill output buffer */
	*imageWidth = symbol->bitmap_width;
	*imageHeight = symbol->bitmap_height;
	*imageChannels = 3;
	const unsigned int imageBufferSize = (*imageWidth) * (*imageHeight) * (*imageChannels);
	*imageBuffer = new unsigned char[imageBufferSize];
	memmove(*imageBuffer, symbol->bitmap, imageBufferSize);

	LOGI("Barcode buffer has been successfully generated, clean memory");
	ZBarcode_Delete(symbol);

	return BARCODE_ERROR_NONE;
}

} /* Barcode */
} /* MediaVision */
