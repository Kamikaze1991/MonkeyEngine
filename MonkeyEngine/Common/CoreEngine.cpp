#include "CoreEngine.h"

CoreEngine::~CoreEngine()
{
}

CoreEngine::CoreEngine(int width, int height, bool fullscreen):ClientWidth(width),ClientHeight(height),FullScreen(fullscreen)
{
	MainHwnd = nullptr;
}

void CoreEngine::InitDirect3D(HWND mHwnd)
{
	mCoreGraphics = new CoreGraphics();
	mTimer = new CoreTimer();
	mCoreGraphics->InitDirect3D(mHwnd, ClientWidth, ClientHeight);
	MainHwnd = mHwnd;
	OnInitialize();
}

void CoreEngine::ResetEngine()
{
	mCoreGraphics->OnReset(ClientWidth, ClientHeight);
	CurrFrame = 0;
}

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CoreEngine::GetEngineGraphicsCommandList()
{
	return mCoreGraphics->GraphicsCommandListControl;
}

Microsoft::WRL::ComPtr<ID3D12CommandQueue> CoreEngine::GetEngineCommandQueue()
{
	return mCoreGraphics->CommandQueueControl;
}

Microsoft::WRL::ComPtr<IDXGISwapChain3> CoreEngine::GetEngineSwapChain()
{
	return mCoreGraphics->SwapChainControl;
}

void CoreEngine::Loop(const CoreTimer& gt)
{
	OnInitializeUi();
	OnUpdate(gt);
	OnRender();
	CurrFrame = (CurrFrame + 1) % FrameCount;
}

CoreGraphics* CoreEngine::GetCoreGraphics()
{
	return mCoreGraphics;
}

CoreTimer& CoreEngine::GetCoreTimer()
{
	return *mTimer;
}

void CoreEngine::WindowRedimention(int width, int height)
{
	ClientWidth = width;
	ClientHeight = height;
}

