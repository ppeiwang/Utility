#pragma once
#include <string>

namespace PEIPEISPACE
{
	void trim(std::string &s, const std::string& strTarget)
	{
		if (s.empty() == false)
		{
			s.erase(0, s.find_first_not_of(strTarget));
			s.erase(s.find_last_not_of(" ") + 1);
		}
	}

	void trimLeft(std::string &s, const std::string& strTarget)
	{
		if (s.empty() == false)
		{
			s.erase(0, s.find_first_not_of(strTarget));
		}
	}

	void trimRight(std::string &s, const std::string& strTarget)
	{
		if (s.empty() == false)
		{
			s.erase(s.find_last_not_of(strTarget) + 1);
		}
	}
}