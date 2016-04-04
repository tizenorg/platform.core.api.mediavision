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

#ifndef __MV_LOG_CFG_H__
#define __MV_LOG_CFG_H__

#include <dlog.h>

/* #define ROOTSTRAP_OUT // enables logs to console */

#define TEXT_RED     "\x1b[31m"
#define TEXT_GREEN   "\x1b[32m"
#define TEXT_YELLOW  "\x1b[33m"
#define TEXT_BLUE    "\x1b[34m"
#define TEXT_MAGENTA "\x1b[35m"
#define TEXT_CYAN    "\x1b[36m"
#define TEXT_RESET   "\x1b[0m"

#ifdef ROOTSTRAP_OUT

#define LOGD(...)                                 \
do {                                              \
	printf("<%s:%d>", __FUNCTION__, __LINE__);    \
	printf(TEXT_CYAN);                            \
	printf(__VA_ARGS__);                          \
	printf(TEXT_RESET "\n");                      \
} while (0)

#define LOGI(...)                                 \
do  {                                             \
	printf("<%s:%d>", __FUNCTION__, __LINE__);    \
	printf(TEXT_GREEN);                           \
	printf(__VA_ARGS__);                          \
	printf(TEXT_RESET "\n");                      \
} while (0)

#define LOGW(...)                                 \
do {                                              \
	printf("<%s:%d>", __FUNCTION__, __LINE__);    \
	printf(TEXT_YELLOW);                          \
	printf(__VA_ARGS__);                          \
	printf(TEXT_RESET "\n");                      \
} while (0)

#define LOGE(...)                                 \
do {                                              \
	printf("<%s:%d>", __FUNCTION__, __LINE__);    \
	printf(TEXT_RED);                             \
	printf(__VA_ARGS__);                          \
	printf(TEXT_RESET "\n");                      \
} while (0)

#endif

#endif /* __MV_LOG_CFG_H__ */
