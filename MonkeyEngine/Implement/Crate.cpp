#include "Crate.h"

void Crate::OnUpdate()
{
	CurrentFrameResourceIndex = (CurrentFrameResourceIndex + 1) % 3;
	CurrFrameResource = FrameResources[CurrentFrameResourceIndex].get();
	

	if (CurrFrameResource->mFrameFenceCount != 0 && mCoreGraphics->FenceControl->GetCompletedValue() < CurrFrameResource->mFrameFenceCount)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
		ExceptionFuse(mCoreGraphics->FenceControl->SetEventOnCompletion(CurrFrameResource->mFrameFenceCount, eventHandle));
		if (eventHandle) {
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}

	//mCoreGraphics->FlushCommandQueue(CurrFrameResource->mFrameFenceCount);
}

void Crate::OnRender()
{
	PopulateCommands();
}

void Crate::PopulateCommands()
{
	auto mCurrCmd = CurrFrameResource->mFrameCommandAllocator;
	mCurrCmd->Reset();
	GetEngineGraphicsCommandList()->Reset(mCurrCmd.Get(), nullptr);
	D3D12_RESOURCE_BARRIER rbInitial = CD3DX12_RESOURCE_BARRIER::Transition(mCoreGraphics->RenderTargetBuffer[CurrFrame].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	GetEngineGraphicsCommandList()->ResourceBarrier(1, &rbInitial);
	CD3DX12_CPU_DESCRIPTOR_HANDLE descriptor(mCoreGraphics->RenderTargetViewHeap->GetCPUDescriptorHandleForHeapStart(), CurrFrame, mCoreGraphics->RenderTargetViewHeapSize);
	float clearColor[] = { ClearColor.x/ ClearColor.w,ClearColor.y / ClearColor.w, ClearColor.z / ClearColor.w, ClearColor.w };
	GetEngineGraphicsCommandList()->ClearRenderTargetView(descriptor, clearColor, 0, nullptr);
	GetEngineGraphicsCommandList()->ClearDepthStencilView(mCoreGraphics->DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	D3D12_CPU_DESCRIPTOR_HANDLE currBackBuffer = mCoreGraphics->CurrentBackBufferView(CurrFrame);
	D3D12_CPU_DESCRIPTOR_HANDLE currDepthStencil = mCoreGraphics->DepthStencilView();
	GetEngineGraphicsCommandList()->OMSetRenderTargets(1, &currBackBuffer, true, &currDepthStencil);

	ID3D12DescriptorHeap* descriptorHeaps[] = { ShadowResourceViewDescriptorHeap.Get() };
	GetEngineGraphicsCommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	//GetEngineGraphicsCommandList()->SetDescriptorHeaps(1, &mCoreGraphics->mCbvSrvUavHeap);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), GetEngineGraphicsCommandList().Get());
	rbInitial = CD3DX12_RESOURCE_BARRIER::Transition(mCoreGraphics->RenderTargetBuffer[CurrFrame].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	GetEngineGraphicsCommandList()->ResourceBarrier(1, &rbInitial);
	GetEngineGraphicsCommandList()->Close();

	ID3D12CommandList* mList[] = { GetEngineGraphicsCommandList().Get() };
	GetEngineCommandQueue()->ExecuteCommandLists(_countof(mList), mList);
	GetEngineSwapChain()->Present(1, 0);
	CurrFrameResource->mFrameFenceCount = ++mCoreGraphics->FenceControlCount;

	GetEngineCommandQueue()->Signal(mCoreGraphics->FenceControl.Get(), mCoreGraphics->FenceControlCount);

	//mCoreGraphics->FlushCommandQueue(CurrFrameResource->mFrameFenceCount);
}

Crate::~Crate()
{
		
}

Crate::Crate(int among):CoreEngine()
{
	
}

void Crate::OnInitialize()
{
	BuildFrameResurces();
	BuildLocalDescriptorHeap();
	BuilduserInterface();
	mCoreGraphics->FlushCommandQueue();
}

void Crate::OnInitializeUi()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	if (ShowDemoWindow)
		ImGui::ShowDemoWindow(&ShowDemoWindow);

	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!x");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &ShowDemoWindow);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &ShowAnotherWindow);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&ClearColor); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);
		ImGui::End();
	}

	if (ShowAnotherWindow)
	{
		ImGui::Begin("Another Window", &ShowAnotherWindow);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			ShowAnotherWindow = false;
		ImGui::End();
	}
	ImGui::Render();
	//ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pd3dCommandList);

}

void Crate::BuildFrameResurces()
{
	for (int i = 0; i < 3; ++i)
	{
		FrameResources.push_back(std::make_unique<FrameResource>(mCoreGraphics->DeviceControl.Get()));
	}
}

void Crate::BuildLocalDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC mSrvDesc = {};
	mSrvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	mSrvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	mSrvDesc.NumDescriptors = 1;
	ExceptionFuse(mCoreGraphics->DeviceControl->CreateDescriptorHeap(&mSrvDesc, IID_PPV_ARGS(ShadowResourceViewDescriptorHeap.GetAddressOf())));
}

void Crate::BuilduserInterface()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO(); (void)io;
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(MainHwnd);
	ImGui_ImplDX12_Init(mCoreGraphics->DeviceControl.Get(), 3,
		DXGI_FORMAT_R8G8B8A8_UNORM, ShadowResourceViewDescriptorHeap.Get(),
		ShadowResourceViewDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		ShadowResourceViewDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
}
