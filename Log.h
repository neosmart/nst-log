/*
 * NeoSmart Logging Library
 * Author: Mahmoud Al-Qudsi <mqudsi@neosmart.net>
 * Copyright (C) 2011 by NeoSmart Technologies
 * This code is released under the terms of the MIT License
*/

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

	class ScopeLog
	{
		LPCTSTR _name;
		bool _allocated;

		void Initialize(LPCTSTR name);

	public:
		ScopeLog(LPCTSTR name);
		ScopeLog(LPCSTR name);

		~ScopeLog();
	};

	extern Logger logger;
}
