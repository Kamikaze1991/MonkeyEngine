#include "CoreEngine.h"

CoreEngine::~CoreEngine()
{
}

CoreEngine::CoreEngine(int width, int height, bool fullscreen):mClientWidth(width),mClientHeight(height),mFullsccreen(fullscreen)
{
	
}

void CoreEngine::InitDirect3D(HWND mHwnd)
{
	mCoreGraphics = new CoreGraphics();
	mTimer = new CoreTimer();
	mCoreGraphics->InitDirect3D(mHwnd, mClientWidth, mClientHeight);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO(); (void)io;
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(mHwnd);
	ImGui_ImplDX12_Init(mCoreGraphics->mDevice.Get(), 3,
		DXGI_FORMAT_R8G8B8A8_UNORM, mCoreGraphics->mCbvSrvUavHeap.Get(),
		mCoreGraphics->mCbvSrvUavHeap->GetCPUDescriptorHandleForHeapStart(),
		mCoreGraphics->mCbvSrvUavHeap->GetGPUDescriptorHandleForHeapStart());


	OnInitialize();
}

void CoreEngine::ResetEngine()
{
	mCoreGraphics->OnReset(mClientWidth, mClientHeight);
	mCurrFrame = 0;
}

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CoreEngine::GetEngineGraphicsCommandList()
{
	return mCoreGraphics->mGraphicsCommandList;
}

Microsoft::WRL::ComPtr<ID3D12CommandQueue> CoreEngine::GetEngineCommandQueue()
{
	return mCoreGraphics->mCommandQueue;
}

Microsoft::WRL::ComPtr<IDXGISwapChain3> CoreEngine::GetEngineSwapChain()
{
	return mCoreGraphics->mSwapChain;
}

void CoreEngine::Loop()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::SetIO(io);
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!x");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);
		ImGui::End();
	}

	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}
	ImGui::Render();
	//ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pd3dCommandList);


	OnUpdate();
	OnRender();
	mCurrFrame = (mCurrFrame + 1) % mFrameCount;
}

CoreGraphics* CoreEngine::GetCoreGraphics()
{
	return mCoreGraphics;
}

CoreTimer* CoreEngine::GetCoreTimer()
{
	return mTimer;
}

void CoreEngine::WindowRedimention(int width, int height)
{
	mClientWidth = width;
	mClientHeight = height;
}

