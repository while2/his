/*	================================================================
	These methods provide a pairwise iteration for images. Each pair 
	of neighboring pixels are iterated once and only once, with order 
	up-down, left-right.

	Inputs:
		Mat1 mat1, Mat2 mat2, ... :
			Images to iterate.
		Func func:
			A functor takes corresponding pixels. Two parameters
			for each neighboring pixels, can by value, const 
			reference or reference if the corresponding image 
			is not a const one.


	A grayscale laplacian sample:
	
	his::for_each_pair(image, laplacian,
		[](uchar b1, uchar b2, float &f1, float &f2) {
		f1 += (b1 - b2);
		f2 += (b2 - b1);
	});

	See http://while2.github.io/abstraction-of-iterations.html for details.
 */

#ifndef HIS_IMAGEPROCESSING_FOREACHPAIR_HPP
#define HIS_IMAGEPROCESSING_FOREACHPAIR_HPP

#include <cassert>

namespace his
{


template<typename Mat1, class Func>
void for_each_pair(Mat1 &mat, Func func)
{
	Mat1::FOR_EACH_ABLE;
	// first column
	auto up = mat[0];
	auto down = mat[1];
	for (size_t y = 1; y < mat.rows(); ++y)
	{
		func(*up, *down);
		up += mat.step(), down += mat.step();
	}

	// first row
	auto left = mat[0];
	auto right = mat[0] + 1;
	for (size_t x = 1; x < mat.cols(); ++x)
	{
		func(*left, *right);
		left += 1, right += 1;
	}

	// the rest
	for (size_t y = 1; y < mat.rows(); ++y)
	{
		auto up = mat[y - 1] + 1;
		auto left = mat[y];
		auto center = mat[y] + 1;
		for (size_t x = 1; x < mat.cols(); ++x)
		{
			func(*up, *center);
			func(*left, *center);
			up += 1, left += 1, center += 1;
		}
	}
}

template<class Mat1, class Mat2, class Func>
void for_each_pair(Mat1 &mat1, Mat2 &mat2, Func func)
{
	Mat1::FOR_EACH_ABLE;
	Mat2::FOR_EACH_ABLE;

	assert(mat1.rows() == mat2.rows() && mat1.cols() == mat2.cols());
	// first column
	auto up1 = mat1[0];
	auto down1 = mat1[1];
	auto up2 = mat2[0];
	auto down2 = mat2[1];
	for (size_t y = 1; y < mat1.rows(); ++y)
	{
		func(*up1, *down1, *up2, *down2);
		up1 += mat1.step(), down1 += mat1.step();
		up2 += mat2.step(), down2 += mat2.step();
	}

	// first row
	auto left1 = mat1[0];
	auto right1 = mat1[0] + 1;
	auto left2 = mat2[0];
	auto right2 = mat2[0] + 1;
	for (size_t x = 1; x < mat1.cols(); ++x)
	{
		func(*left1, *right1, *left2, *right2);
		left1 += 1, right1 += 1;
		left2 += 1, right2 += 1;
	}

	// the rest
	for (size_t y = 1; y < mat1.rows(); ++y)
	{
		auto up1 = mat1[y - 1] + 1;
		auto left1 = mat1[y];
		auto center1 = mat1[y] + 1;
		auto up2 = mat2[y - 1] + 1;
		auto left2 = mat2[y];
		auto center2 = mat2[y] + 1;
		for (size_t x = 1; x < mat1.cols(); ++x)
		{
			func(*up1, *center1, *up2, *center2);
			func(*left1, *center1, *left2, *center2);
			up1 += 1, left1 += 1, center1 += 1;
			up2 += 1, left2 += 1, center2 += 1;
		}
	}
}

template<class Mat1, class Mat2, class Mat3, class Func>
void for_each_pair(Mat1 &mat1, Mat2 &mat2, Mat3 &mat3, Func func)
{
	Mat1::FOR_EACH_ABLE;
	Mat2::FOR_EACH_ABLE;
	Mat3::FOR_EACH_ABLE;

	assert(mat1.rows() == mat2.rows() && mat1.cols() == mat2.cols());
	assert(mat1.rows() == mat3.rows() && mat1.cols() == mat3.cols());
	// first column
	auto up1 = mat1[0];
	auto down1 = mat1[1];

	auto up2 = mat2[0];
	auto down2 = mat2[1];

	auto up3 = mat3[0];
	auto down3 = mat3[1];

	for (size_t y = 1; y < mat1.rows(); ++y)
	{
		func(*up1, *down1, *up2, *down2, *up3, *down3);
		up1 += mat1.step(), down1 += mat1.step();
		up2 += mat2.step(), down2 += mat2.step();
		up3 += mat3.step(), down3 += mat3.step();
	}

	// first row
	auto left1 = mat1[0];
	auto right1 = mat1[0] + 1;

	auto left2 = mat2[0];
	auto right2 = mat2[0] + 1;

	auto left3 = mat3[0];
	auto right3 = mat3[0] + 1;

	for (size_t x = 1; x < mat1.cols(); ++x)
	{
		func(*left1, *right1, *left2, *right2, *left3, *right3);
		left1 += 1, right1 += 1;
		left2 += 1, right2 += 1;
		left3 += 1, right3 += 1;
	}

	// the rest
	for (size_t y = 1; y < mat1.rows(); ++y)
	{
		auto up1 = mat1[y - 1] + 1;
		auto left1 = mat1[y];
		auto center1 = mat1[y] + 1;

		auto up2 = mat2[y - 1] + 1;
		auto left2 = mat2[y];
		auto center2 = mat2[y] + 1;

		auto up3 = mat3[y - 1] + 1;
		auto left3 = mat3[y];
		auto center3 = mat3[y] + 1;

		for (size_t x = 1; x < mat1.cols(); ++x)
		{
			func(*up1, *center1, *up2, *center2, *up3, *center3);
			func(*left1, *center1, *left2, *center2, *left3, *center3);
			up1 += 1, left1 += 1, center1 += 1;
			up2 += 1, left2 += 1, center2 += 1;
			up3 += 1, left3 += 1, center3 += 1;
		}
	}
}

template<class Mat1, class Mat2, class Mat3, class Mat4, class Func>
void for_each_pair(Mat1 &mat1, Mat2 &mat2, Mat3 &mat3, Mat4 &mat4, Func func)
{
	Mat1::FOR_EACH_ABLE;
	Mat2::FOR_EACH_ABLE;
	Mat3::FOR_EACH_ABLE;
	Mat4::FOR_EACH_ABLE;

	assert(mat1.rows() == mat2.rows() && mat1.cols() == mat2.cols());
	assert(mat1.rows() == mat3.rows() && mat1.cols() == mat3.cols());
	assert(mat1.rows() == mat4.rows() && mat1.cols() == mat4.cols());

	// first column
	auto up1 = mat1[0];
	auto down1 = mat1[1];

	auto up2 = mat2[0];
	auto down2 = mat2[1];

	auto up3 = mat3[0];
	auto down3 = mat3[1];

	auto up4 = mat4[0];
	auto down4 = mat4[1];

	for (size_t y = 1; y < mat1.rows(); ++y)
	{
		func(*up1, *down1, *up2, *down2, *up3, *down3, *up4, *down4);
		up1 += mat1.step(), down1 += mat1.step();
		up2 += mat2.step(), down2 += mat2.step();
		up3 += mat3.step(), down3 += mat3.step();
		up4 += mat4.step(), down4 += mat4.step();
	}

	// first row
	auto left1 = mat1[0];
	auto right1 = mat1[0] + 1;

	auto left2 = mat2[0];
	auto right2 = mat2[0] + 1;

	auto left3 = mat3[0];
	auto right3 = mat3[0] + 1;

	auto left4 = mat4[0];
	auto right4 = mat4[0] + 1;

	for (size_t x = 1; x < mat1.cols(); ++x)
	{
		func(*left1, *right1, *left2, *right2, *left3, *right3, *left4, *right4);
		left1 += 1, right1 += 1;
		left2 += 1, right2 += 1;
		left3 += 1, right3 += 1;
		left4 += 1, right4 += 1;
	}

	// the rest
	for (size_t y = 1; y < mat1.rows(); ++y)
	{
		auto up1 = mat1[y - 1] + 1;
		auto left1 = mat1[y];
		auto center1 = mat1[y] + 1;

		auto up2 = mat2[y - 1] + 1;
		auto left2 = mat2[y];
		auto center2 = mat2[y] + 1;

		auto up3 = mat3[y - 1] + 1;
		auto left3 = mat3[y];
		auto center3 = mat3[y] + 1;

		auto up4 = mat4[y - 1] + 1;
		auto left4 = mat4[y];
		auto center4 = mat4[y] + 1;	

		for (size_t x = 1; x < mat1.cols(); ++x)
		{
			func(*up1, *center1, *up2, *center2, *up3, *center3, *up4, *center4);
			func(*left1, *center1, *left2, *center2, *left3, *center3, *left4, *center4);
			up1 += 1, left1 += 1, center1 += 1;
			up2 += 1, left2 += 1, center2 += 1;
			up3 += 1, left3 += 1, center3 += 1;
			up4 += 1, left4 += 1, center4 += 1;
		}
	}
}


}
#endif // HIS_IMAGEPROCESSING_FOREACHPAIR_HPP
