#ifndef _CORE_ENGINE_H_
#define _CORE_ENGINE_H_
#include "CoreUtil.h"
#include "CoreGraphics.h"
#include "CoreTimer.h"

class CoreEngine {
protected:

	CoreGraphics* mGsx = nullptr;
	CoreTimer* mTimer = nullptr;
	int mCurrFrame = 0;
	int mFrameCount = 2;
	bool mFullsccreen = true;
public:
	int mClientWidth = 800;
	int mClientHeight = 600;

	CoreEngine() = default;
	~CoreEngine();
	CoreEngine(int width, int height, bool fullscreen);
	void InitDirect3D(HWND mHwnd);
	virtual void Loop();
	virtual void OnInitialize()=0;
	virtual void OnUpdate()=0;
	virtual void OnRender()=0;

	CoreGraphics* GetCoreGraphics();
	CoreTimer* GetCoreTimer();
};
#endif