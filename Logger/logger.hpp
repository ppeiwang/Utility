#pragma once
#include <QString>
#include "Spdlog/spdlog.h"
/*
	Auther: Peiwang comeback74@vip.qq.coms
	Date: 2018.3.1
*/

// Output log file is encoded by UTF-8

namespace PEIPEISPACE
{
	class CLogger
	{
	public:
		static CLogger& GetInstance();

		std::shared_ptr<spdlog::logger> GetLogger();

		std::shared_ptr<spdlog::logger> GetConsole();

		void SetStreamHook(std::function<void(const QString&, int)>);

	public:
		void OutputError(const QString& msg) const;

		void OutputWarning(const QString& msg) const;

		void OutputInfo(const QString& msg) const;

		void ConsolePrintInfo(const QString& msg) const;

	private:
		CLogger();

		~CLogger();

		class CInstance
		{
		public:
			CInstance()
			{
				GetInstance();
			}
		};

		//static CInstance m_insInstance; // 取消线程安全的设计，避免开启多个进程时日志文件句柄被占用，导致exception抛出. 

	private:
		std::shared_ptr<spdlog::logger> m_logger;
		std::shared_ptr<spdlog::logger> m_stdLogger;
		std::vector<std::function<void(const QString&, int)>> m_hookFuncs;
	};
}