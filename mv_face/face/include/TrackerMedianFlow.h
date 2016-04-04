/*M///////////////////////////////////////////////////////////////////////////////////////
 //
 //  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
 //
 //  By downloading, copying, installing or using the software you agree to this license.
 //  If you do not agree to this license, do not download, install,
 //  copy or use the software.
 //
 //
 //                           License Agreement
 //                For Open Source Computer Vision Library
 //
 // Copyright (C) 2013, OpenCV Foundation, all rights reserved.
 // Third party copyrights are property of their respective owners.
 //
 // Redistribution and use in source and binary forms, with or without modification,
 // are permitted provided that the following conditions are met:
 //
 //   * Redistribution's of source code must retain the above copyright notice,
 //     this list of conditions and the following disclaimer.
 //
 //   * Redistribution's in binary form must reproduce the above copyright notice,
 //     this list of conditions and the following disclaimer in the documentation
 //     and/or other materials provided with the distribution.
 //
 //   * The name of the copyright holders may not be used to endorse or promote products
 //     derived from this software without specific prior written permission.
 //
 // This software is provided by the copyright holders and contributors "as is" and
 // any express or implied warranties, including, but not limited to, the implied
 // warranties of merchantability and fitness for a particular purpose are disclaimed.
 // In no event shall the Intel Corporation or contributors be liable for any direct,
 // indirect, incidental, special, exemplary, or consequential damages
 // (including, but not limited to, procurement of substitute goods or services;
 // loss of use, data, or profits; or business interruption) however caused
 // and on any theory of liability, whether in contract, strict liability,
 // or tort (including negligence or otherwise) arising in any way out of
 // the use of this software, even if advised of the possibility of such damage.
 //
 //M*/

#ifndef __TRACKERMEDIANFLOW_H__
#define __TRACKERMEDIANFLOW_H__

#include "opencv2/core/core.hpp"

namespace cv
{

class TrackerMedianFlowModel;

/** @brief Median Flow tracker implementation.

Implementation of a paper @cite MedianFlow .

The tracker is suitable for very smooth and predictable movements when object is visible throughout
the whole sequence. It's quite and accurate for this type of problems (in particular, it was shown
by authors to outperform MIL). During the implementation period the code at
<http://www.aonsquared.co.uk/node/5>, the courtesy of the author Arthur Amarra, was used for the
reference purpose.
 */
class EXPORT_API TrackerMedianFlow : public virtual Algorithm {
public:
	struct Params {
		/**
		 * @brief TrackerMedianFlow algorithm parameters constructor
		 */
		Params();
		void read(const FileNode& fn);
		void write(FileStorage& fs) const;

		int mPointsInGrid; /**< Square root of number of keypoints used.
								Increase it to trade accurateness for speed.
								Default value is sensible and recommended */

		Size mWindowSize;  /**< Size of the search window at each pyramid level
								for Lucas-Kanade optical flow search used for
								tracking */

		int mPyrMaxLevel;  /**< Number of pyramid levels for Lucas-Kanade optical
								flow search used for tracking */
	};

	TrackerMedianFlow(Params paramsIn = Params());

	bool copyTo(TrackerMedianFlow& copy) const;

	bool init(const Mat& image, const Rect_<float>& boundingBox);
	bool update(const Mat& image, Rect_<float>& boundingBox);

	bool isInited() const;

	float getLastConfidence() const;
	Rect_<float> getLastBoundingBox() const;

	void read(FileStorage& fn);
	void write(FileStorage& fs) const;

private:
	bool isInit;

	bool medianFlowImpl(Mat oldImage, Mat newImage, Rect_<float>& oldBox);

	Rect_<float> vote(
			const std::vector<Point2f>& oldPoints,
			const std::vector<Point2f>& newPoints,
			const Rect_<float>& oldRect,
			Point2f& mD);

	template<typename T>
	T getMedian(
			std::vector<T>& values, int size = -1);

	void check_FB(
			std::vector<Mat> newPyramid,
			const std::vector<Point2f>& oldPoints,
			const std::vector<Point2f>& newPoints,
			std::vector<bool>& status);

	void check_NCC(
			const Mat& oldImage,
			const Mat& newImage,
			const std::vector<Point2f>& oldPoints,
			const std::vector<Point2f>& newPoints,
			std::vector<bool>& status);

	inline float l2distance(Point2f p1, Point2f p2);

	Params params;               /**< Parameters used during tracking, see
										@ref TrackerMedianFlow::Params */

	TermCriteria termcrit;       /**< Terminating criteria for OpenCV
										Lucas–Kanade optical flow algorithm used
										during tracking */

	Rect_<float> m_boundingBox;  /**< Tracking object bounding box */

	float m_confidence;          /**< Confidence that face was tracked correctly
										at the last tracking iteration */

	Mat m_image;                 /**< Last image for which tracking was
										performed */

	std::vector<Mat> m_pyramid;  /**< The pyramid had been calculated for
										the previous frame or when
										initialize the model */
};

} /* namespace cv */

#endif /* __TRACKERMEDIANFLOW_H__ */
