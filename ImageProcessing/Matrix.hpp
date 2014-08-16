/*	================================================================
	Matrix provides an abstraction of a 2d array.
	As a subclass, Matrix provides all the accessing interfaces as 
	MatrixWrapper, while this one holds its own memory in a 
	shared_ptr.
	
	Copy constructor and assignment operator triggers a shallow copy.
	Use clone() for a deep copy.

	Memory will be transfered via copy constructor and assignement
	operator, and will be automatically released with the last 
	destruction of the instances.
*/

#ifndef HIS_IMAGEPROCESSING_MATRIX_HPP
#define HIS_IMAGEPROCESSING_MATRIX_HPP

#include <memory>

#include "MatrixWrapper.hpp"

namespace his
{

template<typename T>
class Matrix : public MatrixWrapper<T>
{
public:
	Matrix()
	{}

	Matrix(int rows, int cols)
	{
		create(rows, cols);
	}
	
	void create(int rows, int cols)
	{
		this->m_rows = rows, this->m_cols = this->m_step = cols;
		m_data = std::shared_ptr<T>(new T[rows * cols], 
			[](const T *p) { delete []p;});
		this->m_start = m_data.get();
	}

	/*
		Crop the matrix.
		Inputs:
			int top, int left: 
				Top and Left position of the cropped image,	under the original 
				coordinates.
				These arguments can be negative(if this matrix has already been
				cropped, you can get back to the original one), but make sure 
				you know what's going on.
			int rows, int cols:
				The size of the cropped image.
		Output:
			Return a new MatrixWrapper represents the cropped image. Usually 
			this would be a part of the original ones.
		
		Note:
			The cropped matrix still shares data with the origin one. To create
			a new matrix, call clone() after crop.
	*/
	Matrix crop(int top, int left, int rows, int cols) const
	{
		assert(top + rows <= this->m_rows && left + cols <= this->m_cols);
		Matrix<T> sub = *this;
		sub.m_start = this->m_start + this->m_step * top + left;
		sub.m_rows = rows;
		sub.m_cols = cols;
		return sub;
	}

protected:
	std::shared_ptr<T>	m_data;
};


}
#endif // HIS_IMAGEPROCESSING_MATRIX_HPP
