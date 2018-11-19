#pragma once
#include <vector>
#include <string>
#include <locale>
#include <codecvt>

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

	template<typename T=std::string>
	T ParternReplace(const T& str, const T& partern, const T& pReplace)
	{
		T strSrc = str;
		typename T::size_type pos = 0;
		typename T::size_type srclen = partern.length();
		typename T::size_type dstlen = pReplace.length();
		while ((pos = strSrc.find(partern, pos)) != T::npos)
		{
			strSrc.replace(pos, srclen, pReplace);
			pos += dstlen;
		}
		return strSrc;
	}

	// equal to CString's Left
	template<typename T>
	T Left(const T& str, size_t nIndex)
	{
		if (nIndex < str.length())
		{
			return str.substr(0, nIndex);
		}

		return T();
	}

	// equal to CString's Mid
	template<typename T>
	T Mid(const T& str, size_t nIndex)
	{
		if (nIndex < str.length())
		{
			return str.substr(nIndex);
		}

		return T();
	}

	// equal to CString's Right
	template<typename T>
	T Right(const T& str, size_t nIndex)
	{
		if (nIndex <= str.length())
		{
			return str.substr(str.length() - nIndex);
		}

		return T();
	}

	template<typename T>
	std::string ConvertWideCharToGBK(const std::wstring& wStrSrc)
	{
		static std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>> cvt_ansi{ new std::codecvt_byname<wchar_t, char, std::mbstate_t>(".936") };
		std::string strGbk = cvt_ansi.to_bytes(wStrSrc);
		return std::move(strGbk);
	}

	template<typename T>
	std::wstring ConvertGBKToWideChar(const std::string& strSrc)
	{
		static std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>> cvt_ansi{ new std::codecvt_byname<wchar_t, char, std::mbstate_t>(".936") };
		std::wstring strGbk = cvt_ansi.from_bytes(strSrc);
		return std::move(strGbk);
	}
	
	template<typename T>
	std::string ConvertWideCharToUtf8(const std::wstring& wStrSrc)
	{
		static std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
		std::string strUtf8 = myconv.to_bytes(wStrSrc);
		return std::move(strUtf8);
	}

	template<typename T>
	std::wstring ConvertUtf8ToWideChar(const std::string& strSrc)
	{
		static std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
		std::wstring wstrUtf16 = myconv.from_bytes(strSrc);
		return std::move(wstrUtf16);
	}
}