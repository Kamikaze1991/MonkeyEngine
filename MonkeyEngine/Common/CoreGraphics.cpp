#include "CoreGraphics.h"

CoreGraphics::~CoreGraphics()
{
	FlushCommandQueue();

}

CoreGraphics::CoreGraphics()
{
	ScissorRect = {};
	ViewPort = {};
}

void CoreGraphics::InitDirect3D(HWND mHwnd, int clientWidth, int clientHeight)
{
#if defined(DEBUG)||defined(_DEBUG)
	{
		Microsoft::WRL::ComPtr<ID3D12Debug> mDebug;
		ExceptionFuse(D3D12GetDebugInterface(IID_PPV_ARGS(mDebug.GetAddressOf())));
		mDebug->EnableDebugLayer();
	}
#endif
	ExceptionFuse(CreateDXGIFactory2(0, IID_PPV_ARGS(FactoryControl.GetAddressOf())));

	HRESULT hrCreateDevice = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(DeviceControl.GetAddressOf()));
	if (FAILED(hrCreateDevice)) {
		Microsoft::WRL::ComPtr<IDXGIAdapter> localAdapter;
		ExceptionFuse(FactoryControl->EnumWarpAdapter(IID_PPV_ARGS(localAdapter.GetAddressOf())));
		ExceptionFuse(D3D12CreateDevice(localAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(DeviceControl.GetAddressOf())));
	}

	DepthStencilViewHeapSize = DeviceControl->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	RenderTargetViewHeapSize = DeviceControl->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mCbvSrvUavheapSize = DeviceControl->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	DeviceControl->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(FenceControl.GetAddressOf()));


	BuildCommandObjects();
	BuildSwapChain(mHwnd,clientWidth,clientHeight);
	BuildMainDescriptorHeaps();

	OnReset(clientWidth,clientHeight);
}



void CoreGraphics::FlushCommandQueue()
{
	FenceControlCount++;
	CommandQueueControl->Signal(FenceControl.Get(), FenceControlCount);
	//ExceptionFuse(CommandQueueControl->Signal(FenceControlCount));

	if (FenceControl->GetCompletedValue() < FenceControlCount) {
		HANDLE eventHandle = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
		if (eventHandle == nullptr)
			return;
		FenceControl->SetEventOnCompletion(FenceControlCount, eventHandle);
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}

void CoreGraphics::FlushCommandQueue(UINT64 fenceValue)
{
	if (fenceValue != 0 && FenceControl->GetCompletedValue() < fenceValue) {
		HANDLE handleLocal = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
		if (!handleLocal)return;
		FenceControl->SetEventOnCompletion(fenceValue, handleLocal);
		WaitForSingleObject(handleLocal, INFINITE);
		CloseHandle(handleLocal);
	}
}

int CoreGraphics::GetCurrentBufferIndex()const
{
	return SwapChainControl->GetCurrentBackBufferIndex();
}

DXGI_FORMAT CoreGraphics::GetBackBufferFormat() const
{
	return mBackBufferFormat;
}

DXGI_FORMAT CoreGraphics::GetDepthStencilFormat() const
{
	return mDepthStencilFormat;
}

int CoreGraphics::GetMsaaState() const
{
	return m4xMsaaState;
}

int CoreGraphics::GetMsaaQuality() const
{
	return m4xMsaaQuality;
}

void CoreGraphics::BeginScene(ID3D12CommandAllocator* commandAllocator, ID3D12PipelineState* pipelineState, const float* clearColor) const
{
	commandAllocator->Reset();
	GraphicsCommandListControl->Reset(commandAllocator, pipelineState);

	GraphicsCommandListControl->RSSetViewports(1, &ViewPort);
	GraphicsCommandListControl->RSSetScissorRects(1, &ScissorRect);
	D3D12_RESOURCE_BARRIER rbInitial = CD3DX12_RESOURCE_BARRIER::Transition(GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	GraphicsCommandListControl->ResourceBarrier(1, &rbInitial);
	GraphicsCommandListControl->ClearRenderTargetView(CurrentBackBufferView(), clearColor, 0, nullptr);
	GraphicsCommandListControl->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	D3D12_CPU_DESCRIPTOR_HANDLE currBackBuffer = CurrentBackBufferView();
	D3D12_CPU_DESCRIPTOR_HANDLE currDepthStencil = DepthStencilView();
	GraphicsCommandListControl->OMSetRenderTargets(1, &currBackBuffer, true, &currDepthStencil);
}

int CoreGraphics::EndScene()
{
	D3D12_RESOURCE_BARRIER rbInitial = CD3DX12_RESOURCE_BARRIER::Transition(GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	GraphicsCommandListControl->ResourceBarrier(1, &rbInitial);
	GraphicsCommandListControl->Close();

	ID3D12CommandList* mList[] = { GraphicsCommandListControl.Get()};
	CommandQueueControl->ExecuteCommandLists(_countof(mList), mList);
	SwapChainControl->Present(1, 0);
	
	++FenceControlCount;
	CommandQueueControl->Signal(FenceControl.Get(), FenceControlCount);
	return FenceControlCount;
}

void CoreGraphics::OnReset(int clientWidth, int clientHeight)
{
	if (!DeviceControl)
		return;
	FlushCommandQueue();
	//	CommandAllocatorControl->Reset();
	ExceptionFuse(GraphicsCommandListControl->Reset(CommandAllocatorControl.Get(), nullptr));
	//reset buffers
	for (int i = 0; i < FrameCount; i++)
		BackBuffer[i].Reset();
	DepthStencilBuffer.Reset();
	//resize swap chain
	DXGI_SWAP_CHAIN_DESC sd = {};
	SwapChainControl->GetDesc(&sd);
	ExceptionFuse(SwapChainControl->ResizeBuffers(FrameCount, clientWidth, clientHeight, SwapChainControlFormat, sd.Flags));
	//rebuild buffers
	CD3DX12_CPU_DESCRIPTOR_HANDLE mRtvHandle(RenderTargetViewHeap->GetCPUDescriptorHandleForHeapStart());
	for (int i = 0; i < FrameCount; i++) {
		ExceptionFuse(SwapChainControl->GetBuffer(i, IID_PPV_ARGS(BackBuffer[i].GetAddressOf())));
		DeviceControl->CreateRenderTargetView(BackBuffer[i].Get(), nullptr, mRtvHandle);
		mRtvHandle.Offset(1, RenderTargetViewHeapSize);
	}

	D3D12_CLEAR_VALUE clearValue;
	clearValue.DepthStencil.Depth = 1.0f;
	clearValue.DepthStencil.Stencil = 0;
	clearValue.Format = mDepthStencilFormat;
	D3D12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC mDsvDesc = CD3DX12_RESOURCE_DESC::Tex2D(mDepthStencilFormat, clientWidth, clientHeight, 1, 0);
	mDsvDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	DeviceControl->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&mDsvDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clearValue,
		IID_PPV_ARGS(DepthStencilBuffer.GetAddressOf()));

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvd = {};
	dsvd.Flags = D3D12_DSV_FLAG_NONE;
	dsvd.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
	dsvd.Format = mDepthStencilFormat;
	DeviceControl->CreateDepthStencilView(DepthStencilBuffer.Get(), &dsvd, DepthStencilView());

	GraphicsCommandListControl->Close();
	ID3D12CommandList* cmdList[] = { GraphicsCommandListControl.Get() };
	CommandQueueControl->ExecuteCommandLists(_countof(cmdList), cmdList);
	FlushCommandQueue();
	ViewPort.Height = static_cast<float>(clientHeight);
	ViewPort.Width = static_cast<float>(clientWidth);
	ViewPort.MaxDepth = 1.0f;
	ViewPort.MinDepth = 0.0f;
	ViewPort.TopLeftX = 0;
	ViewPort.TopLeftY = 0;

	ScissorRect = { 0,0,clientWidth,clientHeight };

}

D3D12_CPU_DESCRIPTOR_HANDLE CoreGraphics::CurrentBackBufferView() const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(RenderTargetViewHeap->GetCPUDescriptorHandleForHeapStart(), SwapChainControl->GetCurrentBackBufferIndex(), RenderTargetViewHeapSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE CoreGraphics::DepthStencilView() const
{
	return DepthStencilViewHeap->GetCPUDescriptorHandleForHeapStart();
}

ID3D12GraphicsCommandList* CoreGraphics::GetGraphicsCommandList() const
{
	return GraphicsCommandListControl.Get();
}

ID3D12CommandAllocator* CoreGraphics::GetEngineCommandAllocator() const
{
	return CommandAllocatorControl.Get();
}

ID3D12CommandQueue* CoreGraphics::GetEngineCommandQueue() const
{
	return CommandQueueControl.Get();
}

IDXGISwapChain3* CoreGraphics::GetEngineSwapChain() const
{
	return SwapChainControl.Get();
}

ID3D12Resource* CoreGraphics::GetCurrentBackBuffer() const
{
	return BackBuffer[SwapChainControl->GetCurrentBackBufferIndex()].Get();
}

ID3D12Resource* CoreGraphics::GetDepthStencilBuffer() const
{
	return nullptr;
}

ID3D12Device* CoreGraphics::GetDeviceControl() const
{
	return DeviceControl.Get();
}

/// <summary>
/// Create Commands Objects
/// </summary>
void CoreGraphics::BuildCommandObjects()
{
	ExceptionFuse(DeviceControl->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(CommandAllocatorControl.GetAddressOf())));
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	ExceptionFuse(DeviceControl->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(CommandQueueControl.GetAddressOf())));
	ExceptionFuse(DeviceControl->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocatorControl.Get(), nullptr, IID_PPV_ARGS(GraphicsCommandListControl.GetAddressOf())));
	GraphicsCommandListControl->Close();
}

/// <summary>
/// Swap chain initialization
/// </summary>
/// <param name="mHwnd">Windows handlet</param>
void CoreGraphics::BuildSwapChain(HWND mHwnd, int clientWidth, int clientHeight)
{
	SwapChainControl.Reset();
	Microsoft::WRL::ComPtr<IDXGISwapChain1> localSwapChain;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = FrameCount;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.Format = SwapChainControlFormat;
	swapChainDesc.Height = clientHeight;
	swapChainDesc.Width = clientWidth;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	ExceptionFuse(FactoryControl->CreateSwapChainForHwnd(CommandQueueControl.Get(), mHwnd, &swapChainDesc, nullptr, nullptr, &localSwapChain));
	ExceptionFuse(localSwapChain.As(&SwapChainControl));
}


/// <summary>
/// initialize basic descriptor heaps
/// </summary>
void CoreGraphics::BuildMainDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.NumDescriptors = FrameCount;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	ExceptionFuse(DeviceControl->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(RenderTargetViewHeap.GetAddressOf())));
	ExceptionFuse(DeviceControl->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(DepthStencilViewHeap.GetAddressOf())));
}
