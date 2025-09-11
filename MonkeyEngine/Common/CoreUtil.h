#ifndef _CORE_UTIL_H_
#define _CORE_UTIL_H_

#include <Windows.h>
#include <iostream>
#include <string>
#include <comdef.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl.h>
#include <mutex>
#include "ThirdParty/d3dx12.h"
#include "CoreException.h"
#include <unordered_map>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include "ThirdParty/LunaHelpers/GeometryGenerator.h"
#include <xutility>
#include <map>
#include <d3dcompiler.h>
#include "ThirdParty/DDSTextureLoader.h"
#include <array>
using namespace Microsoft::WRL;
using namespace DirectX;
class CoreUtil {
public:
    static std::wstring AnsiToWString(const std::string& str);
    static UINT CalcConstantBufferByteSize(UINT byteSize);
    static XMFLOAT4X4 Identity4x4();
    static ComPtr<ID3D12Resource> CreateDefaultBuffer(ID3D12GraphicsCommandList* graphicsCommandList, ID3D12Device* device, UINT64 byteSize, const void* initData, ComPtr<ID3D12Resource>& intermediateBuffer);
    static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
        const std::wstring& filename,
        const D3D_SHADER_MACRO* defines,
        const std::string& entrypoint,
        const std::string& target);
};

namespace Utility
{
    inline size_t HashRange(const uint32_t* const Begin, const uint32_t* const End, size_t Hash)
    {
#if ENABLE_SSE_CRC32
        const uint64_t* Iter64 = (const uint64_t*)Math::AlignUp(Begin, 8);
        const uint64_t* const End64 = (const uint64_t* const)Math::AlignDown(End, 8);

        // If not 64-bit aligned, start with a single u32
        if ((uint32_t*)Iter64 > Begin)
            Hash = _mm_crc32_u32((uint32_t)Hash, *Begin);

        // Iterate over consecutive u64 values
        while (Iter64 < End64)
            Hash = _mm_crc32_u64((uint64_t)Hash, *Iter64++);

        // If there is a 32-bit remainder, accumulate that
        if ((uint32_t*)Iter64 < End)
            Hash = _mm_crc32_u32((uint32_t)Hash, *(uint32_t*)Iter64);
#else
        // An inexpensive hash for CPUs lacking SSE4.2
        for (const uint32_t* Iter = Begin; Iter < End; ++Iter)
            Hash = 16777619U * Hash ^ *Iter;
#endif

        return Hash;
    }

    template <typename T> inline size_t HashState(const T* StateDesc, size_t Count = 1, size_t Hash = 2166136261U)
    {
        static_assert((sizeof(T) & 3) == 0 && alignof(T) >= 4, "State object is not word-aligned");
        return HashRange((uint32_t*)StateDesc, (uint32_t*)(StateDesc + Count), Hash);
    }

} // namespace Utility

inline void AssertOrThrow(bool condition, const char* message)
{
    if (!condition)
        throw std::runtime_error(message);
}



#define MaxLights 16

#ifndef ExceptionFuse
#define ExceptionFuse(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = CoreUtil::AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw CoreException(hr__, L#x, wfn, __LINE__); } \
}
#endif

#endif
