/*
 * NeoSmart Logging Library
 * Author: Mahmoud Al-Qudsi <mqudsi@neosmart.net>
 * Copyright (C) 2011 by NeoSmart Technologies
 * This library is released under the terms of the MIT License
*/

#pragma once

#include <atlstr.h>

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
		CString _name;

	public:
		ScopeLog(LPCTSTR name);

		~ScopeLog();
	};
}
