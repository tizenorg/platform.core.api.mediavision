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

#ifndef __TIZEN_MEDIAVISION_BARCODE_UTILS_H__
#define __TIZEN_MEDIAVISION_BARCODE_UTILS_H__

#include "mv_common.h"

namespace zbar {
class Image;
}

namespace MediaVision {
namespace Barcode {

/**
 * @brief    This function converts media vision image handle to zbar image handle.
 *
 * @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
 * @param [in]  mvSource      Media vision image handle
 * @param [out] zbarSource    Zbar image handle
 * @return @c MEDIA_VISION_ERROR_NONE on success,
           otherwise a negative error value
 */
int convertSourceMV2Zbar(mv_source_h mvSource, zbar::Image& zbarSource);

} /* Barcode */
} /* MediaVision */

#endif /* __TIZEN_MEDIAVISION_BARCODE_UTILS_H__ */
