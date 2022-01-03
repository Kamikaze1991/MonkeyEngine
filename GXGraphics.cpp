#include "GXGraphics.h"

void GXGraphics::initialize()
{
}

/// <summary>
/// Descriptor Heap Build
/// </summary>
void GXGraphics::buildDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC mRtvHeapDesc = {};
	mRtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	mRtvHeapDesc.NumDescriptors = mFenceCount;
	mRtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	GXManageException(mDevice->CreateDescriptorHeap(&mRtvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())))

	D3D12_DESCRIPTOR_HEAP_DESC mDsvHeapDesc = {};
	mDsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	mDsvHeapDesc.NumDescriptors = 1;
	mDsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	GXManageException(mDevice->CreateDescriptorHeap(&mDsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())));
	
}

/// <summary>
/// Swap chain constructor
/// </summary>
void GXGraphics::buildSwapChain()
{
	DXGI_SWAP_CHAIN_DESC sc = {};
	//Buffer Description Configuration
	sc.BufferDesc.Width = mClientWidth;
	sc.BufferDesc.Height = mCLientHeight;
	sc.BufferDesc.Format = mGraphicFormat;
	sc.BufferDesc.RefreshRate.Numerator = 60;
	sc.BufferDesc.RefreshRate.Denominator = 1;
	sc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	//Sample Description Configuration
	sc.SampleDesc.Count = 1;
	sc.SampleDesc.Quality = 0;

	//General SwapChain Configs
	sc.Windowed = mWindowed;
	sc.BufferCount = mBufferCount;
	sc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sc.OutputWindow = mMainHwnd;
	sc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	GXManageException(mFactory->CreateSwapChain(mDevice.Get(), &sc, mSwapChain.GetAddressOf()));
}
