#include <stdio.h>

#include <random>
#include <string>
#include <vector>
using namespace std;

#include "his/DataStructure/SegmentTree.hpp"

/*
	Use SegmentTree for range summation on a vector<int>.
	That is, given a vector, and a range [start, end), compute
	the sum of integers in that range.

		int sum_from_start_to_end = segment_tree.query(start, end);

	is equivalent to:

		int sum_from_start_to_end = 0;
		for (size_t i = start; i < end; ++i)
			sum_from_start_to_end += values[i];
*/
void TestSummation(size_t test_size)
{
	// initialize a vector randomly
	int test_min = -1000;
	int test_max = 1000;
	
	vector<int> values(test_size);
	for (size_t i = 0; i < values.size(); ++i)
		values[i] = rand() % (test_max - test_min) + test_min;
	
	// for operator+ on integers, the identity element is 0
	his::SegmentTree<int> segm_tree(begin(values), end(values), 0,
		[](int a, int b)->int { return a + b; });

	auto segment_summation = [&](size_t start, size_t end)->int
	{
		int sum = 0;
		for (size_t i = start; i < end; ++i)
			sum += values[i];
		return sum;
	};

	for (size_t i = 0; i < values.size(); ++i)
	{
		for (size_t j = 0; j <= values.size(); ++j)
		{
			int sum = segm_tree.query(i, j);
			if (sum != segment_summation(i, j))
				printf("Error [%d %d)\n", i, j);
		}
	}
}

/*
	Use SegmentTree on a string concatenation problem.
	Given a long string, and a range [start, end), concatenate
	the characters from start to end(exclusive).
	In this case, the identity element is an empty string,
	and the concatenation operation is noncommutative:
	(a + b != b + a for string a and string b)
	
	This is just a sample, the query time is not O(logN) because
	of the string copy cost.
*/
void TestStringConcatenation(size_t test_size)
{
	// initialize a long string randomly
	string long_string;
	long_string.reserve(test_size);
	for (size_t i = 0; i < test_size; ++i)
		long_string.push_back(rand()%26+'a');
	
	// for string concatenation the identity element is an empty string
	his::SegmentTree<string> segm_tree(begin(long_string), end(long_string), "",
		[](const string &a, const string &b)->string
	{
		return a + b;
	});

	for (size_t i = 0; i < long_string.size(); ++i)
	{
		for (size_t j = i; j <= long_string.size(); ++j)
		{
			string substring = segm_tree.query(i, j);
			if (substring != long_string.substr(i, j-i))
				printf("Error [%d %d)\n", i, j);
		}
	}
}

int main()
{
	TestSummation(100);
	TestStringConcatenation(100);

	return 0;
}