#include "CoreGraphics.h"

CoreGraphics::CoreGraphics(int width, int height, int windowsName)
{
}

void CoreGraphics::InitDirect3D(HINSTANCE hInstance)
{
#if defined(DEBUG)||defined(_DEBUG)
{
	Microsoft::WRL::ComPtr<ID3D12Debug> mDebug;
	ExceptionProtect(D3D12GetDebugInterface(IID_PPV_ARGS(mDebug.GetAddressOf())));
	mDebug->EnableDebugLayer();
}
#endif
}
