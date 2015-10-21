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

#ifndef __BARCODE_H__
#define __BARCODE_H__

#include "mv_barcode.h"

#include <stddef.h>
#include <zbar.h>

namespace MediaVision {
namespace Barcode {
/**
 * @class    Barcode
 * @brief    Handle to barcode object.
 */
class Barcode {
public:
	/**
	 * @brief   Barcode constructor.
	 *
	 * @since_tizen 2.4
	 * @remarks create copy of bar_obj
	 * @param   [in] barObj zbar barcode handle
	 *
	 */
	Barcode(const zbar::Symbol& barObj);

	/**
	 * @brief  Barcode destructor.
	 *
	 * @since_tizen 2.4
	 */
	~Barcode();

	/**
	 * @brief  Gets encoded message from barcode object.
	 *
	 * @since_tizen 2.4
	 * @return Encoded message
	 */
	std::string getMessage(void) const;

	/**
	 * @brief Gets the type of the barcode.
	 *
	 * @since_tizen 2.4
	 * @return Enumeration value corresponding to the barcode type
	 */
	mv_barcode_type_e getType(void) const;

	/**
	 * @brief  Calculates location of barcode handle from zbar.
	 *         location polygon.
	 *
	 * @since_tizen 2.4
	 * @param  [out] location    Quadrangle that contains barcode on image
	 * @return @c MEDIA_VISION_ERROR_NONE on success,
	 *         otherwise a negative error value
	 */
	int calculateLocation(mv_quadrangle_s& location) const;

private:
	const zbar::Symbol *m_pBarcodeObj; ///< Pointer to zbar barcode handle
};

} /* Barcode */
} /* MediaVision */

#endif /* __BARCODE_H__ */
