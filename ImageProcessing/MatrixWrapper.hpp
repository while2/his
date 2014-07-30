/*	================================================================
	A simple 2d array wrapper, holding a pointer to the beginning of 
	the data. Member `m_rows` and `m_cols` indicates the size, `m_step`
	is the length to skip from one row to the next at the same column, 
	equals to `m_cols` by default.

	MatrixWrapper use shallow copy for copy constructor and assignment 
	operator. When MatrixWrapper<int> a = b; a[i][j] accesses to the 
	same address as b[i][j].

	Note:
	This class does NOT care about memory management. Make sure the 
	pointer is available during its scope.
*/

#ifndef HIS_IMAGEPROCESSING_MATRIXWRAPPER_HPP
#define HIS_IMAGEPROCESSING_MATRIXWRAPPER_HPP

#include <cassert>
#include <memory>

namespace his
{

template<typename T> class Matrix;
struct Idx;

template<typename T>
class MatrixWrapper
{
public:
	MatrixWrapper()
		:m_start(nullptr)
		,m_rows(0), m_cols(0), m_step(0)
	{}


	/*
		Inputs:
		void *start: 
			The beginning of the image data.			
		size_t rows, size_t cols:
			Size of the image.		
		size_t step:
			Number of elements to jump from one row to the next 
			at the same column.
	*/
	MatrixWrapper(void *start, size_t rows, size_t cols, size_t step)
		:m_start(static_cast<T*>(start))
		,m_rows(rows), m_cols(cols), m_step(step)
	{}


	/*
		Inputs:
		void *start, size_t rows, size_t cols:
			The same as the previous version.
		With step equals to cols.
	*/
	MatrixWrapper(void *start, size_t rows, size_t cols)
		:m_start(static_cast<T*>(start))
		,m_rows(rows), m_cols(cols), m_step(cols)
	{}


	/*
		Initialize all elements with a initial value
		Input:
			const T &t: The initial value
	*/
	void set(const T &init_val)
	{
		for (size_t y = 0; y < rows(); ++y)
			for (size_t x = 0; x < rows(); ++x)
				(*this)[y][x] = init_val;
	}
	

	/*	
		Copy the elements to the destination matrix.
		Input:
			MatrixWrapper &dst: The destination matrix
	*/	
	void copy_to(MatrixWrapper &dst) const
	{
		assert(dst.rows() == rows() && dst.cols() == cols());
		for (size_t y = 0; y < rows(); ++y)
			memcpy(dst[y], (*this)[y], m_cols * sizeof(T));
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
			size_t rows, size_t cols:
				The size of the cropped image.
		Output:
			Return a new MatrixWrapper represents the cropped image. Usually 
			this would be a part of the original ones.
	*/
	MatrixWrapper crop(int top, int left, size_t rows, size_t cols) const
	{
		assert(top + rows <= m_rows && left + cols <= m_cols);
		MatrixWrapper<T> sub(m_start, rows, cols, step());
		sub.m_start = m_start + m_step * top + left;
		return sub;
	}


	/*
		Output:
			A deep copy of the matrix.

		Since MatrixWrapper does NOT hold memories, clone() creates 
		a Matrix instance.
	*/
	Matrix<T> clone() const
	{
		Matrix<T> matrix(m_rows, m_cols);
		copy_to(matrix);
		return matrix;
	}


	/*
		Input:
			int y: a row indicator.
		Output:
			A row pointer, which points to the beginning of the y-th row. 
		
		Usage:
			his::Matrix<int> mat(rows, cols);
			mat[i][j] = x;
		
		In extreme cases, y can be negative, e.g. acccess to an outside pixel of
		a cropped image. If so, make sure you know what you are doing.
	*/
			T *operator [](int y)			{ return m_start + m_step * y; }
	const	T *operator [](int y) const		{ return m_start + m_step * y; }


	/*
		Inputs:
			int y, int x: row and col indicator
		Output:
			An element with the given position.
		
		Usage:
			his::Matrix<int> mat(rows, cols);
			mat(i, j) = x;

		Note the row indicator goes befor the col one.
		x, y can be also negative for the same reason of operator[].
	*/
			T &operator ()(int y, int x)		{ return (*this)[y][x]; }
	const	T &operator ()(int y, int x) const	{ return (*this)[y][x]; }


	/*
		Inputs:
			his::Idx idx: An Idx type indicates the access index.
		Output:
			An element with the given position.
	*/
			T &operator ()(his::Idx idx)		{ return (*this)[idx.y][idx.x]; }
	const	T &operator ()(his::Idx idx) const	{ return (*this)[idx.y][idx.x]; }


	/*
		Other member accessing methods.
	*/
			T *start()			{ return m_start; }
	const	T *start() const	{ return m_start; }

	size_t rows() const { return m_rows; }
	size_t cols() const { return m_cols; }
	size_t step() const { return m_step; }


	// A contract to make type checking in for_each functions
	enum { FOR_EACH_ABLE, };

protected:
	T *m_start;
	size_t m_rows, m_cols, m_step;
};


}
#endif // HIS_IMAGEPROCESSING_MATRIXWRAPPER_HPP