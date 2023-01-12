#ifndef _CORE_UTIL_H_
#define _CORE_UTIL_H_

#include <Windows.h>
#include <iostream>
#include <string>
#include <comdef.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl.h>
#include "d3dx12.h"
#include "CoreException.h"

inline std::wstring AnsiToWString(const std::string& str)
{
    WCHAR buffer[512];
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
    return std::wstring(buffer);
}

#ifndef ExceptionFuse
#define ExceptionFuse(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw CoreException(hr__, L#x, wfn, __LINE__); } \
}
#endif

#endif
