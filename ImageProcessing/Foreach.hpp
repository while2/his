/*	================================================================
	These methods are used to iterate images, with the last argument
	Func be a functor to define how the corresponding elements are 
	processed in each iteration.
	
	Inputs:
		Mat1 mat1[, [Mat2 mat2, ... Mat4 mat4]]:
			Images to iterate. At most 4 images are supported now. 

		Func func:
			A functor takes corresponding pixels.

	A rgb to grayscale sample:

		his::foreach(rgb_image, gray_image, 
			[](const uchar rgb[3], uchar &scale)
		{
			scale = uchar(rgb[0] * 0.299
					+ rgb[1] * 0.587
					+ rgb[2] * 0.114);
		});


	To iterate more images, one can use IdxMap:

		his::for_each(his::IdxMap(mat1), [&](his::Idx)
		{
			auto &pixel1 = mat1(idx);
			auto &pixel2 = mat2(idx);
			...
		});

	See http://while2.github.io/abstraction-of-iterations.html for details.
*/

#ifndef HIS_IMAGEPROCESSING_FOREACH_HPP
#define HIS_IMAGEPROCESSING_FOREACH_HPP

#include <cassert>

namespace his
{


template<class Mat1, class Func>
void for_each(Mat1 mat, Func func)
{
	Mat1::FOR_EACH_ABLE;

	for (size_t y = 0; y < mat.rows(); ++y)
	{
		auto p = mat[y];
		for (size_t x = 0; x < mat.cols(); ++x)
		{
			func(*p);
			p += 1;
		}
	}
}


// 2 matrices
template<class Mat1, class Mat2, class Func>
void for_each(Mat1 mat1, Mat2 mat2, Func func)
{
	Mat1::FOR_EACH_ABLE;
	Mat2::FOR_EACH_ABLE;

	assert(mat1.rows() == mat2.rows() && mat1.cols() == mat2.cols());

	for (size_t y = 0; y < mat1.rows(); ++y)
	{
		auto p1 = mat1[y];
		auto p2 = mat2[y];
		for (size_t x = 0; x < mat1.cols(); ++x)
		{
			func(*p1, *p2);
			p1 += 1, p2 += 1;
		}
	}
}


// 3 matrices
template<class Mat1, class Mat2, class Mat3, class Func>
void for_each(Mat1 mat1, Mat2 mat2, Mat3 mat3, Func func)
{
	Mat1::FOR_EACH_ABLE;
	Mat2::FOR_EACH_ABLE;
	Mat3::FOR_EACH_ABLE;

	assert(mat1.rows() == mat2.rows() && mat1.cols() == mat2.cols());
	assert(mat1.rows() == mat3.rows() && mat1.cols() == mat3.cols());
				
	for (size_t y = 0; y < mat1.rows(); ++y)
	{
		auto p1 = mat1[y];
		auto p2 = mat2[y];
		auto p3 = mat3[y];
		for (size_t x = 0; x < mat1.cols(); ++x)
		{
			func(*p1, *p2, *p3);
			p1 += 1, p2 += 1, p3 += 1;
		}
	}
}


// 4 matrices
template<class Mat1, class Mat2, class Mat3, class Mat4, class Func>
void for_each(Mat1 mat1, Mat2 mat2, Mat3 mat3, Mat4 &mat4, Func func)
{
	Mat1::FOR_EACH_ABLE;
	Mat2::FOR_EACH_ABLE;
	Mat3::FOR_EACH_ABLE;
	Mat4::FOR_EACH_ABLE;

	assert(mat1.rows() == mat2.rows() && mat1.cols() == mat2.cols());
	assert(mat1.rows() == mat3.rows() && mat1.cols() == mat3.cols());
	assert(mat1.rows() == mat4.rows() && mat1.cols() == mat4.cols());
		
	for (size_t y = 0; y < mat1.rows(); ++y)
	{
		auto p1 = mat1[y];
		auto p2 = mat2[y];
		auto p3 = mat3[y];
		auto p4 = mat4[y];
		for (size_t x = 0; x < mat1.cols(); ++x)
		{
			func(*p1, *p2, *p3, *p4);
			p1 += 1, p2 += 1, p3 += 1, p4 += 1;
		}
	}
}


}
#endif // HIS_IMAGEPROCESSING_FOREACH_HPP