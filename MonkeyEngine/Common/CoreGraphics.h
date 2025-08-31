#ifndef _CORE_GRAPHICS_H_
#define _CORE_GRAPHICS_H_
#include "CoreUtil.h"


class CoreGraphics {
public:
#pragma region Directx Main Variables
	Microsoft::WRL::ComPtr<ID3D12Device> DeviceControl;
	Microsoft::WRL::ComPtr<IDXGIFactory4> FactoryControl;
	Microsoft::WRL::ComPtr<IDXGISwapChain3> SwapChainControl;

	Microsoft::WRL::ComPtr<ID3D12Fence> FenceControl;
	UINT FenceControlCount = 0;

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GraphicsCommandListControl;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> CommandQueueControl;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CommandAllocatorControl;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> RenderTargetViewHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DepthStencilViewHeap;
	UINT RenderTargetViewHeapSize = 0;
	UINT DepthStencilViewHeapSize = 0;
	UINT mCbvSrvUavheapSize = 0;

	static const int FrameCount = 2;
	Microsoft::WRL::ComPtr<ID3D12Resource> RenderTargetBuffer[FrameCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> DepthStencilBuffer;

	D3D12_VIEWPORT ViewPort;
	D3D12_RECT ScissorRect;


	DXGI_FORMAT SwapChainControlFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
#pragma endregion
	bool FullScreen = true;
	// Set true to use 4X MSAA (§4.1.8).  The default is false.
	bool      m4xMsaaState = false;    // 4X MSAA enabled
	UINT      m4xMsaaQuality = 0;      // quality level of 4X MSAA

public:
	~CoreGraphics();
	CoreGraphics();
	void InitDirect3D(HWND mHwnd, int clientWidth, int clientHeight);
	void OnReset(int clientWidth, int clientHeight);

	void FlushCommandQueue();
	void FlushCommandQueue(UINT64 fenceValue);
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;

	int GetCurrentBufferIndex()const;
private:

	void BuildSwapChain(HWND mHwnd, int clientWidth, int clientHeight);
	void BuildCommandObjects();
	void BuildMainDescriptorHeaps();
};
#endif
