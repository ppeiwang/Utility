#pragma once
#include <string>
#include <chrono>
#include <ctime>

namespace PEIPEISPACE
{
	std::string AGetCurrentTime(const char* format = "%Y.%m.%d %H:%M:%S")
	{
		auto now = std::chrono::system_clock::now();
		time_t t = std::chrono::system_clock::to_time_t(now);
		tm tt;
		localtime_s(&tt, &t);
		char buffer[128];
		strftime(buffer, sizeof(buffer), format, &tt);
		return std::string(buffer);
	}

	std::wstring WGetCurrentTime(const wchar_t* format = L"%Y.%m.%d %H:%M:%S")
	{
		auto now = std::chrono::system_clock::now();
		time_t t = std::chrono::system_clock::to_time_t(now);
		tm tt;
		localtime_s(&tt, &t);
		wchar_t buffer[128];
		wcsftime(buffer, sizeof(buffer)/sizeof(buffer[0]), format, &tt);
		return std::wstring(buffer);
	}
}