#pragma once
#include <vector>
#include <unordered_map>

class UFSet
{
public:
	UFSet(size_t size) : m_parent(size)
	{
		for (int i = 0; i < size; ++i)
			m_parent[i] = i;
	}

	bool Merge(size_t id1, size_t id2)
	{
		int root1 = Root(id1);
		int root2 = Root(id2);
		m_parent[id1] = m_parent[root2] = root1;
		return root1 != root2;
	}

	int Query(size_t id)
	{
		return m_parent[id];
	}

	int Reduce()
	{
		std::unordered_map<size_t, size_t> root_to_rid;
		for (int i = 0; i < m_parent.size(); ++i)
			root_to_rid.insert(std::pair<size_t, size_t>(m_parent[i] = Root(i), root_to_rid.size()));
		for (int i = 0; i < m_parent.size(); ++i)
			m_parent[i] = root_to_rid[m_parent[i]];
		return root_to_rid.size();
	}

private:
	int Root(int id)
	{
		while (id != m_parent[id])
		{
			id = m_parent[id];
			int p = m_parent[id];
			m_parent[id] = m_parent[p];
			id = p;
		}
		return id;
	}
	std::vector<int> m_parent;
};

