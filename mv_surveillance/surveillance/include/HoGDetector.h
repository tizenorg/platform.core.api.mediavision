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
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009, Willow Garage Inc., all rights reserved.
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

#ifndef __MEDIA_VISION_HOGDETECTOR_H__
#define __MEDIA_VISION_HOGDETECTOR_H__

/**
 * @file  HOGDetector.h
 * @brief This file contains structure of HOG detector.
 */

#include "opencv2/core/core.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#include <vector>

namespace modifiedcv {

using namespace cv;

struct EXPORT_API HOGDescriptor {
	enum { L2Hys = 0 };
	enum { DEFAULT_NLEVELS = 64 };

	// default constructor
	HOGDescriptor() :
		winSize(64, 128),
		blockSize(16, 16),
		blockStride(8, 8),
		cellSize(8, 8),
		nbins(9),
		derivAperture(1),
		winSigma(-1),
		histogramNormType(HOGDescriptor::L2Hys),
		L2HysThreshold(0.2),
		gammaCorrection(true),
		nlevels(HOGDescriptor::DEFAULT_NLEVELS) {}

	// constructor
	HOGDescriptor(
		Size _winSize,
		Size _blockSize,
		Size _blockStride,
		Size _cellSize,
		int _nbins,
		int _derivAperture = 1,
		double _winSigma = -1.,
		int _histogramNormType = L2Hys,
		double _L2HysThreshold = 0.2,
		bool _gammaCorrection = false,
		int _nlevels = DEFAULT_NLEVELS) :
		winSize(_winSize),
		blockSize(_blockSize),
		blockStride(_blockStride),
		cellSize(_cellSize),
		nbins(_nbins),
		derivAperture(_derivAperture),
		winSigma(_winSigma),
		histogramNormType(_histogramNormType),
		L2HysThreshold(_L2HysThreshold),
		gammaCorrection(_gammaCorrection),
		nlevels(_nlevels) {}

	// default destructor
	virtual ~HOGDescriptor() {}

	size_t getDescriptorSize() const;

	bool checkDetectorSize() const;

	double getWinSigma() const;

	virtual void setSVMDetector(InputArray _svmdetector);

	virtual void compute(
		const Mat& img,
		CV_OUT vector<float>& descriptors,
		Size winStride = Size(),
		Size padding = Size(),
		const vector<Point>& locations = vector<Point>()) const;

	//with found weights output
	virtual void detect(
		const Mat& img,
		CV_OUT vector<Point>& foundLocations,
		CV_OUT vector<double>& weights,
		double hitThreshold = 0.,
		Size winStride = Size(),
		Size padding = Size(),
		const vector<Point>& searchLocations = vector<Point>()) const;

	//without found weights output
	virtual void detect(
		const Mat& img,
		CV_OUT vector<Point>& foundLocations,
		double hitThreshold = 0.,
		Size winStride = Size(),
		Size padding = Size(),
		const vector<Point>& searchLocations = vector<Point>()) const;

	//with result weights output
	virtual void detectMultiScale(
		const Mat& img,
		CV_OUT vector<Rect>& foundLocations,
		CV_OUT vector<double>& foundWeights,
		double hitThreshold = 0,
		Size winStride = Size(),
		Size padding = Size(),
		double scale = 1.05,
		double finalThreshold = 2.0,
		bool useMeanshiftGrouping = false) const;

	//without found weights output
	virtual void detectMultiScale(
		const Mat& img,
		CV_OUT vector<Rect>& foundLocations,
		double hitThreshold = 0.,
		Size winStride = Size(),
		Size padding = Size(),
		double scale = 1.05,
		double finalThreshold = 2.0,
		bool useMeanshiftGrouping = false) const;

	virtual void computeGradient(
		const Mat& img,
		CV_OUT Mat& grad,
		CV_OUT Mat& angleOfs,
		Size paddingTL = Size(),
		Size paddingBR = Size()) const;

	void groupRectangles(
		vector<cv::Rect>& rectList,
		vector<double>& weights,
		int groupThreshold,
		double eps) const;

	Size winSize;
	Size blockSize;
	Size blockStride;
	Size cellSize;
	int nbins;
	int derivAperture;
	double winSigma;
	int histogramNormType;
	double L2HysThreshold;
	bool gammaCorrection;
	vector<float> svmDetector;
	int nlevels;
};

} /* modifiedcv */

#endif /* __MEDIA_VISION_HOGDETECTOR_H__ */
