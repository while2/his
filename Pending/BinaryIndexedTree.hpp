/*
 *	A simple Binary Indexed Tree
 *	Note that key must be positive *	
 */

#pragma once
#include <vector>
#include <cassert>


namespace his
{


template<typename Key, typename Value>
class BinaryIndexedTree
{
public:
	BinaryIndexedTree(int size) : m_tree(size, 0)
	{}

	void Add(Key key, Value val) 
	{
		assert(key > 0);
		while (key < m_tree.size()) 
		{
			m_tree[key] += val;
			key += last_bit(key);
		}
	}

	Value Accumulate(Key key) 
	{
		Value acc = 0;
		while (key > 0) 
		{
			acc += m_tree[key];
			key -= last_bit(key);
		}
		return acc;
	}

	Value Get(Key key) 
	{
		assert(key > 0);
		return Accumulate(key) - Accumulate(key-1);
	}

	void Set(Key key, Value val) 
	{
		assert(key > 0);
		Add(key, val - Get(key));
	}


private:
	Key last_bit(Key key) 
	{
		return key & (-key);
	}
	std::vector<Value> m_tree;
};


}