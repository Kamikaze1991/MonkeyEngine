#ifndef _GXTOOLS_H_
#define _GXTOOLS_H_
#include <Windows.h>
#include <d3d12.h>
#include "d3dx12.h"
#include <dxgi1_4.h>
#include <wrl.h>
#include <string>
#include <vector>
#include "GXException.h"

/// <summary>
/// function to convert ansi to string
/// </summary>
/// <param name="str">string input</param>
/// <returns>wstring output</returns>
inline std::wstring AnsiToWString(const std::string& str)
{
    WCHAR buffer[512];
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
    return std::wstring(buffer);
}

#define GXManageException(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw GXException(hr__, L#x, wfn, __LINE__); } \
}

#endif

#ifndef ReleaseCom
#define ReleaseCom(x) { if(x){ x->Release(); x = 0; } }
#endif


