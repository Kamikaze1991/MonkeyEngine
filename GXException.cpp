#include "GXException.h"

/// <summary>
/// Constructor
/// </summary>
/// <param name="hr">HRESULT</param>
/// <param name="functionName"> name of the function</param>
/// <param name="fileName">name of the file</param>
/// <param name="lineNumber">line error code</param>
GXException::GXException(HRESULT hr, const std::wstring& functionName, const std::wstring& fileName, int lineNumber)
{
	mErrorCode = hr;
	mFunctionName = functionName;
	mFileName = fileName;
	mLineNumber = lineNumber;
}

/// <summary>
/// Transform to custom error string
/// </summary>
/// <returns>custom error string</returns>
std::wstring GXException::toString() const
{
	_com_error err(mErrorCode);
	std::wstring msg = err.ErrorMessage();
	return mFunctionName + L" failed in " + mFileName + L"; line " + std::to_wstring(mLineNumber) + L"; error: " + msg;
}
