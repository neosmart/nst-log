/*
 * NeoSmart Logging Library
 * Author: Mahmoud Al-Qudsi <mqudsi@neosmart.net>
 * Copyright (C) 2012 by NeoSmart Technologies
 * This code is released under the terms of the MIT License
*/

#include <assert.h>
#ifdef WIN32
#include <atlstr.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _tcsclen strlen
#define _stprintf_s snprintf
#define vwprintf_s vprintf
#define _vsntprintf vsnprintf 
#endif

#include "Log.h"

using namespace neosmart;
using namespace std;

namespace neosmart
{
	__thread int IndentLevel = -1;

	Logger logger;

	static LPCTSTR logPrefixes[] = {_T("DEBG: "), _T("INFO: "), _T("WARN: "), _T("ERRR: "), _T("")};

	Logger::Logger(LogLevel logLevel)
	{
		_logLevel = logLevel;
#ifdef WIN32
		_defaultLog = &std::wcout;
#else
		_defaultLog = &std::cout;
#endif
		AddLogDestination(*_defaultLog, logLevel);
	}

	void Logger::InnerLog(LogLevel level, LPCTSTR message, va_list params)
	{
		TCHAR *mask;

		//As an optimization, we're not going to check level so don't pass in None!
		assert(level >= Debug && level <= Passthru);

		//Indentation only works if ScopeLog is printing 
		size_t size = 4 + 2 + _tcsclen(message) + 2 + 1;
		if(IndentLevel >= 0 && _logLevel <= neosmart::Debug)
		{
			size += (size_t) IndentLevel;
			mask = new TCHAR[size];
			_stprintf_s(mask, size, _T("%*s%s\r\n"), IndentLevel + 4, logPrefixes[level], message);
		}
		else
		{
			mask = new TCHAR[size];
			_stprintf_s(mask, size, _T("%s%s\r\n"), logPrefixes[level], message);
		}

#ifdef WIN32
		size_t length = _vsntprintf(NULL, 0, mask, params);
#else
		//See http://stackoverflow.com/questions/8047362/is-gcc-mishandling-a-pointer-to-a-va-list-passed-to-a-function
		va_list args_copy;
		va_copy(args_copy, params);
		size_t length = _vsntprintf(NULL, 0, mask, args_copy);
		va_end(args_copy);
#endif
		TCHAR *final = new TCHAR [length + 1];
		_vsntprintf(final, length + 1, mask, params);

		Broadcast(level, final);

		delete [] final;
		delete [] mask;
	}

	void Logger::Broadcast(LogLevel level, LPCTSTR message)
	{
		for(map<ostream*, LogLevel>::iterator i = _outputs.begin(); i != _outputs.end(); ++i)
		{
			if(level < i->second)
				continue;
			(*i->first) << message;
		}
	}

	void Logger::Log(LogLevel level, LPCTSTR message, ...)
	{
		va_list va_args;
		va_start(va_args, message);
		InnerLog(level, message, va_args);
		va_end(va_args);
	}

	void Logger::Log(LPCTSTR message, ...)
	{
		va_list va_args;
		va_start(va_args, message);
		InnerLog(neosmart::Info, message, va_args);
		va_end(va_args);
	}

	void Logger::Debug(LPCTSTR message, ...)
	{
		va_list va_args;
		va_start(va_args, message);
		InnerLog(neosmart::Debug, message, va_args);
		va_end(va_args);
	}

	void Logger::Info(LPCTSTR message, ...)
	{
		va_list va_args;
		va_start(va_args, message);
		InnerLog(neosmart::Info, message, va_args);
		va_end(va_args);
	}

	void Logger::Warn(LPCTSTR message, ...)
	{
		va_list va_args;
		va_start(va_args, message);
		InnerLog(neosmart::Warn, message, va_args);
		va_end(va_args);
	}

	void Logger::Error(LPCTSTR message, ...)
	{
		va_list va_args;
		va_start(va_args, message);
		InnerLog(neosmart::Error, message, va_args);
		va_end(va_args);
	}

	void Logger::Passthru(LPCTSTR message, ...)
	{
		va_list va_args;
		va_start(va_args, message);
		InnerLog(neosmart::Passthru, message, va_args);
		va_end(va_args);
	}

	void Logger::SetLogLevel(LogLevel logLevel)
	{
		_outputs[_defaultLog] = logLevel;
		_logLevel = logLevel;
	}

	void Logger::AddLogDestination(neosmart::ostream &destination)
	{
		return AddLogDestination(destination, _logLevel);
	}

	void Logger::AddLogDestination(neosmart::ostream &destination, LogLevel level)
	{
		_outputs[&destination] = level;
	}

	void Logger::ClearLogDestinations()
	{
		_outputs.clear();
	}

	void ScopeLog::Initialize(LPCTSTR name)
	{
		_name = name;
		++IndentLevel;
		logger.Log(Debug, _T("Entering %s"), _name);
	}

	ScopeLog::ScopeLog(LPCTSTR name)
	{
		_allocated = false;
		Initialize(name);
	}

#ifdef _WIN32
	ScopeLog::ScopeLog(LPCSTR name)
	{
		_allocated = true;
		LPCTSTR newName = _tcsdup(CString(name));
		Initialize(newName);
	}
#endif

	ScopeLog::~ScopeLog()
	{
		logger.Log(Debug, _T("Leaving %s"), _name);
		--IndentLevel;

		if(_allocated)
			free((void*) _name);
	}
}
