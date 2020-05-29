#pragma once

#include "String/PPCString.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <assert.h>
#include "TestCase.h"

namespace PEIPEISPACE
{

#ifdef _WIN32
	static std::string GBK_CODE_PAGE_NAME = ".936";
	static std::string UTF8_CODE_PAGE_NAME = ".65001";
#elif __linux__

#endif

	class CStringTestCase : public CTestCase
	{
	public:
		CStringTestCase() = default;

		~CStringTestCase() = default;

		virtual void operator() ()
		{
			const std::wstring strInstance = L" C盘\\我的文件\\tokens.txt";
			std::vector<std::wstring> tokens;
			PEIPEISPACE::StringTokenizer<std::wstring>(tokens, strInstance, L"\\ ");
			std::wcout.imbue(std::locale(GBK_CODE_PAGE_NAME));
			for (const auto& token : tokens)
			{
				std::wcout << token << std::endl;
			}

			const std::string strUtf8 = u8"这是utf8哦.txt";
			const std::wstring strGbk = L"这是GBK哦.txt";
			const std::string sht = "这是GBK编码";

			std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
			std::wstring wstrConv = myconv.from_bytes(strUtf8);
			std::wcout << wstrConv;

			std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>> cvt_ansi{ new std::codecvt<wchar_t, char, std::mbstate_t>("CHS") };
			std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>> cvt_utf8{ new std::codecvt_byname<wchar_t, char, std::mbstate_t>(".65001") };

			const std::wstring wstr = L"你好吗";
			const wchar_t* pwchar = L"你好吗";
			cvt_ansi.to_bytes(wstr);
			//myconv.to_bytes(L"你好吗");

			std::wofstream outFile;
			outFile.open(wstrConv, std::ios::out);
			if (outFile.is_open())
			{
				//outFile.imbue(std::locale(outFile.getloc(), new std::codecvt_byname<wchar_t, char, std::mbstate_t>(GBK_CODE_PAGE_NAME)));
				outFile.imbue(std::locale(UTF8_CODE_PAGE_NAME));
				outFile << wstr << 123 << 123.123 << std::endl;
			}
			else
			{
				assert(false);
			}

			std::ifstream inFile;
			inFile.open(wstrConv, std::ios::in);

			getchar();
		}
	};

}