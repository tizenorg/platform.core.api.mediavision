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

#include "MediaSource.h"

#include <mv_private.h>

#include <cstring>

namespace MediaVision
{
namespace Common
{

MediaSource::MediaSource() : m_pBuffer (NULL), m_bufferSize (0), m_width (0),
        m_height (0), m_colorspace (MEDIA_VISION_COLORSPACE_INVALID)
{
}

MediaSource::~MediaSource()
{
    clear();
}

void MediaSource::clear(void)
{
    if (m_pBuffer != NULL)
    {
        LOGD("Delete internal buffer for media source %p", this);
        delete[] m_pBuffer;
    }
    LOGD("Set defaults for media source %p : buffer = NULL; "
            "bufferSize = 0; width = 0; height = 0; "
            "colorspace = MEDIA_VISION_COLORSPACE_INVALID", this);
    m_pBuffer = NULL;
    m_bufferSize = 0;
    m_width = 0;
    m_height = 0;
    m_colorspace = MEDIA_VISION_COLORSPACE_INVALID;
}

bool MediaSource::fill(const unsigned char *buffer, unsigned int bufferSize,
        unsigned int width, unsigned int height, mv_colorspace_e colorspace)
{
    if (bufferSize == 0 || buffer == NULL)
    {
        return false;
    }

    LOGD("Call clear() first for media source %p", this);
    clear();

    try
    {
        LOGD("Allocate memory for buffer in media source %p", this);
        m_pBuffer = new unsigned char[bufferSize];
    }
    catch(...)
    {
        LOGE("Memory allocating for buffer in media source %p failed!", this);
        m_pBuffer = NULL;
        return false;
    }

    LOGD("Copy data from external buffer (%p) to the internal buffer (%p) of "
            "media source %p", buffer, m_pBuffer, this);
    std::memcpy(m_pBuffer, buffer, bufferSize);

    LOGD("Assign new size of the internal buffer of media source %p. "
            "New size is %ui.", this, bufferSize);
    m_bufferSize = bufferSize;

    LOGD("Assign new size (%ui x %ui) of the internal buffer image for "
            "the media source %p", width, height, this);
    m_width = width;
    m_height = height;

    LOGD("Assign new colorspace (%i) of the internal buffer image for "
                "the media source %p", colorspace, this);
    m_colorspace = colorspace;

    return true;
}

unsigned char *MediaSource::getBuffer(void) const
{
    return m_pBuffer;
}

unsigned int MediaSource::getBufferSize(void) const
{
    return m_bufferSize;
}

unsigned int MediaSource::getWidth(void) const
{
    return m_width;
}

unsigned int MediaSource::getHeight(void) const
{
    return m_height;
}

mv_colorspace_e MediaSource::getColorspace(void) const
{
    return m_colorspace;
}

} /* Common */
} /* MediaVision */
