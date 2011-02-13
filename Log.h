#pragma once

#include <Windows.h>

namespace neosmart
{
	extern __declspec(thread) int IndentLevel;

	enum LogLevel
	{
		Debug,
		Info,
		Warn,
		Error
	};

	class Logger
	{
	private:
		LogLevel _logLevel;

		void InnerLog(LogLevel level, LPCTSTR message, va_list params);

	public:
		Logger(LogLevel logLevel = neosmart::Warn);

		void SetLogLevel(LogLevel level);

		void Log(LogLevel level, LPCTSTR message, ...);
		void Log(LPCTSTR message, ...);

		//Convenience Functions
		void Debug(LPCTSTR message, ...);
		void Info(LPCTSTR message, ...);
		void Warn(LPCTSTR message, ...);
		void Error(LPCTSTR message, ...);
	};

	extern Logger logger;

	class ScopeLog
	{
		LPCTSTR _name;

	public:
		ScopeLog(LPCTSTR name);

		~ScopeLog();
	};
}
