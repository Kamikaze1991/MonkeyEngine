#ifndef _CORE_ENGINE_H_
#define _CORE_ENGINE_H_
#include "../Common/ThirdParty/imgui/imgui.h"
#include "../Common/ThirdParty/imguibackends/imgui_impl_win32.h"
#include "../Common/ThirdParty/imguibackends/imgui_impl_dx12.h"
#include "CoreUtil.h"
#include "CoreGraphics.h"
#include "CoreTimer.h"

class CoreEngine {
protected:
	std::unique_ptr<ICoreGraphics> mCoreGraphics = std::make_unique<CoreGraphics>();
	std::unique_ptr<CoreTimer> mTimer = nullptr;
	ImGuiIO *io=nullptr;
	HWND MainHwnd;
	int FrameCount = 2;
	bool FullScreen = true;
#pragma region variables imgui
	ImVec4 ClearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	bool ShowDemoWindow = true;
	bool ShowAnotherWindow = false;
#pragma endregion

	
public:
	int ClientWidth = 800;
	int ClientHeight = 600;

	CoreEngine() = default;
	~CoreEngine();
	CoreEngine(int width, int height, bool fullscreen);
	virtual void InitDirect3D(HWND mHwnd);
	void ResetEngine();
	virtual void Loop(const CoreTimer& gt);
	virtual void OnInitialize()=0;
	virtual void OnInitializeUi() = 0;
	virtual void OnUpdate(const CoreTimer& gt) =0;
	virtual void OnRender()=0;

	//engine helpers

	CoreTimer& GetCoreTimer();

	void WindowRedimention(int width, int height);
};
#endif
