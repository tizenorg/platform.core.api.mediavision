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
#include <stdio.h>
#include "HoGDetector.h"
#include "opencv2/imgproc/imgproc.hpp"
#include <iterator>

#ifdef ENABLE_NEON
#include <arm_neon.h>
#endif

#ifdef ENABLE_OMP
#include <sched.h>
#define NCORES 4
static int coreids[NCORES] = {1, 2, 3, 4};
#endif

/****************************************************************************************\
      The code below is implementation of HOG (Histogram-of-Oriented Gradients)
      descriptor and object detection, introduced by Navneet Dalal and Bill Triggs.

      The computed feature vectors are compatible with the
      INRIA Object Detection and Localization Toolkit
      (http://pascal.inrialpes.fr/soft/olt/)
\****************************************************************************************/

namespace modifiedcv {

class ParallelLoopBodyWrapper {
public:
	ParallelLoopBodyWrapper(const cv::ParallelLoopBody& _body, const cv::Range& _r) {
		body = &_body;
		wholeRange = _r;
		nstripes = cvRound(wholeRange.end - wholeRange.start);
	}
	void operator()(const cv::Range& sr) const {
		cv::Range r;
		r.start = (int)(wholeRange.start +
						((uint64)sr.start*(wholeRange.end - wholeRange.start) + nstripes/2)/nstripes);
		r.end = sr.end >= nstripes ? wholeRange.end : (int)(wholeRange.start +
				((uint64)sr.end*(wholeRange.end - wholeRange.start) + nstripes/2)/nstripes);
		(*body)(r);
	}
	cv::Range stripeRange() const {
		return cv::Range(0, nstripes);
	}

protected:
	const cv::ParallelLoopBody* body;
	cv::Range wholeRange;
	int nstripes;
};

void parallel_for_(const cv::Range& range, const cv::ParallelLoopBody& body)
{
#if defined ENABLE_OMP
	ParallelLoopBodyWrapper pbody(body, range);
	cv::Range stripeRange = pbody.stripeRange();
	int i = 0;
	#pragma omp parallel for private(i) num_threads(NCORES)
	for (i = stripeRange.start; i < stripeRange.end; ++i) {
		cpu_set_t mask;
		CPU_ZERO(&mask);
		CPU_SET(coreids[i % 4], &mask);

		if (sched_setaffinity (0, sizeof(mask), &mask) == -1) {
			printf("Could not set CPU Affinity, continuing...");
		}

		pbody(Range(i, i + 1));
	}
#else
	cv::parallel_for_(range, body);
#endif
}

size_t HOGDescriptor::getDescriptorSize() const
{
	return (size_t)nbins*
		(blockSize.width/cellSize.width)*
		(blockSize.height/cellSize.height)*
		((winSize.width - blockSize.width)/blockStride.width + 1)*
		((winSize.height - blockSize.height)/blockStride.height + 1);
}

double HOGDescriptor::getWinSigma() const
{
	return winSigma >= 0 ? winSigma : (blockSize.width + blockSize.height)/8.;
}

bool HOGDescriptor::checkDetectorSize() const
{
	size_t detectorSize = svmDetector.size(), descriptorSize = getDescriptorSize();
	return detectorSize == 0 ||
		detectorSize == descriptorSize ||
		detectorSize == descriptorSize + 1;
}

void HOGDescriptor::setSVMDetector(InputArray _svmDetector)
{
	_svmDetector.getMat().convertTo(svmDetector, CV_32F);
	CV_Assert(checkDetectorSize());
}

void HOGDescriptor::computeGradient(const Mat& img, Mat& grad, Mat& qangle,
									Size paddingTL, Size paddingBR) const
{
	CV_Assert(img.type() == CV_8U);

	Size gradsize(img.cols + paddingTL.width + paddingBR.width,
			img.rows + paddingTL.height + paddingBR.height);
	grad.create(gradsize, CV_32FC2);  /* <magnitude*(1-alpha), magnitude*alpha> */
	qangle.create(gradsize, CV_8UC2); /* [0..nbins-1] - quantized gradient orientation */
	Size wholeSize;
	Point roiofs;
	img.locateROI(wholeSize, roiofs);

	int i, x, y;
	/*    int cn = img.channels(); */

	Mat_<float> _lut(1, 256);
	const float* lut = &_lut(0, 0);

	if ( gammaCorrection )
		for ( i = 0; i < 256; i++ )
			_lut(0, i) = std::sqrt((float)i);
	else
		for ( i = 0; i < 256; i++ )
			_lut(0, i) = (float)i;

	AutoBuffer<int> mapbuf(gradsize.width + gradsize.height + 4);
	int* xmap = (int*)mapbuf + 1;
	int* ymap = xmap + gradsize.width + 2;

	const int borderType = (int)cv::BORDER_REFLECT_101;

	for ( x = -1; x < gradsize.width + 1; x++ )
		xmap[x] = cv::borderInterpolate(x - paddingTL.width + roiofs.x,
				wholeSize.width, borderType) - roiofs.x;
	for ( y = -1; y < gradsize.height + 1; y++ )
		ymap[y] = cv::borderInterpolate(y - paddingTL.height + roiofs.y,
				wholeSize.height, borderType) - roiofs.y;

	/* x- & y- derivatives for the whole row */
	int width = gradsize.width;
	AutoBuffer<float> _dbuf(width*4);
	float* dbuf = _dbuf;
	Mat Dx(1, width, CV_32F, dbuf);
	Mat Dy(1, width, CV_32F, dbuf + width);
	Mat Mag(1, width, CV_32F, dbuf + width*2);
	Mat Angle(1, width, CV_32F, dbuf + width*3);

	int _nbins = nbins;
	float angleScale = (float)(_nbins/CV_PI);

	for ( y = 0; y < gradsize.height; y++ ) {
		const uchar* imgPtr  = img.data + img.step*ymap[y];
		const uchar* prevPtr = img.data + img.step*ymap[y-1];
		const uchar* nextPtr = img.data + img.step*ymap[y+1];
		float* gradPtr = (float*)grad.ptr(y);
		uchar* qanglePtr = (uchar*)qangle.ptr(y);

		for (x = 0; x < width; x++) {
			int x1 = xmap[x];
			dbuf[x] = (float)(lut[imgPtr[xmap[x+1]]] - lut[imgPtr[xmap[x-1]]]);
			dbuf[width + x] = (float)(lut[nextPtr[x1]] - lut[prevPtr[x1]]);
		}

		cartToPolar(Dx, Dy, Mag, Angle, false);

		for (x = 0; x < width; x++) {
			float mag = dbuf[x+width*2], angle = dbuf[x+width*3]*angleScale - 0.5f;
			int hidx = cvFloor(angle);
			angle -= hidx;
			gradPtr[x*2] = mag*(1.f - angle);
			gradPtr[x*2+1] = mag*angle;

			if ( hidx < 0 )
				hidx += _nbins;
			else if ( hidx >= _nbins )
				hidx -= _nbins;
			assert((unsigned)hidx < (unsigned)_nbins);

			qanglePtr[x*2] = (uchar)hidx;
			hidx++;
			hidx &= hidx < _nbins ? -1 : 0;
			qanglePtr[x*2+1] = (uchar)hidx;
		}
	}
}


struct HOGCache {
	struct BlockData {
		BlockData() : histOfs(0), imgOffset() {}
		int histOfs;
		Point imgOffset;
	};

	struct PixData {
		size_t gradOfs, qangleOfs;
		int histOfs[4];
		float histWeights[4];
		float gradWeight;
	};

	HOGCache();
	HOGCache(const HOGDescriptor* descriptor,
			const Mat& img, Size paddingTL, Size paddingBR,
			bool useCache, Size cacheStride);
	virtual ~HOGCache() {};
	virtual void init(const HOGDescriptor* descriptor,
			const Mat& img, Size paddingTL, Size paddingBR,
			bool useCache, Size cacheStride);

	Size windowsInImage(Size imageSize, Size winStride) const;
	Rect getWindow(Size imageSize, Size winStride, int idx) const;

	const float* getBlock(Point pt, float* buf);
	virtual void normalizeBlockHistogram(float* histogram) const;

	vector<PixData> pixData;
	vector<BlockData> blockData;

	bool useCache;
	vector<int> ymaxCached;
	Size winSize, cacheStride;
	Size nblocks, ncells;
	int blockHistogramSize;
	int count1, count2, count4;
	Point imgoffset;
	Mat_<float> blockCache;
	Mat_<uchar> blockCacheFlags;

	Mat grad, qangle;
	const HOGDescriptor* descriptor;
};


HOGCache::HOGCache()
{
	useCache = false;
	blockHistogramSize = count1 = count2 = count4 = 0;
	descriptor = 0;
}

HOGCache::HOGCache(const HOGDescriptor* _descriptor,
		const Mat& _img, Size _paddingTL, Size _paddingBR,
		bool _useCache, Size _cacheStride)
{
	init(_descriptor, _img, _paddingTL, _paddingBR, _useCache, _cacheStride);
}

void HOGCache::init(const HOGDescriptor* _descriptor,
		const Mat& _img, Size _paddingTL, Size _paddingBR,
		bool _useCache, Size _cacheStride)
{
	descriptor = _descriptor;
	cacheStride = _cacheStride;
	useCache = _useCache;

	descriptor->computeGradient(_img, grad, qangle, _paddingTL, _paddingBR);
	imgoffset = _paddingTL;

	winSize = descriptor->winSize;
	Size blockSize = descriptor->blockSize;
	Size blockStride = descriptor->blockStride;
	Size cellSize = descriptor->cellSize;
	int i, j, nbins = descriptor->nbins;
	int rawBlockSize = blockSize.width*blockSize.height;

	nblocks = Size((winSize.width - blockSize.width)/blockStride.width + 1,
			(winSize.height - blockSize.height)/blockStride.height + 1);
	ncells = Size(blockSize.width/cellSize.width, blockSize.height/cellSize.height);
	blockHistogramSize = ncells.width*ncells.height*nbins;

	if ( useCache ) {
		Size cacheSize((grad.cols - blockSize.width)/cacheStride.width+1,
				(winSize.height/cacheStride.height)+1);
		blockCache.create(cacheSize.height, cacheSize.width*blockHistogramSize);
		blockCacheFlags.create(cacheSize);
		size_t cacheRows = blockCache.rows;
		ymaxCached.resize(cacheRows);
		for (size_t ii = 0; ii < cacheRows; ii++ )
			ymaxCached[ii] = -1;
	}

	Mat_<float> weights(blockSize);
	float sigma = (float)descriptor->getWinSigma();
	float scale = 1.f/(sigma*sigma*2);

	float blockHalfHeight = blockSize.height*0.5f;
	float blockHalfWidth = blockSize.width*0.5f;
	for (i = 0; i < blockSize.height; i++)
		for (j = 0; j < blockSize.width; j++) {
			float di = i - blockHalfHeight;
			float dj = j - blockHalfWidth;
			weights(i, j) = std::exp(-(di*di + dj*dj)*scale);
		}

	blockData.resize(nblocks.width*nblocks.height);
	pixData.resize(rawBlockSize*3);

	/*
	 * Initialize 2 lookup tables, pixData & blockData.
	 * Here is why:
	 *
	 * The detection algorithm runs in 4 nested loops (at each pyramid layer):
	 *  loop over the windows within the input image
	 *    loop over the blocks within each window
	 *      loop over the cells within each block
	 *        loop over the pixels in each cell
	 *
	 * As each of the loops runs over a 2-dimensional array,
	 * we could get 8(!) nested loops in total, which is very-very slow.
	 *
	 * To speed the things up, we do the following:
	 *   1. loop over windows is unrolled in the HOGDescriptor::{compute|detect} methods;
	 *         inside we compute the current search window using getWindow() method.
	 *         Yes, it involves some overhead (function call + couple of divisions),
	 *         but it's tiny in fact.
	 *   2. loop over the blocks is also unrolled. Inside we use pre-computed blockData[j]
	 *         to set up gradient and histogram pointers.
	 *   3. loops over cells and pixels in each cell are merged
	 *       (since there is no overlap between cells, each pixel in the block is processed once)
	 *      and also unrolled. Inside we use PixData[k] to access the gradient values and
	 *      update the histogram
	 */

	count1 = count2 = count4 = 0;
	for ( j = 0; j < blockSize.width; j++ )
		for ( i = 0; i < blockSize.height; i++ ) {
			PixData* data = 0;
			float cellX = (j+0.5f)/cellSize.width - 0.5f;
			float cellY = (i+0.5f)/cellSize.height - 0.5f;
			int icellX0 = cvFloor(cellX);
			int icellY0 = cvFloor(cellY);
			int icellX1 = icellX0 + 1, icellY1 = icellY0 + 1;
			cellX -= icellX0;
			cellY -= icellY0;

			if ( (unsigned)icellX0 < (unsigned)ncells.width &&
					(unsigned)icellX1 < (unsigned)ncells.width ) {
				if ( (unsigned)icellY0 < (unsigned)ncells.height &&
						(unsigned)icellY1 < (unsigned)ncells.height ) {
					data = &pixData[rawBlockSize*2 + (count4++)];
					data->histOfs[0] = (icellX0*ncells.height + icellY0)*nbins;
					data->histWeights[0] = (1.f - cellX)*(1.f - cellY);
					data->histOfs[1] = (icellX1*ncells.height + icellY0)*nbins;
					data->histWeights[1] = cellX*(1.f - cellY);
					data->histOfs[2] = (icellX0*ncells.height + icellY1)*nbins;
					data->histWeights[2] = (1.f - cellX)*cellY;
					data->histOfs[3] = (icellX1*ncells.height + icellY1)*nbins;
					data->histWeights[3] = cellX*cellY;
				} else {
					data = &pixData[rawBlockSize + (count2++)];
					if ( (unsigned)icellY0 < (unsigned)ncells.height ) {
						icellY1 = icellY0;
						cellY = 1.f - cellY;
					}
					data->histOfs[0] = (icellX0*ncells.height + icellY1)*nbins;
					data->histWeights[0] = (1.f - cellX)*cellY;
					data->histOfs[1] = (icellX1*ncells.height + icellY1)*nbins;
					data->histWeights[1] = cellX*cellY;
					data->histOfs[2] = data->histOfs[3] = 0;
					data->histWeights[2] = data->histWeights[3] = 0;
				}
			} else {
				if ( (unsigned)icellX0 < (unsigned)ncells.width ) {
					icellX1 = icellX0;
					cellX = 1.f - cellX;
				}

				if ( (unsigned)icellY0 < (unsigned)ncells.height &&
						(unsigned)icellY1 < (unsigned)ncells.height ) {
					data = &pixData[rawBlockSize + (count2++)];
					data->histOfs[0] = (icellX1*ncells.height + icellY0)*nbins;
					data->histWeights[0] = cellX*(1.f - cellY);
					data->histOfs[1] = (icellX1*ncells.height + icellY1)*nbins;
					data->histWeights[1] = cellX*cellY;
					data->histOfs[2] = data->histOfs[3] = 0;
					data->histWeights[2] = data->histWeights[3] = 0;
				} else {
					data = &pixData[count1++];
					if ( (unsigned)icellY0 < (unsigned)ncells.height ) {
						icellY1 = icellY0;
						cellY = 1.f - cellY;
					}
					data->histOfs[0] = (icellX1*ncells.height + icellY1)*nbins;
					data->histWeights[0] = cellX*cellY;
					data->histOfs[1] = data->histOfs[2] = data->histOfs[3] = 0;
					data->histWeights[1] = data->histWeights[2] = data->histWeights[3] = 0;
				}
			}
			data->gradOfs = (grad.cols*i + j)*2;
			data->qangleOfs = (qangle.cols*i + j)*2;
			data->gradWeight = weights(i, j);
		}

	assert(count1 + count2 + count4 == rawBlockSize);
	/* defragment pixData */
	for ( j = 0; j < count2; j++ )
		pixData[j + count1] = pixData[j + rawBlockSize];
	for ( j = 0; j < count4; j++ )
		pixData[j + count1 + count2] = pixData[j + rawBlockSize*2];
	count2 += count1;
	count4 += count2;

	/* initialize blockData */
	for ( j = 0; j < nblocks.width; j++ )
		for ( i = 0; i < nblocks.height; i++ ) {
			BlockData& data = blockData[j*nblocks.height + i];
			data.histOfs = (j*nblocks.height + i)*blockHistogramSize;
			data.imgOffset = Point(j*blockStride.width, i*blockStride.height);
		}
}


const float* HOGCache::getBlock(Point pt, float* buf)
{
	float* blockHist = buf;
	assert(descriptor != 0);

	Size blockSize = descriptor->blockSize;
	pt += imgoffset;

	CV_Assert( (unsigned)pt.x <= (unsigned)(grad.cols - blockSize.width) &&
			(unsigned)pt.y <= (unsigned)(grad.rows - blockSize.height) );

	if ( useCache ) {
		CV_Assert(pt.x % cacheStride.width == 0 &&
				pt.y % cacheStride.height == 0);
		Point cacheIdx(pt.x/cacheStride.width,
				(pt.y/cacheStride.height) % blockCache.rows);
		if ( pt.y != ymaxCached[cacheIdx.y] ) {
			Mat_<uchar> cacheRow = blockCacheFlags.row(cacheIdx.y);
			cacheRow = (uchar)0;
			ymaxCached[cacheIdx.y] = pt.y;
		}

		blockHist = &blockCache[cacheIdx.y][cacheIdx.x*blockHistogramSize];
		uchar& computedFlag = blockCacheFlags(cacheIdx.y, cacheIdx.x);
		if ( computedFlag != 0 )
			return blockHist;
		computedFlag = (uchar)1; /* set it at once, before actual computing */
	}

	int k, C1 = count1, C2 = count2, C4 = count4;
	const float* gradPtr = (const float*)(grad.data + grad.step*pt.y) + pt.x*2;
	const uchar* qanglePtr = qangle.data + qangle.step*pt.y + pt.x*2;

	CV_Assert(blockHist != 0);
	for ( k = 0; k < blockHistogramSize; k++ )
		blockHist[k] = 0.f;

	const PixData* _pixData = &pixData[0];

	for ( k = 0; k < C1; k++ ) {
		const PixData& pk = _pixData[k];
		const float* a = gradPtr + pk.gradOfs;
		float w = pk.gradWeight*pk.histWeights[0];
		const uchar* h = qanglePtr + pk.qangleOfs;
		int h0 = h[0], h1 = h[1];
		float* hist = blockHist + pk.histOfs[0];
		float t0 = hist[h0] + a[0]*w;
		float t1 = hist[h1] + a[1]*w;
		hist[h0] = t0;
		hist[h1] = t1;
	}

	for ( ; k < C2; k++ ) {
		const PixData& pk = _pixData[k];
		const float* a = gradPtr + pk.gradOfs;
		float w, t0, t1, a0 = a[0], a1 = a[1];
		const uchar* h = qanglePtr + pk.qangleOfs;
		int h0 = h[0], h1 = h[1];

		float* hist = blockHist + pk.histOfs[0];
		w = pk.gradWeight*pk.histWeights[0];
		t0 = hist[h0] + a0*w;
		t1 = hist[h1] + a1*w;
		hist[h0] = t0;
		hist[h1] = t1;

		hist = blockHist + pk.histOfs[1];
		w = pk.gradWeight*pk.histWeights[1];
		t0 = hist[h0] + a0*w;
		t1 = hist[h1] + a1*w;
		hist[h0] = t0;
		hist[h1] = t1;
	}

	for ( ; k < C4; k++ ) {
		const PixData& pk = _pixData[k];
		const float* a = gradPtr + pk.gradOfs;
		float w, t0, t1, a0 = a[0], a1 = a[1];
		const uchar* h = qanglePtr + pk.qangleOfs;
		int h0 = h[0], h1 = h[1];

		float* hist = blockHist + pk.histOfs[0];
		w = pk.gradWeight*pk.histWeights[0];
		t0 = hist[h0] + a0*w;
		t1 = hist[h1] + a1*w;
		hist[h0] = t0;
		hist[h1] = t1;

		hist = blockHist + pk.histOfs[1];
		w = pk.gradWeight*pk.histWeights[1];
		t0 = hist[h0] + a0*w;
		t1 = hist[h1] + a1*w;
		hist[h0] = t0;
		hist[h1] = t1;

		hist = blockHist + pk.histOfs[2];
		w = pk.gradWeight*pk.histWeights[2];
		t0 = hist[h0] + a0*w;
		t1 = hist[h1] + a1*w;
		hist[h0] = t0;
		hist[h1] = t1;

		hist = blockHist + pk.histOfs[3];
		w = pk.gradWeight*pk.histWeights[3];
		t0 = hist[h0] + a0*w;
		t1 = hist[h1] + a1*w;
		hist[h0] = t0;
		hist[h1] = t1;
	}

	normalizeBlockHistogram(blockHist);

	return blockHist;
}

void HOGCache::normalizeBlockHistogram(float* _hist) const
{
#ifdef ENABLE_NEON
	/* NEON vector for loading the histogram to the memory */
	float32x4_t hist_v;
	/* Initialize the accumulator for summation storing */
	float32x4_t acc = vdupq_n_f32(0.f);
#endif

	/* Histogram pointer in the memory */
	float *hist_ptr = &_hist[0];
	/* Variable to store values of summations */
	float sum = 0.f;
	size_t sz = blockHistogramSize;

#ifdef ENABLE_NEON
	for (; sz != 0u; sz -= 4u) {
		hist_v = vld1q_f32(hist_ptr);
		acc = vmlaq_f32(acc, hist_v, hist_v);
		hist_ptr += 4;
	}

	sum += vgetq_lane_f32(acc, 0) + vgetq_lane_f32(acc, 1) +
		vgetq_lane_f32(acc, 2) + vgetq_lane_f32(acc, 3);

	/* Reset accumulator */
	acc = vdupq_n_f32(0.f);

	sz = blockHistogramSize;
	hist_ptr = &_hist[0];
#else
	for (size_t i = 0; i < sz; ++i)
		sum += hist_ptr[i] * hist_ptr[i];
#endif

	float scale = 1.f / (std::sqrt(sum) + sz * 0.1f);
	sum = 0.f;

#ifdef ENABLE_NEON
	float32x4_t thres_v = vdupq_n_f32((float)descriptor->L2HysThreshold);

	for (; sz != 0; sz -= 4) {
		/* Find minimal value among threshold and histogram value, accumulate
		 * this value squared */
		hist_v = vminq_f32(vmulq_n_f32(vld1q_f32(hist_ptr), scale), thres_v);
		acc = vmlaq_f32(acc, hist_v, hist_v);
		/* Update histograms in memory according with found min values */
		vst1q_f32(hist_ptr, hist_v);
		hist_ptr += 4;
	}

	sum += vgetq_lane_f32(acc, 0) + vgetq_lane_f32(acc, 1) +
		vgetq_lane_f32(acc, 2) + vgetq_lane_f32(acc, 3);

#else
	float thresh = (float)descriptor->L2HysThreshold;
	for (size_t i = 0; i < sz; ++i) {
		hist_ptr[i] = std::min(hist_ptr[i] * scale, thresh);
		sum += hist_ptr[i] * hist_ptr[i];
	}
#endif

	scale = 1.f / (std::sqrt(sum) + 1e-3f);

#ifdef ENABLE_NEON
	sz = blockHistogramSize;
	hist_ptr = &_hist[0];

	/* Scale histogram (normalize): */
	for (; sz != 0; sz -= 4) {
		vst1q_f32(hist_ptr, vmulq_n_f32(vld1q_f32(hist_ptr), scale));
		hist_ptr += 4;
	}
#else
	for (size_t i = 0; i < sz; i++ )
		hist_ptr[i] *= scale;
#endif
}


Size HOGCache::windowsInImage(Size imageSize, Size winStride) const
{
	return Size((imageSize.width - winSize.width)/winStride.width + 1,
				(imageSize.height - winSize.height)/winStride.height + 1);
}

Rect HOGCache::getWindow(Size imageSize, Size winStride, int idx) const
{
	int nwindowsX = (imageSize.width - winSize.width)/winStride.width + 1;
	int y = idx / nwindowsX;
	int x = idx - nwindowsX*y;
	return Rect( x*winStride.width, y*winStride.height, winSize.width, winSize.height );
}


void HOGDescriptor::compute(const Mat& img, vector<float>& descriptors,
							Size winStride, Size padding,
							const vector<Point>& locations) const
{
	if ( winStride == Size() )
		winStride = cellSize;
	Size cacheStride(gcd(winStride.width, blockStride.width),
			gcd(winStride.height, blockStride.height));
	size_t nwindows = locations.size();
	padding.width = (int)alignSize(std::max(padding.width, 0), cacheStride.width);
	padding.height = (int)alignSize(std::max(padding.height, 0), cacheStride.height);
	Size paddedImgSize(img.cols + padding.width*2, img.rows + padding.height*2);

	HOGCache cache(this, img, padding, padding, nwindows == 0, cacheStride);

	if ( !nwindows )
		nwindows = cache.windowsInImage(paddedImgSize, winStride).area();

	const HOGCache::BlockData* blockData = &cache.blockData[0];

	int nblocks = cache.nblocks.area();
	int blockHistogramSize = cache.blockHistogramSize;
	size_t dsize = getDescriptorSize();
	descriptors.resize(dsize*nwindows);

	for ( size_t i = 0; i < nwindows; i++ ) {
		float* descriptor = &descriptors[i*dsize];

		Point pt0;
		if ( !locations.empty() ) {
			pt0 = locations[i];
			if ( pt0.x < -padding.width || pt0.x > img.cols + padding.width - winSize.width ||
					pt0.y < -padding.height || pt0.y > img.rows + padding.height - winSize.height )
				continue;
		} else {
			pt0 = cache.getWindow(paddedImgSize, winStride, (int)i).tl() - Point(padding);
			CV_Assert(pt0.x % cacheStride.width == 0 && pt0.y % cacheStride.height == 0);
		}

		for ( int j = 0; j < nblocks; j++ ) {
			const HOGCache::BlockData& bj = blockData[j];
			Point pt = pt0 + bj.imgOffset;

			float* dst = descriptor + bj.histOfs;
			const float* src = cache.getBlock(pt, dst);
			if ( src != dst )
				for ( int k = 0; k < blockHistogramSize; k++ )
					dst[k] = src[k];
		}
	}
}


void HOGDescriptor::detect(const Mat& img,
		vector<Point>& hits, vector<double>& weights, double hitThreshold,
		Size winStride, Size padding, const vector<Point>& locations) const
{
	hits.clear();
	if ( svmDetector.empty() )
		return;

	if ( winStride == Size() )
		winStride = cellSize;
	Size cacheStride(gcd(winStride.width, blockStride.width),
			gcd(winStride.height, blockStride.height));
	size_t nwindows = locations.size();
	padding.width = (int)alignSize(std::max(padding.width, 0), cacheStride.width);
	padding.height = (int)alignSize(std::max(padding.height, 0), cacheStride.height);
	Size paddedImgSize(img.cols + padding.width*2, img.rows + padding.height*2);

	HOGCache cache(this, img, padding, padding, nwindows == 0, cacheStride);

	if ( !nwindows )
		nwindows = cache.windowsInImage(paddedImgSize, winStride).area();

	const HOGCache::BlockData* blockData = &cache.blockData[0];

	int nblocks = cache.nblocks.area();
	int blockHistogramSize = cache.blockHistogramSize;
	size_t dsize = getDescriptorSize();

	double rho = svmDetector.size() > dsize ? svmDetector[dsize] : 0;
	vector<float> blockHist(blockHistogramSize);

	for ( size_t i = 0; i < nwindows; i++ ) {
		Point pt0;
		if ( !locations.empty() ) {
			pt0 = locations[i];
			if ( pt0.x < -padding.width || pt0.x > img.cols + padding.width - winSize.width ||
					pt0.y < -padding.height || pt0.y > img.rows + padding.height - winSize.height )
				continue;
		} else {
			pt0 = cache.getWindow(paddedImgSize, winStride, (int)i).tl() - Point(padding);
			CV_Assert(pt0.x % cacheStride.width == 0 && pt0.y % cacheStride.height == 0);
		}
		double s = rho;
		const float* svmVec = &svmDetector[0];
		int j, k;

		for ( j = 0; j < nblocks; j++, svmVec += blockHistogramSize ) {
			const HOGCache::BlockData& bj = blockData[j];
			Point pt = pt0 + bj.imgOffset;

			const float* vec = cache.getBlock(pt, &blockHist[0]);
#ifdef ENABLE_NEON
			float32x4_t vec_v; /* NEON feature vector */
			float32x4_t svm_v; /* NEON SVM feature weights */
			float32x4_t acc = vdupq_n_f32(0.f); /* NEON partial sum */
			for ( k = 0; k <= blockHistogramSize - 4; k += 4 ) {
				vec_v = vld1q_f32(vec + k);
				svm_v = vld1q_f32(svmVec + k);
				acc = vmlaq_f32(acc, vec_v, svm_v);
			}

			s += vgetq_lane_f32(acc, 0) + vgetq_lane_f32(acc, 1) +
				vgetq_lane_f32(acc, 2) + vgetq_lane_f32(acc, 3);

#else
			for ( k = 0; k <= blockHistogramSize - 4; k += 4 )
				s += vec[k]*svmVec[k] + vec[k+1]*svmVec[k+1] +
					vec[k+2]*svmVec[k+2] + vec[k+3]*svmVec[k+3];
#endif
			for ( ; k < blockHistogramSize; k++ )
				s += vec[k]*svmVec[k];
		}

		if ( s >= hitThreshold ) {
			hits.push_back(pt0);
			weights.push_back(s);
		}
	}
}

void HOGDescriptor::detect(const Mat& img, vector<Point>& hits, double hitThreshold,
		Size winStride, Size padding, const vector<Point>& locations) const
{
	vector<double> weightsV;
	detect(img, hits, weightsV, hitThreshold, winStride, padding, locations);
}

class HOGInvoker : public ParallelLoopBody {
	public:
		HOGInvoker(const HOGDescriptor* _hog, const Mat& _img,
				double _hitThreshold, Size _winStride, Size _padding,
				const double* _levelScale, std::vector<Rect> * _vec, Mutex* _mtx,
				std::vector<double>* _weights = 0, std::vector<double>* _scales = 0) {
			hog = _hog;
			img = _img;
			hitThreshold = _hitThreshold;
			winStride = _winStride;
			padding = _padding;
			levelScale = _levelScale;
			vec = _vec;
			weights = _weights;
			scales = _scales;
			mtx = _mtx;
		}

		void operator()(const Range& range) const {
			int i, i1 = range.start, i2 = range.end;
			double minScale = i1 > 0 ? levelScale[i1] : i2 > 1 ? levelScale[i1+1] : std::max(img.cols, img.rows);
			Size maxSz(cvCeil(img.cols/minScale), cvCeil(img.rows/minScale));
			Mat smallerImgBuf(maxSz, img.type());
			vector<Point> locations;
			vector<double> hitsWeights;

			Size wholeSize;
			Point offset;
			img.locateROI(wholeSize, offset);

			for ( i = i1; i < i2; i++ ) {
				double scale = levelScale[i];
				Size sz(cvRound(img.cols/scale), cvRound(img.rows/scale));
				Mat smallerImg(sz, img.type(), smallerImgBuf.data);
				if (sz == img.size())
					smallerImg = Mat(sz, img.type(), img.data, img.step);
				else
					resize(img, smallerImg, sz);
				hog->detect(smallerImg, locations, hitsWeights, hitThreshold, winStride, padding);

				Size scaledWinSize = Size(cvRound(hog->winSize.width*scale), cvRound(hog->winSize.height*scale));

				mtx->lock();
				for ( size_t j = 0; j < locations.size(); j++ ) {
					vec->push_back(Rect(cvRound(locations[j].x*scale),
								cvRound(locations[j].y*scale),
								scaledWinSize.width, scaledWinSize.height));
					if (scales) {
						scales->push_back(scale);
					}
				}
				mtx->unlock();

				if (weights && (!hitsWeights.empty())) {
					mtx->lock();
					for (size_t j = 0; j < locations.size(); j++) {
						weights->push_back(hitsWeights[j]);
					}
					mtx->unlock();
				}
			}
		}

		const HOGDescriptor* hog;
		Mat img;
		double hitThreshold;
		Size winStride;
		Size padding;
		const double* levelScale;
		std::vector<Rect>* vec;
		std::vector<double>* weights;
		std::vector<double>* scales;
		Mutex* mtx;
};


void HOGDescriptor::detectMultiScale(
		const Mat& img, vector<Rect>& foundLocations, vector<double>& foundWeights,
		double hitThreshold, Size winStride, Size padding,
		double scale0, double finalThreshold, bool useMeanshiftGrouping) const
{
	double scale = 1.;
	int levels = 0;

	vector<double> levelScale;
	for ( levels = 0; levels < nlevels; levels++ ) {
		levelScale.push_back(scale);
		if ( cvRound(img.cols/scale) < winSize.width ||
				cvRound(img.rows/scale) < winSize.height ||
				scale0 <= 1 )
			break;
		scale *= scale0;
	}
	levels = std::max(levels, 1);
	levelScale.resize(levels);

	std::vector<Rect> allCandidates;
	std::vector<double> tempScales;
	std::vector<double> tempWeights;
	std::vector<double> foundScales;
	Mutex mtx;

	modifiedcv::parallel_for_(Range(0, (int)levelScale.size()),
			HOGInvoker(this, img, hitThreshold, winStride, padding, &levelScale[0], &allCandidates, &mtx, &tempWeights, &tempScales));

	std::copy(tempScales.begin(), tempScales.end(), back_inserter(foundScales));
	foundLocations.clear();
	std::copy(allCandidates.begin(), allCandidates.end(), back_inserter(foundLocations));
	foundWeights.clear();
	std::copy(tempWeights.begin(), tempWeights.end(), back_inserter(foundWeights));

	if ( useMeanshiftGrouping ) {
		groupRectangles_meanshift(foundLocations, foundWeights, foundScales, finalThreshold, winSize);
	} else {
		groupRectangles(foundLocations, foundWeights, (int)finalThreshold, 0.2);
	}
}

void HOGDescriptor::detectMultiScale(const Mat& img, vector<Rect>& foundLocations,
		double hitThreshold, Size winStride, Size padding,
		double scale0, double finalThreshold, bool useMeanshiftGrouping) const
{
	vector<double> foundWeights;
	detectMultiScale(img, foundLocations, foundWeights, hitThreshold, winStride,
			padding, scale0, finalThreshold, useMeanshiftGrouping);
}

void HOGDescriptor::groupRectangles(vector<cv::Rect>& rectList, vector<double>& weights, int groupThreshold, double eps) const
{
	if ( groupThreshold <= 0 || rectList.empty() ) {
		return;
	}

	CV_Assert(rectList.size() == weights.size());

	vector<int> labels;
	int nclasses = partition(rectList, labels, SimilarRects(eps));

	vector<cv::Rect_<double> > rrects(nclasses);
	vector<int> numInClass(nclasses, 0);
	vector<double> foundWeights(nclasses, DBL_MIN);
	int i, j, nlabels = (int)labels.size();

	for ( i = 0; i < nlabels; i++ ) {
		int cls = labels[i];
		rrects[cls].x += rectList[i].x;
		rrects[cls].y += rectList[i].y;
		rrects[cls].width += rectList[i].width;
		rrects[cls].height += rectList[i].height;
		foundWeights[cls] = max(foundWeights[cls], weights[i]);
		numInClass[cls]++;
	}

	for ( i = 0; i < nclasses; i++ ) {
		/* find the average of all ROI in the cluster */
		cv::Rect_<double> r = rrects[i];
		double s = 1.0/numInClass[i];
		rrects[i] = cv::Rect_<double>(cv::saturate_cast<double>(r.x*s),
				cv::saturate_cast<double>(r.y*s),
				cv::saturate_cast<double>(r.width*s),
				cv::saturate_cast<double>(r.height*s));
	}

	rectList.clear();
	weights.clear();

	for ( i = 0; i < nclasses; i++ ) {
		cv::Rect r1 = rrects[i];
		int n1 = numInClass[i];
		double w1 = foundWeights[i];
		if ( n1 <= groupThreshold )
			continue;
		/* filter out small rectangles inside large rectangles */
		for ( j = 0; j < nclasses; j++ ) {
			int n2 = numInClass[j];

			if ( j == i || n2 <= groupThreshold )
				continue;

			cv::Rect r2 = rrects[j];

			int dx = cv::saturate_cast<int>(r2.width * eps);
			int dy = cv::saturate_cast<int>(r2.height * eps);

			if ( r1.x >= r2.x - dx &&
					r1.y >= r2.y - dy &&
					r1.x + r1.width <= r2.x + r2.width + dx &&
					r1.y + r1.height <= r2.y + r2.height + dy &&
					(n2 > std::max(3, n1) || n1 < 3) )
				break;
		}

		if ( j == nclasses ) {
			rectList.push_back(r1);
			weights.push_back(w1);
		}
	}
}
}
