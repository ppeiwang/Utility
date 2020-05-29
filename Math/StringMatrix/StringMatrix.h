#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include "TextTable.h"

#define COLUNM_MAJOR
//#define ROW_MAJOR
using std::string;

template<size_t D>
class StringMatrix
{
public: 
#ifdef COLUNM_MAJOR
	/*
	|1	|4	|7	|
	|2	|5	|8	|
	|3	|6	|9	|
	*/
	std::string data[D][D];
#else
	/*
	|1	|2	|3	|
	|4	|5	|6	|
	|7	|8	|9	|
	*/
	std::string data[D * D];
#endif

	std::string mPlacehold = "0";

	StringMatrix() = default;

	StringMatrix(const string s)
	{
		for (int i = 0; i < D; i++)
			for (int j = 0; j < D; j++)
				data[i][j] = s;
	}

	void Construct(const std::vector<string>& sequence)
	{
		assert(sequence.size() == D * D);
#ifdef COLUNM_MAJOR
		std::copy(sequence.begin(), sequence.end(), &data[0][0]);
#else
		std::copy(sequence.begin(), sequence.end(), &data[0]);
#endif
	}
	
	string& operator [] (size_t i)
	{
		return *(&(data[0][0]) + i);
	}

	const string& operator [] (size_t i) const
	{
		return *(&(data[0][0]) + i);
	}

	StringMatrix operator* (const StringMatrix& other) const
	{
		StringMatrix<D> MProduct;
		for (size_t i = 0; i < D; i++)
		{
			const auto RIndices = GetRow(i);
			for (size_t j = 0; j < D; j++)
			{
				const auto CIndices = GetColunm(j);
				std::vector<string> products;
				for (size_t n = 0; n < RIndices.size(); n++)
				{
					const auto l = operator[](RIndices[n]);
					const auto r = other[CIndices[n]];
					const auto p = Mul(l, r);
					products.push_back(p);
					//products.push_back(Mul(operator[](RIndices[n]), other[CIndices[n]]));
				}
				string sum = "0";
				std::for_each(products.begin(), products.end(), [&](const string& s) { sum = Add(sum, s); });
				const auto index = ConvertIndex(i, j);
				MProduct[index] = sum;
			}
		}
		return MProduct;
	}

	StringMatrix Transpose()
	{
		StringMatrix<D> m;
		for (size_t i = 0; i < D; i++)
		{
			for (size_t j = 0; j < D; j++)
			{
				auto index0 = ConvertIndex(i, j);
				auto index1 = ConvertIndex(j, i);
				m[index0] = operator[](index1);
			}
		}

		return m;
	}

	string ToString()const
	{
		TextTable t('-', '|', '+');

		for (size_t i = 0; i < D; i++)
		{
			auto rVec = GetRow(i);
			for (auto r : rVec)
			{
				t.add(operator[](r));
			}
			t.endOfRow();
		}

		t.setAlignment(2, TextTable::Alignment::RIGHT);

		std::stringstream ss;
		ss << t;
		return ss.str();
	}

	void Print()const
	{
		std::cout << ToString() << std::endl;
	}

	static StringMatrix RotateX(const std::string s)
	{
		StringMatrix m("0");
#ifdef COLUNM_MAJOR
		m[0 + 4*0] = "1";
		m[1 + 4 * 1] = "c"+s;
		m[2 + 4 * 1] = "s"+s;
		m[1 + 4 * 2] = "-s"+s;
		m[2 + 4 * 2] = "c"+s;
		m[3 + 4 * 3] = "1";
#else
		m[0 + 4*0] = "1";
		m[1 + 4 * 1] = "c"+s;
		m[1 + 4 * 2] = "s"+s;
		m[2 + 4 * 1] = "-s"+s;
		m[2 + 4 * 2] = "c"+s;
		m[3 + 4 * 3] = "1";
#endif
		return m;
	}

	static StringMatrix RotateY(const std::string s)
	{
		StringMatrix m("0");
#ifdef COLUNM_MAJOR
		m[0 + 4 * 0] = "c"+s;
		m[2 + 4 * 0] = "-s" + s;
		m[1 + 4 * 1] = "1";
		m[0 + 4 * 2] = "s" + s;
		m[2 + 4 * 2] = "c" + s;
		m[3 + 4 * 3] = "1";
#else
		m[0 + 4 * 0] = "c"+s;
		m[0 + 4 * 2] = "-s" + s;
		m[1 + 4 * 1] = "1";
		m[2 + 4 * 0] = "s" + s;
		m[2 + 4 * 2] = "c" + s;
		m[3 + 4 * 3] = "1";
#endif
		return m;
	}

	static StringMatrix RotateZ(const std::string s)
	{
		StringMatrix m("0");
#ifdef COLUNM_MAJOR
		m[0 + 4 * 0] = "c" + s;
		m[1 + 4 * 0] = "s" + s;
		m[0 + 4 * 1] = "-s" + s;
		m[1 + 4 * 1] = "c" + s;
		m[2 + 4 * 2] = "1";
		m[3 + 4 * 3] = "1";
#else
		m[0 + 4 * 0] = "c" + s;
		m[0 + 4 * 1] = "s" + s;
		m[1 + 4 * 0] = "-s" + s;
		m[1 + 4 * 1] = "c" + s;
		m[2 + 4 * 2] = "1";
		m[3 + 4 * 3] = "1";
#endif
		return m;
	}

private:
	size_t ConvertIndex(size_t row, size_t colunm) const
	{
#ifdef COLUNM_MAJOR
		return colunm * D + row;
#else
		return row * D + colunm;
#endif
	}

	std::vector<size_t> GetColunm(size_t n) const
	{
		std::vector<size_t> c;
#ifdef COLUNM_MAJOR
		for (size_t i = 0; i < D; i++)
		{
			c.push_back(n * D + i);
		}
#else
		for (size_t i = 0; i < D; i++)
		{
			c.push_back(i * D + n);
		}
#endif
		return c;
	}

	std::vector<size_t> GetRow(size_t n) const
	{
		std::vector<size_t> r;

#ifdef COLUNM_MAJOR
		for (size_t i = 0; i < D; i++)
		{
			r.push_back(i * D + n);
		}
#else
		for (size_t i = 0; i < D; i++)
		{
			r.push_back(n * D + i);
		}
#endif	
		return r;
	}

	string Mul(const string& a, const string& b) const
	{
		if (Skip(a) || Skip(b))
		{
			return mPlacehold;
		}

		if (a == "1")
		{
			return b;
		}

		if (b == "1")
		{
			return a;
		}

		if (a.front() == '-')
		{
			if (b.front() == '-')
			{
				return string(a.begin() + 1, a.end()) + "*" + string(b.begin() + 1, b.end());
			}
			else
			{
				return a + "*" + b;
			}
		}
		
		if (b.front() == '-')
		{
			return "-" + a + "*" + string(b.begin() + 1, b.end());
		}

		return a + "*" + b;
	}

	string Add(const string& a, const string& b) const
	{
		if (Skip(a))
		{
			return b;
		}

		if (Skip(b))
		{
			return a;
		}
		if (b.front() == '-')
		{
			return a + b;
		}

		return a + "+" + b;
	}

	bool Skip(const string& s) const
	{
		return s.empty() || s == "0";
	}

};
