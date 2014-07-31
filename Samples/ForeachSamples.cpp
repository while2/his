#include <cassert>
#include <cstdio>

#include <opencv2/opencv.hpp>

#include "his/ImageProcessing.h"


/*
	A gray scale convertion sample.
	Wrap the color and grayscale opencv images with MatrixWrapper,
	define the pixel convertion process by a lambda expression.
*/
void GrayscaleConvertionByForeach()
{
	cv::Mat3b color_image = cv::imread("lena.jpg");
	cv::Mat1b gray_image(color_image.size());
	his::for_each(his::MatrixWrapper<uchar[3]>(color_image.data, color_image.rows, color_image.cols),
		his::MatrixWrapper<uchar>(gray_image.data, gray_image.rows, gray_image.cols),
		[](const uchar rgb[3], uchar &scale)
	{
		// Assign the grayscale intensity as a
		// weighted average of rgb values.
		// Note that opencv use BGR order.
		scale = uchar(rgb[2] * 0.299
				+ rgb[1] * 0.587
				+ rgb[0] * 0.114);
	});
	cv::imwrite("lena_gray.jpg", gray_image);
}


/*
	A grayscale laplacian sample.
	Wrap grayscale image(input) and the laplacian(output) with MatrixWrapper,
	iterate all the neighboring pixels and add the difference to the corresponding
	pixel in the laplacian image.
	Note that laplacian can be negative.
*/
void LaplacianByForeachPair()
{
	cv::Mat1b gray_image = cv::imread("lena_gray.jpg", cv::IMREAD_GRAYSCALE);
	cv::Mat1i laplacian(gray_image.size(), 0);
	
	his::for_each_pair(his::MatrixWrapper<uchar>(gray_image.data, gray_image.rows, gray_image.cols),
		his::MatrixWrapper<int>(laplacian.data, laplacian.rows, laplacian.cols),
		[](uchar b1, uchar b2, int &l1, int &l2) 
	{
		// l1 is the laplacian pixel corresponding to b1, initialized
		// to 0. b2 is a neighbor of b1. Adding (b1 - b2) finished the 
		// laplacian for one of its neighbors. Other neighbors will be 
		// handled in other iterations.
		l1 += (b1 - b2);
		l2 += (b2 - b1);
	});

	cv::imwrite("lena_laplacian.jpg", laplacian);
}


/*
	Add a fading effect on the left half of the image.
	Scale down the color intensity by a factor depending on
	the x coordinates of each pixel.
*/
void FadingByIdxMap()
{
	cv::Mat3b image = cv::imread("lena.jpg");
	his::MatrixWrapper<uchar[3]> image_wrapper(image.data, image.rows, image.cols);

	his::for_each(image_wrapper, his::IdxMap(image_wrapper),
		[&](uchar rgb[3], his::Idx idx)
	{
		if (idx.x < image.cols / 2)
		{
			// linearly blend from 0 to 1 in range[0, cols)
			float factor = 2.f * idx.x / image.cols;
			for (int i = 0; i < 3; ++i)
				rgb[i] = uchar(rgb[i] * factor + 0.5f);
		}
	});
	cv::imwrite("lena_fading.jpg", image);
}

int main()
{
	GrayscaleConvertionByForeach();
	LaplacianByForeachPair();
	FadingByIdxMap();
	return 0;
}