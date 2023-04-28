#ifndef _CORE_ENGINE_H_
#define _CORE_ENGINE_H_
#include "CoreUtil.h"
#include "CoreGraphics.h"

class CoreBase {
protected:

	CoreGraphics* mGsx = nullptr;
	int mCurrFrame = 0;
	int mFrameCount = 2;
	bool mFullsccreen = true;
public:
	int mClientWidth = 800;
	int mClientHeight = 600;

	CoreBase() = default;
	~CoreBase();
	CoreBase(int width, int height, bool fullscreen);
	void InitDirect3D(HWND mHwnd);
	virtual void Loop();
	virtual void OnInitialize()=0;
	virtual void OnUpdate()=0;
	virtual void OnRender()=0;

	CoreGraphics* GetCoreGraphics();
};
#endif
