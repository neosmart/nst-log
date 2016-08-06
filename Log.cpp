/*
 * NeoSmart Logging Library
 * Author: Mahmoud Al-Qudsi <mqudsi@neosmart.net>
 * Copyright (C) 2012 by NeoSmart Technologies
 * This code is released under the terms of the MIT License
*/

#include "Log.h"

using namespace neosmart;
using namespace std;

namespace neosmart
{
	__thread int IndentLevel = -1;

	Logger logger;

	Logger::Logger(LogLevel logLevel)
	{
		_logLevel = logLevel;
#if defined(_WIN32) && defined(UNICODE)
		_defaultLog = &std::wcout;
#else
		_defaultLog = &std::cout;
#endif
		AddLogDestination(*_defaultLog, logLevel);
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

	void Logger::SetLogLevel(LogLevel logLevel)
	{
		const auto &defaultLogPair = _outputs.find(_defaultLog);
		if (defaultLogPair != _outputs.end())
			defaultLogPair->second = logLevel;
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
		Initialize(name);
	}

#if defined(_WIN32) && defined(UNICODE)
	void ScopeLog::Initialize(LPCSTR name)
	{
		_name = (LPCTSTR) name;
		++IndentLevel;
		logger.Log(Debug, _T("Entering %S"), _name);
	}

	ScopeLog::ScopeLog(LPCSTR name)
	{
		Initialize(name);
	}
#endif

	ScopeLog::~ScopeLog()
	{
		logger.Log(Debug, _T("Leaving %s"), _name);
		--IndentLevel;
	}
}
