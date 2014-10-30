/*
 * NeoSmart Logging Library
 * Author: Mahmoud Al-Qudsi <mqudsi@neosmart.net>
 * Copyright (C) 2012 by NeoSmart Technologies
 * This code is released under the terms of the MIT License
*/

#pragma once

#include <map>
#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#define __thread __declspec(thread)
#else
typedef char TCHAR;
#include <stdarg.h>
#define _T(x) (x)
typedef const char *LPCTSTR;
#endif

/* Notes on synchronization
 * C++11 changes the behavior of cout and cerr, in particular:
	* cerr is tied to cout, meaning cout will be flushed on calls to cerr
	* cerr is now sync'd by default (ios_base::sync_with_stdio)
 * By default, cerr is unbuffered. Call std::cout.setf(std::ios::unitbuf)
 * to fully-synchronize your cout writes to your cerr writes, if they are 
 * both log outputs.
*/

namespace neosmart
{
#ifdef _WIN32
	typedef std::wostream ostream;
#else
	typedef std::ostream ostream;
#endif

	extern __thread int IndentLevel;

	enum LogLevel
	{
		Debug,
		Info,
		Warn,
		Error,
		Passthru,
		None
	};

	class Logger
	{
	private:
		LogLevel _logLevel;
		std::map<ostream*, LogLevel> _outputs;
		ostream *_defaultLog;

		void InnerLog(LogLevel level, LPCTSTR message, va_list params);
		void Broadcast(LogLevel level, LPCTSTR message);

	public:
		Logger(LogLevel logLevel = neosmart::Warn);

		void SetLogLevel(LogLevel level);
		void AddLogDestination(ostream &output);
		void AddLogDestination(ostream &output, LogLevel level);
		void ClearLogDestinations();

		void Log(LogLevel level, LPCTSTR message, ...);

		//Convenience Functions
		void Log(LPCTSTR message, ...);
		void Debug(LPCTSTR message, ...);
		void Info(LPCTSTR message, ...);
		void Warn(LPCTSTR message, ...);
		void Error(LPCTSTR message, ...);
		void Passthru(LPCTSTR message, ...);
	};

	class ScopeLog
	{
		LPCTSTR _name;
		bool _allocated;

		void Initialize(LPCTSTR name);

	public:
		ScopeLog(LPCTSTR name);
#ifdef _WIN32
		ScopeLog(LPCSTR name);
#endif

		~ScopeLog();
	};

	extern Logger logger;
}
