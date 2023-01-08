//*********************************************************
//
// Copyright (c) Rick Laboratories. All rights reserved.
// This code is licensed under the MIT License (MIT).
// Autor: Luis Borrero
//
//*********************************************************

#ifndef _CORE_EXCEPTION_H
#define _CORE_EXCEPTION_H

#include "CoreUtil.h"

class CoreException {
private:
public:
	CoreException() = default;
	CoreException(HRESULT hr, const std::wstring& functionName, const std::wstring& fileName, int lineNumber);
	std::wstring ProcessMessage()const;
public:
	HRESULT mErrorCode = S_OK;
	std::wstring mFunctionName;
	std::wstring mFileName;
	int mLineNumber=-1;
};


#endif