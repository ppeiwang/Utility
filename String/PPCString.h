#pragma once
#include <vector>
#include <string>

namespace PEIPEISPACE
{
	// string for ANSI
	// wstring for unicode & multibyte

	/*
	trimLeft:
		std::string strSrc = "abcdabc";
		e.g. trimLeft(strSrc, "a b c e");
		std::cout<<strSrc; // will print "dabc", every character that is in pattern "a b c e" will be deleted
	*/

	template<class T>
	void trim(T& s, const T& strTarget)
	{
		if (s.empty() == false)
		{
			s.erase(0, s.find_first_not_of(strTarget));
			s.erase(s.find_last_not_of(strTarget) + 1);
		}
	}

	template<class T>
	void trimLeft(T &s, const T& strTarget)
	{
		if (s.empty() == false)
		{
			s.erase(0, s.find_first_not_of(strTarget));
		}
	}

	template<class T>
	void trimRight(T& s, const T& strTarget)
	{
		if (s.empty() == false)
		{
			s.erase(s.find_last_not_of(strTarget) + 1);
		}
	}


	template<class T>
	void StringTokenizer(std::vector<T>& _tokens, const T& str, const T& separators, bool bIgnoreEmpty = true)
	{
		typename T::const_iterator it = str.begin();
		typename T::const_iterator end = str.end();

		T token;
		bool lastToken = false;

		for (; it != end; ++it)
		{
			if (separators.find(*it) != T::npos)
			{
				if (!token.empty() || !bIgnoreEmpty) _tokens.push_back(token);
				if (!bIgnoreEmpty) lastToken = true;
				token.clear();
			}
			else
			{
				token += *it;
				lastToken = false;
			}
		}

		if (!token.empty())
		{
			if (!token.empty() || !bIgnoreEmpty) _tokens.push_back(token);
		}
		else if (lastToken)
		{
			_tokens.push_back(T());
		}
	}
	
}