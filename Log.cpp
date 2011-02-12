/*
 * NeoSmart Logging Library
 * Author: Mahmoud Al-Qudsi <mqudsi@neosmart.net>
 * Copyright (C) 2011 by NeoSmart Technologies
 * This library is released under the terms of the MIT License
*/

#include "stdafx.h"
#include "Log.h"
#include <atlstr.h>

using namespace neosmart;

namespace neosmart
{
	__declspec(thread) int IndentLevel = -1;

	Logger logger = Logger();

	TCHAR* logLevelNames[] = {_T("DEBG"), _T("INFO"), _T("WARN"), _T("ERRR")};

	Logger::Logger(LogLevel logLevel)
	{
		_logLevel = logLevel;
	}

	void Logger::InnerLog(LogLevel level, LPCTSTR message, va_list params)
	{
		if(level < _logLevel)
			return;

		CString newMessage;
		CString indentText;

		//Indentation only works if ScopeLog is printing 
		if(_logLevel <= neosmart::Debug)
		{
			for(int i = 0; i < IndentLevel; ++i)
				indentText += _T(" ");
		}

		newMessage.Format(_T("%s%s: %s\r\n"), indentText, logLevelNames[level], message);

		vwprintf_s(newMessage, params);
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

	ScopeLog::ScopeLog(LPCTSTR name) : _name(name)
	{
		++IndentLevel;
		logger.Log(Debug, _T("Entering %s"), _name);
	}

	ScopeLog::~ScopeLog()
	{
		logger.Log(Debug, _T("Leaving %s"), _name);
		--IndentLevel;
	}
}
