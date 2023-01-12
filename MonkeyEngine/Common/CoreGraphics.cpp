#include "CoreGraphics.h"

CoreGraphics::~CoreGraphics()
{
	CloseHandle(mEventHandle);
}

CoreGraphics::CoreGraphics(int width, int height, bool fullscreen):mClientWidth(width),mClientHeight(height),mFullsccreen(fullscreen)
{
	mEventHandle = {};
	mScissorRect = {};
	mViewPort = {};
}

void CoreGraphics::InitDirect3D(HWND mHwnd)
{
#if defined(DEBUG)||defined(_DEBUG)
{
	Microsoft::WRL::ComPtr<ID3D12Debug> mDebug;
	ExceptionFuse(D3D12GetDebugInterface(IID_PPV_ARGS(mDebug.GetAddressOf())));
	mDebug->EnableDebugLayer();
}
#endif
	ExceptionFuse(CreateDXGIFactory2(0, IID_PPV_ARGS(mFactory.GetAddressOf())));

	HRESULT hrCreateDevice = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(mDevice.GetAddressOf()));
	if (FAILED(hrCreateDevice)) {
		Microsoft::WRL::ComPtr<IDXGIAdapter> localAdapter;
		ExceptionFuse(mFactory->EnumWarpAdapter(IID_PPV_ARGS(localAdapter.GetAddressOf())));
		ExceptionFuse(D3D12CreateDevice(localAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(mDevice.GetAddressOf())));
	}

	mDsvHeapSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	mRtvHeapSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mCbvSrvUavheapSize= mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(mFence.GetAddressOf()));

	mEventHandle = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);

	InitCommandObjects();
	InitSwapChain(mHwnd);
	InitDescriptorHeaps();



}

void CoreGraphics::FlushCommandQueue()
{
	mFenceCount++;
	ExceptionFuse(mFence->Signal(mFenceCount));
	if (mFence->GetCompletedValue() < mFenceCount) {
		mFence->SetEventOnCompletion(mFenceCount, mEventHandle);
		WaitForSingleObject(mEventHandle, INFINITE);
	}
}

/// <summary>
/// Create Commands Objects
/// </summary>
void CoreGraphics::InitCommandObjects()
{
	ExceptionFuse(mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(mCommandAllocator.GetAddressOf())));
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	ExceptionFuse(mDevice->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(mCommandQueue.GetAddressOf())));
	ExceptionFuse(mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), nullptr, IID_PPV_ARGS(mGraphicsCommandList.GetAddressOf())));
	mGraphicsCommandList->Close();
}

/// <summary>
/// Swap chain initialization
/// </summary>
/// <param name="mHwnd">Windows handlet</param>
void CoreGraphics::InitSwapChain(HWND mHwnd)
{
	mSwapChain.Reset();
	Microsoft::WRL::ComPtr<IDXGISwapChain1> localSwapChain;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = mFrameCount;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.Format = mSwapChainFormat;
	swapChainDesc.Height = mClientHeight;
	swapChainDesc.Width = mClientWidth;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	ExceptionFuse(mFactory->CreateSwapChainForHwnd(mCommandQueue.Get(), mHwnd, &swapChainDesc, nullptr, nullptr, &localSwapChain));
	ExceptionFuse(localSwapChain.As(&mSwapChain));
}


/// <summary>
/// initialize basic descriptor heaps
/// </summary>
void CoreGraphics::InitDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.NumDescriptors = mFrameCount;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	ExceptionFuse(mDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())));
	ExceptionFuse(mDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())));
	
}
