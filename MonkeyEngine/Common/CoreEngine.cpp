#include "CoreEngine.h"

CoreEngine::~CoreEngine()
{
	// Libera recursos propios
	mTimer.reset();         // si fue inicializado
	mCoreGraphics.reset();  // se libera automáticamente, pero puedes hacerlo explícito

	// ImGui cleanup (si tú lo inicializaste)
	ImGui_ImplDX12_Shutdown();
	ImGui::DestroyContext();

	io = nullptr;
}

CoreEngine::CoreEngine(int width, int height, bool fullscreen):ClientWidth(width),ClientHeight(height),FullScreen(fullscreen)
{
	MainHwnd = nullptr;
}

void CoreEngine::InitDirect3D(HWND mHwnd)
{
	mTimer = std::make_unique<CoreTimer>();
	mCoreGraphics->InitDirect3D(mHwnd, ClientWidth, ClientHeight);
	MainHwnd = mHwnd;
	OnInitialize();
}

void CoreEngine::ResetEngine()
{
	mCoreGraphics->OnReset(ClientWidth, ClientHeight);
}

void CoreEngine::Loop(const CoreTimer& gt)
{
	OnInitializeUi();
	OnUpdate(gt);
	OnRender();
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

