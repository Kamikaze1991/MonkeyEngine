#include "Crate.h"

void Crate::OnUpdate()
{

	mCurrFrameResourceIndex = (mCurrFrameResourceIndex + 1) % 3;
	mCurrFrameResource = mFrameResources[mCurrFrameResourceIndex].get();
	// Has the GPU finished processing the commands of the current frame resource?
	// If not, wait until the GPU has completed commands up to this fence point.
	FlushCommandQueue(mCurrFrameResource->mFrameFenceCount);
	
}

void Crate::OnRender()
{
	PopulateCommands();
}

void Crate::PopulateCommands()
{
	auto mCurrCmd = mCurrFrameResource->mFrameCommandAllocator;
	mCurrCmd->Reset();
	mGraphicsCommandList->Reset(mCurrCmd.Get(), nullptr);
	const D3D12_RESOURCE_BARRIER rbInitial = CD3DX12_RESOURCE_BARRIER::Transition(mSwapChainBuffer[mCurrFrame].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	mGraphicsCommandList->ResourceBarrier(1, &rbInitial);

	CD3DX12_CPU_DESCRIPTOR_HANDLE descriptor(mRtvHeap->GetCPUDescriptorHandleForHeapStart(), mCurrFrame, mRtvHeapSize);
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	mGraphicsCommandList->ClearRenderTargetView(descriptor, clearColor, 0, nullptr);
	const D3D12_RESOURCE_BARRIER rbFinal = CD3DX12_RESOURCE_BARRIER::Transition(mSwapChainBuffer[mCurrFrame].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	mGraphicsCommandList->ResourceBarrier(1, &rbFinal);
	mGraphicsCommandList->Close();

	ID3D12CommandList* mList[] = { mGraphicsCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(mList), mList);
	mSwapChain->Present(0, 0);
	mCurrFrameResource->mFrameFenceCount = mFenceCount++;
	mCommandQueue->Signal(mFence.Get(), mFenceCount);

	FlushCommandQueue(mCurrFrameResource->mFrameFenceCount);
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
	for (int i = 0; i < 3; ++i)
	{
		mFrameResources.push_back(std::make_unique<FrameResource>(mDevice.Get()));
	}
}
