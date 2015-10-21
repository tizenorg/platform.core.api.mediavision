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

#include "Barcode.h"

#include <mv_private.h>

namespace MediaVision {
namespace Barcode {

Barcode::Barcode(const zbar::Symbol& barObj):
					m_pBarcodeObj(new zbar::Symbol(barObj))
{
    ; /* NULL */
}

Barcode::~Barcode()
{
	LOGI("Delete ZBar object");
	delete m_pBarcodeObj;
}

std::string Barcode::getMessage(void) const
{
	LOGI("Retrieve message data from ZBar object");
	return m_pBarcodeObj->get_data();
}

mv_barcode_type_e Barcode::getType(void) const
{
	zbar::zbar_symbol_type_t barcodeType = m_pBarcodeObj->get_type();

	switch (barcodeType) {
	case zbar::ZBAR_QRCODE:
		return MV_BARCODE_QR;
	case zbar::ZBAR_UPCA:
		return MV_BARCODE_UPC_A;
	case zbar::ZBAR_UPCE:
		return MV_BARCODE_UPC_E;
	case zbar::ZBAR_EAN8:
		return MV_BARCODE_EAN_8;
	case zbar::ZBAR_EAN13:
		return MV_BARCODE_EAN_13;
	case zbar::ZBAR_CODE128:
		return MV_BARCODE_CODE128;
	case zbar::ZBAR_CODE39:
		return MV_BARCODE_CODE39;
	case zbar::ZBAR_I25:
		return MV_BARCODE_I2_5;
	default:
		LOGE("ZBar symbol colorspace is not supported by media vision");
		return MV_BARCODE_UNDEFINED;
	}
}

int Barcode::calculateLocation(mv_quadrangle_s& location) const
{
	const int numberOfVertexes = 4;

	const int locationPolygonSize = m_pBarcodeObj->get_location_size();

	/*polygon location should contain at least 4 points */
	if (locationPolygonSize < numberOfVertexes) {
		LOGW("Can't compute location of the barcode by %i points (less then %i).", locationPolygonSize, numberOfVertexes);
		return MEDIA_VISION_ERROR_INVALID_OPERATION;
	}

	if (locationPolygonSize == numberOfVertexes) {
		for (int i = 0; i < numberOfVertexes; ++i) {
			location.points[i].x = m_pBarcodeObj->get_location_x(i);
			location.points[i].y = m_pBarcodeObj->get_location_y(i);
		}

		return MEDIA_VISION_ERROR_NONE;
	}

	/* bounding quadrangle is computing by 4 marginal points */
	mv_point_s first = {m_pBarcodeObj->get_location_x(0), m_pBarcodeObj->get_location_y(0)};

	int minX = first.x;
	int maxX = first.x;
	int minY = first.y;
	int maxY = first.y;

	for (int i = 0; i < locationPolygonSize; ++i) {
		mv_point_s current = {m_pBarcodeObj->get_location_x(i), m_pBarcodeObj->get_location_y(i)};
		if (current.x < minX) {
			minX = current.x;
		} else if (current.x > maxX) {
			maxX = current.x;
		}

		if (current.y < minY) {
			minY = current.y;
		} else if (current.y > maxY) {
			maxY = current.y;
		}
	}

	mv_point_s bottomLeft = {minX, maxY};
	mv_point_s bottomRight = {maxX, maxY};
	mv_point_s topRight = {maxX, minY};
	mv_point_s topLeft = {minX, minY};

	location.points[0] = topLeft;
	location.points[1] = topRight;
	location.points[2] = bottomRight;
	location.points[3] = bottomLeft;

	return MEDIA_VISION_ERROR_NONE;
}

} /* Barcode */
} /* MediaVision */
