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

#ifndef __MV_TESTSUITE_COMMON_H__
#define __MV_TESTSUITE_COMMON_H__

#include "mv_common.h"

#include <stddef.h>

typedef enum
{
    FAIL_OR_SUCCESSS,
    FAIL_OR_DONE
} notification_type_e;

/**
 * @brief Prints success result of action.
 *
 * @since_tizen 3.0
 * @param [in] action_name           Name of action which result will be printed
 * @param [in] action_return_value   Return value of action
 */
void print_fail_result(
        const char *action_name,
        int action_return_value);

/**
 * @brief Prints success result of action.
 *
 * @since_tizen 3.0
 * @param [in] action_name           Name of action which result will be printed
 */
void print_done_result(const char *action_name);

/**
 * @brief Prints success result of action.
 *
 * @since_tizen 3.0
 * @param [in] action_name           Name of action which result will be printed
 */
void print_success_result(const char *action_name);

/**
 * @brief Prints action result.
 *
 * @since_tizen 3.0
 * @param [in] action_name           Name of action which result will be printed
 * @param [in] action_return_value   Return value of action
 * @param [in] notification_type_e   Type of notification
 */
void print_action_result(
        const char *action_name,
        int action_return_value,
        notification_type_e notification_type_e);

/**
 * @brief Gets srting from console.
 *
 * @since_tizen 3.0
 * @param [in] prompt     The prompt before getting string value
 * @param [in] max_len    Maximum length of the string which will be got
 * @param [out] string    Output string which was got from console
 * @return Length of the output string on success, otherwise a negative error value
 */
int input_string(const char *prompt, size_t max_len, char **string);

/**
 * @brief Gets unsigned integer from console.
 *
 * @since_tizen 3.0
 * @param [in] prompt      The prompt before getting unsigned integer value
 * @param [in] max_size    The thresold for maximum possible value
 * @param [out] size       The output unsigned integer which was got from console
 * @return @c 0 on success, otherwise a negative error value
 */
int input_size(const char *prompt, size_t max_size, size_t *size);

/**
 * @brief Gets integer from console.
 *
 * @since_tizen 3.0
 * @param [in] prompt       The prompt before getting integer value
 * @param [in] min_value    The thresold for minimum possible value
 * @param [in] max_value    The thresold for maximum possible value
 * @param [out] value       The output integer which was got from console
 * @return @c 0 on success, otherwise a negative error value
 */
int input_int(const char *prompt, int min_value, int max_value, int *value);

/**
 * @brief Gets double from console.
 *
 * @since_tizen 3.0
 * @param [in] prompt       The prompt before getting double value
 * @param [in] min_value    The thresold for minimum possible value
 * @param [in] max_value    The thresold for maximum possible value
 * @param [out] value       The output double which was got from console
 * @return @c 0 on success, otherwise a negative error value
 */
int input_double(const char *prompt, double min_value, double max_value, double *value);

/**
 * @brief Shows confirm dialog in console and gets answer (Yes/No).
 *
 * @since_tizen 3.0
 * @param [in] title    The title for confirm dialog which will be printed
 *                      before input of the answer
 * @return false if answer is "No" and true if answer is "Yes"
 */
bool show_confirm_dialog(const char *title);

/**
 * @brief Shows menu in console and allows to get item from the array of options.
 *
 * @since_tizen 3.0
 * @param [in] title               The title for show menu which will be printed
 *                                 before options
 * @param [in] options             The array with integer numbers of options
 *                                 which will be shown
 * @param [in] names               The array with names of options which will
 *                                 be shown
 * @param [in] number_of_option    The number of options which will be shown
 * @return The selected item positive number from options array on success,
 *         otherwise a negative error value
 */
int show_menu(
        const char *title,
        const int *options,
        const char **names,
        int number_of_option);

/**
 * @brief Loads media source from JPEG image.
 *
 * @since_tizen 3.0
 * @param [in] path_to_image    The path to JPEG image which will be loaded
 * @param [in/out] source       The hadler to media source which will be filled
 * @return @c 0 on success, otherwise a negative error value
 */
int load_mv_source_from_file(
        const char *path_to_image,
        mv_source_h source);

#endif /* __MV_TESTSUITE_COMMON_H__ */
