//*********************************************************
//
// Copyright (c) Rick Laboratories. All rights reserved.
// This code is licensed under the MIT License (MIT).
// Autor: Luis Borrero
//
//*********************************************************

#include "CoreException.h"

/// <summary>
/// exception incialization
/// we need error handle, function name, filename, and error line
/// </summary>
/// <param name="hr">error handle</param>
/// <param name="functionName">function name</param>
/// <param name="fileName">file name</param>
/// <param name="lineNumber">line number</param>
CoreException::CoreException(HRESULT hr, const std::wstring& functionName, const std::wstring& fileName, int lineNumber):
	mFunctionName(functionName), 
	mFileName(fileName),
	mLineNumber(lineNumber),
	mErrorCode(hr){}

std::wstring CoreException::ProcessMessage() const
{
	_com_error error(mErrorCode);

	
	const std::wstring errorResponse =	L"FUNCTION NAME	: " + mFunctionName + L"\n" +
										L"FILE NAME		: " + mFileName + L"\n" +
										L"LINE NUMBER	: " + std::to_wstring(mLineNumber) + L"\n" +
										L"ERROR MESSAGE	: " + error.ErrorMessage() + L"\n";

	HANDLE event_log = RegisterEventSource(NULL, L"MONKEY_LOG_SYSTEM");
	LPCWSTR message =errorResponse.c_str();
	ReportEvent(event_log, EVENTLOG_SUCCESS, 0, 0, NULL, 1, 0, &message, NULL);

	return errorResponse;
}


