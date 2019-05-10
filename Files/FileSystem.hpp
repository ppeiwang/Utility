#pragma  once

#include <filesystem>
#include <vector>

/*
*	C++17 is needed
*/

namespace PEIPEISPACE
{
	using STD_PATH = std::filesystem::path;

	// Search a file under the search path: strSerachPath
	template<typename T= STD_PATH>
	bool SearchFile(const std::string& fileName, const std::string& strSearchPath, T& filePath)
	{
		bool bFound = false;
		for (auto& p : std::filesystem::recursive_directory_iterator(strSearchPath))
		{
			if (p.is_directory() == false)
			{
				const auto _path = p.path();
				if (_path.filename() == fileName)
				{
					bFound = true;
					filePath = _path;
					break;
				}
			}
		}

		return bFound;
	}

	template<>
	bool SearchFile(const std::string& fileName, const std::string& strSearchPath, std::vector<STD_PATH>& filePathList)
	{	
		bool bFound = false;
		for (auto& p : std::filesystem::recursive_directory_iterator(strSearchPath))
		{
			if (p.is_directory() == false)
			{
				const auto _path = p.path();
				if (_path.filename() == fileName)
				{
					bFound = true;
					filePathList.push_back(std::move(_path));
				}
			}
		}

		return bFound;
	}

}