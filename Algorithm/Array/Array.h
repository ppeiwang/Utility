#pragma once
#include <map>
#include <iostream>
#include <set>
#include <unordered_map>
#include <vector>
#include <assert.h>
#include <utility>

template<typename T = int>
void Partition(std::vector<T> & vec, const size_t begin, const size_t end, size_t index)
{
	if (begin > end || end - begin < 2)
	{
		return;
	}

	if (index >= begin + end - begin)
	{
		assert(false);
		return;
	}

	if (index != begin)
	{
		std::swap(vec[begin], vec[index]);
		index = begin;
	}

	const T V = vec[index];
	size_t pilot = begin;
	size_t high = begin;

	while (high < end)
	{
		if (vec[++high] >= V)
		{
			continue;
		}
		else
		{
			std::swap(vec[high], vec[++pilot]);
		}
	}

	std::swap(vec[index], vec[pilot]);
}

void merge(std::vector<int>& nums1, int m, std::vector<int>& nums2, int n)
{
	std::vector<int> tmp;

	const std::vector<int>& F = nums1;
	const std::vector<int>& S = nums2;
	size_t i = 0;
	size_t j = 0;
	while (i < m || j < n)
	{
		if (i < m && j < n)
		{
			if (F[i] < S[j])
			{
				tmp.push_back(F[i++]);
			}
			else
			{
				tmp.push_back(S[j++]);
			}
		}
		else
		{
			if (i < m)
			{
				tmp.push_back(F[i++]);
			}
			else
			{
				tmp.push_back(S[j++]);
			}
		}
	}

	nums1 = std::move(tmp);
}
