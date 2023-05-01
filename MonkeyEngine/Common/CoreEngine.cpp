#include "CoreEngine.h"

CoreEngine::~CoreEngine()
{
}

CoreEngine::CoreEngine(int width, int height, bool fullscreen):mClientWidth(width),mClientHeight(height),mFullsccreen(fullscreen)
{
	
}

void CoreEngine::InitDirect3D(HWND mHwnd)
{
	mGsx = new CoreGraphics(mFullsccreen);
	mTimer = new CoreTimer();
	mGsx->InitDirect3D(mHwnd, mClientWidth, mClientHeight);
	OnInitialize();
}

void CoreEngine::ResetEngine()
{
	mCurrFrame = 0;
}

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CoreEngine::GetEngineGraphicsCommandList()
{
	return mGsx->mGraphicsCommandList;
}

Microsoft::WRL::ComPtr<ID3D12CommandQueue> CoreEngine::GetEngineCommandQueue()
{
	return mGsx->mCommandQueue;
}

Microsoft::WRL::ComPtr<IDXGISwapChain3> CoreEngine::GetEngineSwapChain()
{
	return mGsx->mSwapChain;
}

void CoreEngine::Loop()
{
	OnUpdate();
	OnRender();
	mCurrFrame = (mCurrFrame + 1) % mFrameCount;
}

CoreGraphics* CoreEngine::GetCoreGraphics()
{
	return mGsx;
}

CoreTimer* CoreEngine::GetCoreTimer()
{
	return mTimer;
}

