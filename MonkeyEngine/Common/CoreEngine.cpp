#include "CoreEngine.h"

CoreEngine::~CoreEngine()
{
}

CoreEngine::CoreEngine(int width, int height, bool fullscreen):mClientWidth(width),mClientHeight(height),mFullsccreen(fullscreen)
{
	
}

void CoreEngine::InitDirect3D(HWND mHwnd)
{
	mGsx = new CoreGraphics(mClientWidth, mClientHeight, mFullsccreen);
	mGsx->InitDirect3D(mHwnd);
	OnInitialize();
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

