#ifndef _IGRAPHICS_ENGINE_H_
#define _IGRAPHICS_ENGINE_H_
#include "CoreUtil.h"
class ICoreGraphics {
public:
    virtual ~ICoreGraphics() = default;

    virtual void InitDirect3D(HWND hwnd, int width, int height) = 0;
    virtual void OnReset(int width, int height) = 0;

    virtual void FlushCommandQueue() = 0;
    virtual void FlushCommandQueue(UINT64 fenceValue) = 0;

    virtual D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const = 0;
    virtual D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const = 0;

    virtual int GetCurrentBufferIndex() const = 0;
    virtual ID3D12GraphicsCommandList* GetGraphicsCommandList() const = 0;
    virtual ID3D12CommandQueue* GetEngineCommandQueue() const = 0;
    virtual ID3D12CommandAllocator* GetEngineCommandAllocator() const = 0;
    virtual IDXGISwapChain3* GetEngineSwapChain() const = 0;

    virtual D3D12_VIEWPORT GetViewPort()const = 0;
    virtual D3D12_RECT GetScissorRect()const = 0;

    virtual ID3D12Resource* GetCurrentBackBuffer() const = 0;
    virtual ID3D12Resource* GetDepthStencilBuffer() const = 0;

    virtual ID3D12Device* GetDeviceControl() const = 0;

    virtual DXGI_FORMAT GetBackBufferFormat() const = 0;
    virtual DXGI_FORMAT GetDepthStencilFormat() const = 0;
    virtual int GetMsaaState() const = 0;
    virtual int GetMsaaQuality() const = 0;

    virtual int SyncFenceCount() = 0;

};

#endif
