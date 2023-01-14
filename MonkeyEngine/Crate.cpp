#include "Crate.h"

void Crate::OnUpdate()
{

}

void Crate::OnRender()
{
	PopulateCommands();

	ID3D12CommandList* mList[] = { mGraphicsCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(mList), mList);
	mSwapChain->Present(1, 0);
	FlushCommandQueue();
}

void Crate::PopulateCommands()
{
	mGraphicsCommandList->Reset(mCommandAllocator.Get(), nullptr);
	const D3D12_RESOURCE_BARRIER rbInitial = CD3DX12_RESOURCE_BARRIER::Transition(mSwapChainBuffer[mCurrFrame].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	mGraphicsCommandList->ResourceBarrier(1, &rbInitial);

	CD3DX12_CPU_DESCRIPTOR_HANDLE descriptor(mRtvHeap->GetCPUDescriptorHandleForHeapStart(), mCurrFrame, mRtvHeapSize);
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	mGraphicsCommandList->ClearRenderTargetView(descriptor, clearColor, 0, nullptr);
	const D3D12_RESOURCE_BARRIER rbFinal = CD3DX12_RESOURCE_BARRIER::Transition(mSwapChainBuffer[mCurrFrame].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	mGraphicsCommandList->ResourceBarrier(1, &rbFinal);
	mGraphicsCommandList->Close();

}

Crate::~Crate()
{
		FlushCommandQueue();
		mCommandAllocator->Reset();
}

Crate::Crate(int among)
{
	error = among;
}

void Crate::OnInitialize()
{
}
