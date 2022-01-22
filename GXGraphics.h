#ifndef _GXGRAPHICS_H_
#define _GXGRAPHICS_H_
#include "GXTools.h"

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")


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
	UINT mRtvHeapSize = 0;
	UINT mDsvHeapSize = 0;
	UINT mCbvUavSrvSize = 0;

	/// <summary>
	/// Resources
	/// </summary>
	static const int mBufferCount = 2;
	int mCurrBackBuffer=0;
	ComPtr<ID3D12Resource> mSwapChainBuffer[mBufferCount];
	ComPtr<ID3D12Resource> mDepthStencilBuffer;

	/// <summary>
	/// Some important structures
	/// </summary>
	int mClientWidth=800;
	int mCLientHeight=600;
	DXGI_FORMAT mGraphicFormat = DXGI_FORMAT_R8G8B8A8_SNORM;
	DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	bool mWindowed = true;
	HWND mMainHwnd = 0;
	D3D12_VIEWPORT mViewPort;
	D3D12_RECT mScissorRect;

public:
	GXGraphics(HWND hwnd, bool windowed, UINT width, UINT height);
	void initialize();
	void buildDescriptorHeaps();
	void buildSwapChain();
	void buildCommandObjects();
	

	//log control
	void graphicsLog();
	void logAdapterOutputs(IDXGIAdapter* _adapter);
	void logOutputDisplayMode(IDXGIOutput* _output, DXGI_FORMAT _format);

	//magic actions
	void flushQueue();
	void rebuildTarget();



	//configs
	void setScreenSize(UINT width, UINT height);
	UINT getWidth();
	UINT getHeight();
	ComPtr<ID3D12GraphicsCommandList> getCommandList();
	ComPtr<ID3D12Device> getDevice();

};
#endif