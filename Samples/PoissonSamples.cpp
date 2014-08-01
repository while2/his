/*	================================================================
	This sample shows a Poisson Image Editing application. With 
	for_each iteration utilities, it becomes quite easy to build 
	Poisson equations.

	Algorithm based on SIGGRAPH 2003 Paper 'Poissin Image Editing':
	http://www.cs.jhu.edu/~misha/Fall07/Papers/Perez03.pdf
	
	Dependencies:
	Opencv:	http://opencv.org/
	Eigen:	http://eigen.tuxfamily.org
*/

#include <opencv2/opencv.hpp>
#include <Eigen/Sparse>

#include "his/ImageProcessing.h"

// INSIDE indicates the facial area
enum Mask { OUTSIDE = 0, INSIDE = 255, };

/*
	This function shows a facial transfer application with Poisson
	Image Editing, by solving a Poisson Equation, which claim monalisa's 
	gradients in the facial area, while holding lena's boundary pixels.
*/
void PoissonImageEditing()
{
	cv::Mat3b image1 = cv::imread("monalisa.jpg");
	cv::Mat3b image2 = cv::imread("lena2.jpg");
	cv::Mat1b mask = cv::imread("face.png", cv::IMREAD_GRAYSCALE);
	
	his::Matrix<int> id_map(image1.rows, image2.cols);
	id_map.set(-1);
	int unknowns = 0;
	his::for_each_pair(id_map, his::MatrixWrapper<uchar>(mask.data, mask.rows, mask.cols),
		[&](int &id1, int &id2, uchar m1, uchar m2)
	{
		// Assign an id to each of the pixels of the INSIDE area, and
		// those at the boundary(has at least one neighbor belonging 
		// to the INSIDE area.
		if (m1 == INSIDE || m2 == INSIDE)
		{
			if (id1 == -1)
				id1 = unknowns++;
			if (id2 == -1)
				id2 = unknowns++;
		}
	});

	Eigen::SparseMatrix<float> A(unknowns, unknowns);
	A.reserve(Eigen::VectorXi::Constant(A.rows(), 5));
	Eigen::MatrixXf b(unknowns, 3); b.setZero();

	his::for_each_pair(id_map, his::MatrixWrapper<uchar[3]>(image2.data, image2.rows, image2.cols),
		[&](int id1, int id2, const uchar rgb1[3], const uchar rgb2[3])
	{
		if (id1 >= 0 && id2 >= 0)
		{
			// Claim that neighboring pixels hold the gradients from image2
			// Set the equation in matrix A at id1-th and id2-th row.
			A.coeffRef(id1, id1) += 1;
			A.coeffRef(id1, id2) -= 1;
			A.coeffRef(id2, id2) += 1;
			A.coeffRef(id2, id1) -= 1;

			for (int c = 0; c < 3; ++c)
			{
				b(id1, c) += rgb1[c] - rgb2[c];
				b(id2, c) += rgb2[c] - rgb1[c];
			}
		}
	});

	his::for_each(id_map, his::MatrixWrapper<uchar>(mask.data, mask.rows, mask.cols),
		his::MatrixWrapper<uchar[3]>(image1.data, image1.rows, image1.cols),
		[&](int id, uchar m, const uchar rgb[3])
	{
		if (id >= 0 && m == OUTSIDE) // Boundary pixels
		{
			// Claim that boundary pixels stay the same as image1.
			A.coeffRef(id, id) += 1;
			for (int c = 0; c < 3; ++c)
				b(id, c) += rgb[c];
		}
	});

	// Solve Poisson Equation
	Eigen::SimplicialCholesky<decltype(A)> solver(A);
	b = solver.solve(b);

	his::for_each(id_map, his::MatrixWrapper<uchar[3]>(image1.data, image1.rows, image1.cols),
		[&](int id, uchar rgb[3])
	{
		if (id >= 0)
		{
			// Assign pixels with the solution of the equation.
			for (int c = 0; c < 3; ++c)
				rgb[c] = (uchar)MAX(0, MIN(b(id, c)+0.5, 255));
		}
	});

	cv::imwrite("monalena.jpg", image1);
}

int main()
{
	PoissonImageEditing();
	return 0;
}