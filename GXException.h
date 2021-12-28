#ifndef _GXEXCEPTION_H_
#define _GXEXCEPTION_H_
#include "GXTools.h"
#include <comdef.h>
class GXException {
private:
public:
	GXException() = default;
	GXException(HRESULT hr, const std::wstring & functionName, const std::wstring & fileName, int lineNumber);
	
	std::wstring toString()const;

	HRESULT mErrorCode = S_OK;
	std::wstring mFunctionName;
	std::wstring mFileName;
	int mLineNumber=0;
};


#endif
