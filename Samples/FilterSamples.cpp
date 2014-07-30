#include <opencv2/opencv.hpp>

#include "ImageProcessing/MatrixWrapper.hpp"
#include "ImageProcessing/Filter.hpp"

/*
	A Gaussian Blur sample with function filter.
*/
void GaussianBlurByFilter()
{
	cv::Mat3b image = cv::imread("lena.jpg");
	cv::Mat3b blur(image.size());

	float sum_rgb[3] = {0, 0, 0}, sum_w = 0;
	his::filter(his::MatrixWrapper<uchar[3]>(image.data, image.rows, image.cols),
		his::MatrixWrapper<uchar[3]>(blur.data, blur.rows, blur.cols),
		his::gaussian_kernel<float>(11, 11, 10),
		[&](const uchar rgb[3], float w)
	{
		// Accumulate Functor iterates over the kernel,
		// compute weighted sum of corresponding pixels.
		for (int i = 0; i < 3; ++i)
			sum_rgb[i] += rgb[i] * w;
		sum_w += w;
	},
		[&](uchar rgb[3])
	{
		// Shared sum_rgb and sum_w with the previous 
		// functor, evaluate the output pixel with the 
		// weighted average. And reset the intermediate
		// results.
		for (int i = 0; i < 3; ++i)
		{
			rgb[i] = uchar(sum_rgb[i] / sum_w + 0.5f);
			sum_rgb[i] = 0;
		}
		sum_w = 0;
	});
	cv::imwrite("lena_blur.jpg", blur);
}


int main()
{
	GaussianBlurByFilter();
	return 0;
}