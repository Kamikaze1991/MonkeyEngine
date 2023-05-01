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
	virtual void InitDirect3D(HWND mHwnd);
	void ResetEngine();
	virtual void Loop();
	virtual void OnInitialize()=0;
	virtual void OnUpdate()=0;
	virtual void OnRender()=0;

	//engine helpers
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetEngineGraphicsCommandList();
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetEngineCommandQueue();
	Microsoft::WRL::ComPtr<IDXGISwapChain3> GetEngineSwapChain();

	CoreGraphics* GetCoreGraphics();
	CoreTimer* GetCoreTimer();

	void WindowRedimention(int width, int height);
};
#endif
