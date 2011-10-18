/*
 * NeoSmart Logging Library
 * Author: Mahmoud Al-Qudsi <mqudsi@neosmart.net>
 * Copyright (C) 2011 by NeoSmart Technologies
 * This code is released under the terms of the MIT License
*/

#pragma once

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

		vwprintf_s(mask, params);

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

	ScopeLog::ScopeLog(LPCSTR name)
	{
		_allocated = true;
		LPCTSTR newName = _tcsdup(CString(name));
		Initialize(newName);
	}

	ScopeLog::~ScopeLog()
	{
		logger.Log(Debug, _T("Leaving %s"), _name);
		--IndentLevel;

		if(_allocated)
			free((void*) _name);
	}
}
