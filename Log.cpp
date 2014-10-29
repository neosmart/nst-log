/*
 * NeoSmart Logging Library
 * Author: Mahmoud Al-Qudsi <mqudsi@neosmart.net>
 * Copyright (C) 2012 by NeoSmart Technologies
 * This code is released under the terms of the MIT License
*/

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

	Logger logger = Logger();

	LPCTSTR logLevelNames[] = {_T("DEBG"), _T("INFO"), _T("WARN"), _T("ERRR")};

	Logger::Logger(LogLevel logLevel)
	{
		_logLevel = logLevel;
#ifdef WIN32
		AddLogDestination(std::wcout);
#else
		AddLogDestination(std::cout);
#endif
		_consoleOnly = true;
	}

	void Logger::InnerLog(LogLevel level, LPCTSTR message, va_list params)
	{
		if(level < _logLevel)
			return;

		TCHAR *mask;

		//Indentation only works if ScopeLog is printing 
		size_t size = 4 + 2 + _tcsclen(message) + 2 + 1;
		if(IndentLevel >= 0 && _logLevel <= neosmart::Debug)
		{
			size += (size_t) IndentLevel;
			mask = new TCHAR[size];
			_stprintf_s(mask, size, _T("%*s: %s\r\n"), IndentLevel + 4, logLevelNames[level], message);
		}
		else
		{
			mask = new TCHAR[size];
			_stprintf_s(mask, size, _T("%s: %s\r\n"), logLevelNames[level], message);
		}

		if(_consoleOnly)
		{
			vwprintf_s(mask, params);
		}
		else
		{
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

			for(vector<ostream*>::iterator i = _outputs.begin(); i != _outputs.end(); ++i)
			{
				(**i) << final;
			}

			delete [] final;
		}

		delete [] mask;
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

	void Logger::SetLogLevel(LogLevel logLevel)
	{
		_logLevel = logLevel;
	}

	void Logger::AddLogDestination(neosmart::ostream &destination)
	{
		_consoleOnly = false;
		_outputs.push_back(&destination);
	}

	void Logger::ClearLogDestinations()
	{
		_consoleOnly = false;
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
