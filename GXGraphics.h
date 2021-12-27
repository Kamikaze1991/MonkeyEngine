#ifndef _GXGRAPHICS_H_
#define _GXGRAPHICS_H_
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl.h>
using namespace Microsoft::WRL;
class GXGraphics {
private:
	/// <summary>
	/// Graphics Primals
	/// </summary>
	ComPtr<ID3D12Device> mDevice;
	ComPtr<IDXGIFactory4> mFactory;
	ComPtr<IDXGISwapChain> mSwapChain;

	/// <summary>
	/// Fence and Sync
	/// </summary>
	UINT64 mFenceCount = 0;
	ComPtr<ID3D12Fence> mFence;

	/// <summary>
	/// Commands Stacks Control
	/// </summary>
	ComPtr<ID3D12CommandQueue> mCommandQueue;
	ComPtr<ID3D12GraphicsCommandList> mCommandList;
	ComPtr<ID3D12CommandAllocator> mCommandAllocator;

	/// <summary>
	/// Heaps Engine
	/// </summary>
	ComPtr<ID3D12DescriptorHeap> mRtvHeap;
	ComPtr<ID3D12DescriptorHeap> mDsvHeap;
	UINT mRtvHeapSiz = 0;
	UINT mDsvHeapSize = 0;
	UINT mCbvUavSrv = 0;

	/// <summary>
	/// Resources
	/// </summary>
	static const int mBufferCount = 2;
	int mCurrFrame=0;
	ComPtr<ID3D12Resource> mSwapChainBuffer[mBufferCount];
	ComPtr<ID3D12Resource> mDepthStencilBuffer;

public:
	void initialize();
};
#endif