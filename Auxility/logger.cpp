#include "logger.hpp"
#include <assert.h>

//CLogger::CInstance CLogger::m_insInstance; // provide to be initialized by single thread

CLogger::CLogger()
{
	m_logger = spdlog::basic_logger_mt("PPLogger", "Convertor.log", true);
	m_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [Thread %t] %l: %v");
	
	m_stdLogger = spdlog::stdout_color_mt("Console");
	m_stdLogger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [Thread %t] %v");
}

CLogger::~CLogger()
{
	spdlog::drop_all();
}

CLogger& CLogger::GetInstance()
{
	static CLogger static_instance;

	return static_instance;
}

std::shared_ptr<spdlog::logger> CLogger::GetLogger()
{
	return spdlog::get("PPLogger");
}

std::shared_ptr<spdlog::logger> CLogger::GetConsole()
{
	return spdlog::get("Console");
}

void CLogger::SetStreamHook(std::function<void(const QString&, int)> f)
{
	m_hookFuncs.push_back(f);
}

void CLogger::OutputError(const QString& msg) const
{
#ifdef DEBUG
	assert(false);
#endif
	const QByteArray utf8 = msg.toUtf8();
	m_logger->error(utf8.data());
	const QByteArray gbk = msg.toLocal8Bit();
	m_stdLogger->error(gbk.data());
	for (auto& hook : m_hookFuncs)
	{
		if (hook)
		{
			hook(msg, 2);
		}
	}
}

void CLogger::OutputWarning(const QString& msg) const
{
#ifdef DEBUG
	assert(false);
#endif
	const QByteArray utf8 = msg.toUtf8();
	m_logger->warn(utf8.data());
	const QByteArray gbk = msg.toLocal8Bit();
	m_stdLogger->warn(gbk.data());
	for (auto& hook : m_hookFuncs)
	{
		if (hook)
		{
			hook(msg, 1);
		}
	}
}

void CLogger::OutputInfo(const QString& msg) const
{
	const QByteArray utf8 = msg.toUtf8();
	const QByteArray gbk = msg.toLocal8Bit();
	m_logger->info(utf8.data());
	m_stdLogger->info(gbk.data());
	for (auto& hook : m_hookFuncs)
	{
		if (hook)
		{
			hook(msg, 0);
		}
	}
}

void CLogger::ConsolePrintInfo(const QString& msg) const
{
	QByteArray gbk = msg.toLocal8Bit();
	m_stdLogger->info(gbk.data());
}