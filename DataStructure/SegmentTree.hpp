/*	================================================================
	SegmentTree(http://en.wikipedia.org/wiki/Segment_Tree) is a	Tree
	Data Structure for range accumulative operations, such as 
	summation (see SegmentTreeSample for details).

	To define a SegmentTree, we need:
	1) A domain(D)
		Given by the template argument ValueType(int, string, etc.)

	2) A set of elements(S) in that domain
		Given by two stl iterators in the ctor.

	3) A binary operation(op) on the domain
		The operation op should hold:
		a) Closure: \any a, b \in D, a op b \in D.
		b) Associativity: (a op b) op c == a op (b op c)

		This operation is defined by a functor with signature like
		ValueType(ValueType, ValueType), or any equivalent versions.
		Usually defined by a lambda expression.

	4) An identity element(e)
		\any a \in D, a op e == e op a == a

	You may also interested in: http://en.wikipedia.org/wiki/Monoid.

	================================================================
	
	Usage:

		vector<int> values;
		his::SegmentTree<int> segment_tree(begin(values), end(values), 
			0, [](int a, int b)->int { return a + b; });

		int sum_from_start_to_end = segment_tree.query(start, end);

	is equivalent to:

		int sum_from_start_to_end = 0;
		for (size_t i = start; i < end; ++i)
			sum_from_start_to_end += values[i];

	================================================================
	
	Build time: O(N)
	Query time: O(logN) on average, O(N) on worst case.
	Space Complexity: O(N)

	Notes:
	This class uses an array representation for the tree. Given a set
	of size N, a full binary tree will be built with 2^(cell(logN)+1)-1
	nodes, which wastes memories. If you use this on a space consuming 
	user-defined type, that might be a problem.
*/

#ifndef HIS_DATASTRUCTURE_SEGMENTTREE_H
#define HIS_DATASTRUCTURE_SEGMENTTREE_H

#include <cassert>
#include <functional>
#include <vector>

namespace his
{


template<typename ValueType>
class SegmentTree
{
	typedef std::function<ValueType(ValueType, ValueType)> Operation;

public:
	/*
	Inputs:
	const Iter begin, const Iter end:
		Two stl iterators to indicate the set.
		The range is then defined on this set.

	ValueType identity:
		An identity element of ValueType, such that with the following operation
		operation(identity, value) == operation(value, identity) == value
		holds for any value of ValueType.
		For aesthetic reason I put this parameter before operation.

	Operation operation:
		A functor with type: ValueType(ValueType, ValueType) 
		or any equivalent versions, defining the binary operation.
	*/
	template<class Iter>
	SegmentTree(const Iter begin, const Iter end, 
		ValueType identity, Operation operation)
		:m_identity(identity), m_operation(operation)
	{
		// an optimistic check
		assert(m_operation(identity, identity) == identity);

		// count the element number
		int size = 0;
		for (auto it = begin; it != end; ++it)
			size++;

		// expand to full binary tree, number of leaves must be a 2-power
		int leaves = 1;
		while (leaves < size)
			leaves *= 2;

		// totally (leaves * 2 - 1) nodes, with (leaves - 1) non-leaf nodes
		m_tree.resize(leaves * 2 - 1);

		// put leaves to the end
		size_t pos = leaves-1; // beginning of the leaves
		for (auto it = begin; it != end; ++it)
			m_tree[pos++] = *it;

		// fill with identities
		while (pos < m_tree.size())
			m_tree[pos++] = m_identity;

		// initialize internal nodes
		for (int i = leaves-2; i >= 0; --i)
			m_tree[i] = m_operation(m_tree[get_left_child(i)], m_tree[get_right_child(i)]);
	}

	/*
	Inputs:
	size_t start, size_t end:
	The query range [start, end)
	
	Output:
	The accumulation of operation from start to end(exclusive).

	Equivalent to:
	ValueType accumulation = identity;
	for (size_t i = start; i < end; ++i)
		accumulation = operation(accumulation, the_set[i]);
	*/
	ValueType query(size_t start, size_t end)
	{
		return query(start, end, 0, (m_tree.size()+1) / 2, 0);
	}

private:
	// accessing children/parent indices in an array representation
	size_t get_left_child(size_t id) { return id * 2 + 1; }
	size_t get_right_child(size_t id) { return id * 2 + 2; }
	size_t get_parent(size_t id) { return (id - 1) / 2; }

	ValueType query(size_t start, size_t end, size_t node_start, size_t node_end, size_t node_id)
	{
		// node range is inside query
		if (start <= node_start && node_end <= end)
			return m_tree[node_id];

		// node range is outside query
		if (end <= node_start || node_end <= start)
			return m_identity;

		// query children with higher resolution
		int mid = (node_start + node_end) / 2;
		return m_operation(query(start, end, node_start, mid, get_left_child(node_id)),
			query(start, end, mid, node_end, get_right_child(node_id)));
	}

	ValueType m_identity;
	std::vector<ValueType> m_tree;
	Operation m_operation;
};


}
#endif // HIS_DATASTRUCTURE_SEGMENTTREE_H