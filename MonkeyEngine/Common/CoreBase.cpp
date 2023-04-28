#include "CoreBase.h"

CoreBase::~CoreBase()
{
}

CoreBase::CoreBase(int width, int height, bool fullscreen):mClientWidth(width),mClientHeight(height),mFullsccreen(fullscreen)
{
	
}

void CoreBase::InitDirect3D(HWND mHwnd)
{
	mGsx = new CoreGraphics(mClientWidth, mClientHeight, mFullsccreen);
	mGsx->InitDirect3D(mHwnd);
	OnInitialize();
}

void CoreBase::Loop()
{
	OnUpdate();
	OnRender();
	mCurrFrame = (mCurrFrame + 1) % mFrameCount;
}

CoreGraphics* CoreBase::GetCoreGraphics()
{
	return mGsx;
}

