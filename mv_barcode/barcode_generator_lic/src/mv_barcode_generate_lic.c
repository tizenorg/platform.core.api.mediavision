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

#include "mv_barcode_generate_lic.h"

int mv_barcode_generate_source_lic(
        mv_engine_config_h engine_cfg,
        const char *message,
        mv_barcode_type_e type,
        mv_barcode_qr_mode_e qr_enc_mode,
        mv_barcode_qr_ecc_e qr_ecc,
        int qr_version,
        mv_source_h image)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;;
}

int mv_barcode_generate_image_lic(
        mv_engine_config_h engine_cfg,
        const char *message,
        int image_width,
        int image_height,
        mv_barcode_type_e type,
        mv_barcode_qr_mode_e qr_enc_mode,
        mv_barcode_qr_ecc_e qr_ecc,
        int qr_version,
        const char *image_path,
        mv_barcode_image_format_e image_format)
{
    return MEDIA_VISION_ERROR_NOT_SUPPORTED;
}

