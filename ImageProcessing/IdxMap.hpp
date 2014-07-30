/*	================================================================
	An adapter for 'for_each' style methods, provides accessing to
	the 2d indices without extra memories.

	Usage:
		his::for_each(his::IdxMap(mat1), [&](his::Idx idx)
		{
			auto &pixel1 = mat1(idx);
			auto &pixel2 = mat2(idx);
			...
		});

		his::for_each_pair(his::IdxMap(mat1), 
			[&](his::Idx idx1, his::Idx idx2)
		{
			auto &pixel11 = mat1(idx1);
			auto &pixel12 = mat1(idx2);
			auto &pixel21 = mat2(idx1);
			auto &pixel22 = mat2(idx2);
			...
		});
*/

#ifndef HIS_IMAGEPROCESSING_IDXMAP_HPP
#define HIS_IMAGEPROCESSING_IDXMAP_HPP

namespace his
{


/*
	A data struct to indicate 2d-indices in matrices.
	x, y is the 2d indices. Other members are for internal usage.
*/
struct Idx
{
	size_t x, y;
	Idx(size_t _x, size_t _y) : x(_x), y(_y) {}

	// move pointer in the same row
	void operator+=(size_t _x) { x += _x; }

	// move pointer to the next row
	enum Step {};
	void operator+=(Step step) { ++y; }

	Idx operator+(size_t _x) { return Idx(x+_x, y); }
	const Idx &operator*() { return *this; }
};


/*
	A Matrix like data type, with each element a Idx indicating the 
	x, y coordinates of itself.
	Only used to access indices in for_each or for_each_pair procedures.
	Pixel accessing is unsupported:
		IdxMap idx_map(rows, cols);
		idx_map(i, j);	// Error!
		idx_map[i][j];	// Error!
	This does not make sense, because you already know the position(i, j) 
	of the pixel.
*/
template<typename T> class MatrixWrapper;
class IdxMap
{
public:
	IdxMap(size_t rows, size_t cols) 
		: m_rows(rows), m_cols(cols)
	{}

	template<typename T>
	IdxMap(const MatrixWrapper<T> mat)
		: m_rows(mat.rows()), m_cols(mat.cols())
	{}

	Idx operator[](int y)	{	return Idx(0, y);	}

	// size
	size_t rows() const { return m_rows; }
	size_t cols() const { return m_cols; }
	
	size_t m_rows, m_cols;

	// for_each methods add step() to the pointer to jump to next row,
	// IdxMap return a different type to distinguish the change of row
	// from the change of col, thus Idx can react properly(++y instead
	// ++x).
	Idx::Step step() const { return Idx::Step(0); };
	enum {FOR_EACH_ABLE};
};


}
#endif // HIS_IMAGEPROCESSING_IDXMAP_HPP