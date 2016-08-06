/*
 * NeoSmart Logging Library
 * Author: Mahmoud Al-Qudsi <mqudsi@neosmart.net>
 * Copyright (C) 2012 by NeoSmart Technologies
 * This code is released under the terms of the MIT License
*/

#pragma once

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#include <tchar.h>
#include "Windows.h"
#define __thread __declspec(thread)
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#define _tcsclen strlen
#define _stprintf_s snprintf
#define vwprintf_s vprintf
#define _vsntprintf vsnprintf 
typedef char TCHAR;
#include <stdarg.h>
#define _T(x) (x)
typedef const char *LPCTSTR;
#endif

#include <map>
#include <iostream>
#ifndef TINYFORMAT_USE_VARIADIC_TEMPLATES
#define UNDEF_TINYFORMAT_USE_VARIADIC_TEMPLATES
#define TINYFORMAT_ALLOW_WCHAR_STRINGS
#define TINYFORMAT_USE_VARIADIC_TEMPLATES
#endif
#include "tinyformat.h"
#include <cassert>

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
#if defined(_WIN32) && defined(UNICODE)
	typedef std::wostream ostream;
#else
	typedef std::ostream ostream;
#endif

	extern __thread int IndentLevel;
	static LPCTSTR logPrefixes[] = { _T("DEBG: "), _T("INFO: "), _T("WARN: "), _T("ERRR: "), _T("") };

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

		template<typename... Args>
		inline void InnerLog(LogLevel level, LPCTSTR message, const Args&... args)
		{
			TCHAR *mask;

			//As an optimization, we're not going to check level so don't pass in None!
			assert(level >= LogLevel::Debug && level <= LogLevel::Passthru);

			//Indentation only works if ScopeLog is printing 
			size_t size = 4 + 2 + _tcsclen(message) + 2 + 1;
			if (IndentLevel >= 0 && _logLevel <= neosmart::Debug)
			{
				size += (size_t)IndentLevel;
				mask = new TCHAR[size];
				_stprintf_s(mask, size, _T("%*s%s\r\n"), IndentLevel + 4, logPrefixes[level], message);
			}
			else
			{
				mask = new TCHAR[size];
				_stprintf_s(mask, size, _T("%s%s\r\n"), logPrefixes[level], message);
			}

			std::string final = tfm::format(mask, args...);

			Broadcast(level, final.c_str());
			delete[] mask;
		}

		void Broadcast(LogLevel level, LPCTSTR message);

	public:
		Logger(LogLevel logLevel = neosmart::Warn);

		void SetLogLevel(LogLevel level);
		void AddLogDestination(ostream &output);
		void AddLogDestination(ostream &output, LogLevel level);
		void ClearLogDestinations();

		template<typename... Args>
		inline void Log(LogLevel level, LPCTSTR message, const Args&... args)
		{
			InnerLog(level, message, args...);
		}

		//Convenience Functions
		template<typename... Args>
		inline void Log(LPCTSTR message, const Args&... args)
		{
			InnerLog(neosmart::Info, message, args...);
		}

		template<typename... Args>
		inline void Debug(LPCTSTR message, const Args&... args)
		{
			InnerLog(neosmart::Debug, message, args...);
		}

		template<typename... Args>
		inline void Info(LPCTSTR message, const Args&... args)
		{
			InnerLog(neosmart::Info, message, args...);
		}

		template<typename... Args>
		inline void Warn(LPCTSTR message, const Args&... args)
		{
			InnerLog(neosmart::Warn, message, args...);
		}

		template<typename... Args>
		inline void Error(LPCTSTR message, const Args&... args)
		{
			InnerLog(neosmart::Error, message, args...);
		}

		template<typename... Args>
		inline void Passthru(LPCTSTR message, const Args&... args)
		{
			InnerLog(neosmart::Passthru, message, args...);
		}
	};

	class ScopeLog
	{
		LPCTSTR _name;

		void Initialize(LPCTSTR name);
#if defined(_WIN32) && defined(UNICODE)
		void Initialize(LPCSTR name);
#endif

	public:
		ScopeLog(LPCTSTR name);
#if defined(_WIN32) && defined(UNICODE)
		ScopeLog(LPCSTR name);
#endif
		~ScopeLog();
	};

	extern Logger logger;
}

#ifdef UNDEF_TINYFORMAT_USE_VARIADIC_TEMPLATES
#undef TINYFORMAT_USE_VARIADIC_TEMPLATES
#endif
