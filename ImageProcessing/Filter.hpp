/*	================================================================
	Function filter provide an abstraction of 2d-image filters.
	
	2d image filter is a local process, where each pixel from 
	the output image depends on a local area of the input pixels.
	To implement a filter, typically there is an outter interation 
	over all output pixels, and an inner iteration over the kernel 
	(and corresponding input pixels) for each of the outter ones.

	Except straight forword arguments as the input/output images
	and a kernel matrix, two functors are the key idea to define
	the filter process.
	
	AccmFunc defines the inner process, it will be called in the
	inner iteration, accumulates over the kernel and corresponding 
	input pixels, generates some intermediate results.
	
	EvalFunc was called in the outter iteration, evaluate the output 
	pixel depend on the intermediate results.

	See http://while2.github.io/abstraction-of-filter.html for details.

	================================================================
	
	Usage:

		float sum_rgb[3] = {0, 0, 0}, sum_w = 0;
		his::filter(his::MatrixWrapper<uchar[3]>(image.data, image.rows, image.cols),
			his::MatrixWrapper<uchar[3]>(blur.data, blur.rows, blur.cols),
			his::gaussian_kernel<float>(11, 11, 10),
			[&](const uchar rgb[3], float w)
		{
			for (int i = 0; i < 3; ++i)
				sum_rgb[i] += rgb[i] * w;
			sum_w += w;
		},
			[&](uchar rgb[3])
		{
			for (int i = 0; i < 3; ++i)
			{
				rgb[i] = uchar(sum_rgb[i] / sum_w);
				sum_rgb[i] = 0;
			}
			sum_w = 0;
		});

	This code applies a gaussian blur.

	================================================================

	Note:
	The output matrix must be the same with input, the accumulation
	will be trimmed at the boundary. Sometimes a shrinked output was
	preferred, in this case, you can always ignore the boundary cases 
	and crop after filtering.
	
	The kernel size must be odd numbers, and less than half of the image.
	The time complexity is O(rows*cols*krows*kcols), where rows and 
	cols are the image size, krows and kcols the kernel size.
*/

#ifndef HIS_IMAGEPROCESSING_FILTER_HPP
#define HIS_IMAGEPROCESSING_FILTER_HPP

#include <algorithm>
#include <cassert>

#include "Foreach.hpp"
#include "Matrix.hpp"

namespace his
{


/*
	This function provide an abstract interface for 2d-filters.

	Inputs:
	const Mat1 input: The input image
	Mat2 output: The output image
	const Mat3 kernel: The kernel matrix
	AccmFunc accm:
		A functor to accumulate over the input pixels and 
		corresponding kernel weights. 
		Usually a lambda expression to store intermediate
		results in the captured variables.
	EvalFunc eval:
		A functor to evaluate output pixels depend on the 
		intermediate results.
		Usually a lambda expression capturing the same variables 
		as `accm`, thus they share the intermediate results.
*/
template<class Mat1, class Mat2, class Mat3, class AccmFunc, class EvalFunc>
void filter(const Mat1 input, Mat2 output, const Mat3 kernel, AccmFunc accm, EvalFunc eval)
{
	Mat1::FOR_EACH_ABLE;
	Mat2::FOR_EACH_ABLE;
	Mat3::FOR_EACH_ABLE;

	assert(kernel.rows() % 2 == 1 && kernel.cols() % 2 == 1);
	assert(kernel.rows()*2+1 < input.rows() && kernel.cols()*2+1 < input.cols());
	assert(input.rows() == output.rows() && input.cols() == output.cols());

	auto parse_with_check = [&](int x, int y)
	{
		// The kernel need to be cropped at the boundary
		int x0 = std::max<int>(x - kernel.cols() / 2, 0);
		int x1 = std::min<int>(x + kernel.cols() / 2 + 1, input.cols());
		int y0 = std::max<int>(y - kernel.rows() / 2, 0);
		int y1 = std::min<int>(y + kernel.rows() / 2 + 1, input.rows());
				
		his::for_each(input.crop(y0, x0, y1 - y0, x1 - x0),
			kernel.crop(y0 - y + kernel.rows()/2, x0 - x + kernel.cols()/2, y1 - y0, x1 - x0),
			accm);

		eval(output(y, x));
	};

	// top
	for (int y = 0; y < kernel.rows()/2; ++y)
		for (int x = 0; x < input.cols(); ++x)
			parse_with_check(x, y);
		
	// bottom
	for (int y = input.rows() - kernel.rows()/2; y < input.rows(); ++y)
		for (int x = 0; x < input.cols(); ++x)
			parse_with_check(x, y);

	// left
	for (int y = kernel.rows()/2; y < input.rows() - kernel.rows()/2; ++y)
		for (int x = 0; x < kernel.rows()/2; ++x)
			parse_with_check(x, y);
		
	// right
	for (int y = kernel.rows()/2; y < input.rows() - kernel.rows()/2; ++y)
		for (int x = input.cols() - kernel.rows()/2; x < input.cols(); ++x)
			parse_with_check(x, y);

	// central part, no boundary checks
	for (int y = kernel.rows()/2; y < input.rows() - kernel.rows()/2; ++y)
	{
		for (int x = kernel.cols()/2; x < input.cols() - kernel.cols()/2; ++x)
		{
			his::for_each(input.crop(y - kernel.rows()/2, x - kernel.cols()/2, kernel.rows(), kernel.cols()),
				kernel, accm);

			eval(output(y, x));
		}
	}
}


/*
	This function creates a gaussian kernel
	Input:
	int rows, int cols:
		The kernel size
	double sigma
		The smooth controling parameter for gaussian function.

	In a gaussian kernel, pixle's value depends on their 
	distances from center, as:
		w = exp(-dist^2/(2*sigma^2))
	where w is the pixle value, dist is the euclidean
	distance from the center pixel, sigma is a parameter.
*/
template<typename T>
his::Matrix<T> gaussian_kernel(int rows, int cols, double sigma)
{
	his::Matrix<T> kernel(rows, cols);
	double factor = 0.5 / (sigma * sigma); // prepared the factor depend on sigma
	for (int dy = 0; dy <= rows/2; ++dy)
	{
		for (int dx = 0; dx <= cols/2; ++dx)
		{
			double w = exp(-factor * (dx*dx+dy*dy));
			kernel(rows/2-dy, rows/2-dx) = T(w);
			kernel(rows/2-dy, rows/2+dx) = T(w);
			kernel(rows/2+dy, rows/2+dx) = T(w);
			kernel(rows/2+dy, rows/2-dx) = T(w);
		}
	}
	return kernel;
}


}
#endif // HIS_IMAGEPROCESSING_FILTER_HPP
