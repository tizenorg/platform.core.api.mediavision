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

#ifndef __MEDIASOURCE_H__
#define __MEDIASOURCE_H__

// Need for a colorspace
#include <mv_common.h>

/**
 * @file MediaSource.h
 * @brief This file contains the MediaSource class.
 */

namespace MediaVision {
namespace Common {
/**
 * @class   MediaSource
 * @brief   The Media Source container
 * @details It is class which contains Media Source information. This class
 *          will be use in the Media Vision as simple image.
 */
class MediaSource {
public:
	/**
	 * @brief   Creates a MediaSource.
	 * @details Default parameters values of the MediaSource will be: zero for
	 *          width, height and buffer size; NULL for buffer;
	 *          MEDIA_VISION_COLORSPACE_INVALID for colorspace.
	 *
	 * @since_tizen 2.4
	 *
	 * @see MediaSource::~MediaSource()
	 */
	MediaSource();

	/**
	 * @brief   Destroys the MediaSource and releases all its resources.
	 *
	 * @since_tizen 2.4
	 *
	 * @see MediaSource::MediaSource()
	 */
	virtual ~MediaSource();

	/**
	 * @brief   Clears the MediaSource.
	 * @details Releases all internal resources and set parameters to default values.
	 *
	 * @since_tizen 2.4
	 *
	 * @see MediaSource::MediaSource()
	 * @see MediaSource::fill()
	 */
	void clear(void);

	/**
	 * @brief Fills the MediaSource based on the buffer and metadata.
	 *
	 * @since_tizen 2.4
	 * @param [in] buffer       The buffer of image data
	 * @param [in] bufferSize   The buffer size
	 * @param [in] width        The image width
	 * @param [in] height       The image height
	 * @param [in] colorspace   The image colorspace
	 * @return true if filled process is ok. Otherwise return false.
	 *
	 * @see MediaSource::MediaSource()
	 * @see MediaSource::clear()
	 */
	bool fill(const unsigned char *buffer, unsigned int bufferSize, unsigned int
			width, unsigned int height, mv_colorspace_e colorspace);

	/**
	 * @brief Gets data buffer of the MediaSource.
	 *
	 * @since_tizen 2.4
	 *  @return Pointer to the data buffer.
	 */
	unsigned char *getBuffer(void) const;

/**
	 * @brief Gets buffer size of the MediaSource.
	 *
	 * @since_tizen 2.4
	 * @return Size of data buffer.
	 */
	unsigned int getBufferSize(void) const;

	/**
	 * @brief Gets image width of the MediaSource.
	 *
	 * @since_tizen 2.4
	 * @return Width of image.
	 */
	unsigned int getWidth(void) const;

	/**
	 * @brief Gets image height of the MediaSource.
	 *
	 * @since_tizen 2.4
	 * @return Height of image.
	 */
	unsigned int getHeight(void) const;

	/**
	 * @brief Gets image colorspace of the MediaSource.
	 *
	 * @since_tizen 2.4
	 * @return Colorspace of image.
	 */
	mv_colorspace_e getColorspace(void) const;

private:
	unsigned char *m_pBuffer;        /**< The data buffer */

	unsigned int m_bufferSize;       /**< The buffer size */

	unsigned int m_width;            /**< The image width */

	unsigned int m_height;           /**< The image height */

	mv_colorspace_e m_colorspace;    /**< The image colorspace */
};

} /* Common */
} /* MediaVision */

#endif /* __MEDIASOURCE_H__ */
