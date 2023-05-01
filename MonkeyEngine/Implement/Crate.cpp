#include "Crate.h"

void Crate::OnUpdate()
{
	mCurrFrameResourceIndex = (mCurrFrameResourceIndex + 1) % 3;
	mCurrFrameResource = mFrameResources[mCurrFrameResourceIndex].get();
	

	if (mCurrFrameResource->mFrameFenceCount != 0 && mGsx->mFence->GetCompletedValue() < mCurrFrameResource->mFrameFenceCount)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
		ExceptionFuse(mGsx->mFence->SetEventOnCompletion(mCurrFrameResource->mFrameFenceCount, eventHandle));
		if (eventHandle) {
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}

	//mGsx->FlushCommandQueue(mCurrFrameResource->mFrameFenceCount);
}

void Crate::OnRender()
{
	PopulateCommands();
}

void Crate::PopulateCommands()
{
	auto mCurrCmd = mCurrFrameResource->mFrameCommandAllocator;
	mCurrCmd->Reset();
	GetEngineGraphicsCommandList()->Reset(mCurrCmd.Get(), nullptr);
	D3D12_RESOURCE_BARRIER rbInitial = CD3DX12_RESOURCE_BARRIER::Transition(mGsx->mRenderTargetBuffer[mCurrFrame].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	GetEngineGraphicsCommandList()->ResourceBarrier(1, &rbInitial);

	CD3DX12_CPU_DESCRIPTOR_HANDLE descriptor(mGsx->mRtvHeap->GetCPUDescriptorHandleForHeapStart(), mCurrFrame, mGsx->mRtvHeapSize);
	float clearColor[] = { 0.0f, 1.0f, 0.0f, 1.0f };
	GetEngineGraphicsCommandList()->ClearRenderTargetView(descriptor, clearColor, 0, nullptr);
	rbInitial = CD3DX12_RESOURCE_BARRIER::Transition(mGsx->mRenderTargetBuffer[mCurrFrame].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	GetEngineGraphicsCommandList()->ResourceBarrier(1, &rbInitial);
	GetEngineGraphicsCommandList()->Close();

	ID3D12CommandList* mList[] = { GetEngineGraphicsCommandList().Get() };
	GetEngineCommandQueue()->ExecuteCommandLists(_countof(mList), mList);
	GetEngineSwapChain()->Present(1, 0);
	mCurrFrameResource->mFrameFenceCount = ++mGsx->mFenceCount;
	GetEngineCommandQueue()->Signal(mGsx->mFence.Get(), mGsx->mFenceCount);

	//mGsx->FlushCommandQueue(mCurrFrameResource->mFrameFenceCount);
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
	mGsx->FlushCommandQueue();
}

void Crate::BuildFrameResurces()
{
	for (int i = 0; i < 3; ++i)
	{
		mFrameResources.push_back(std::make_unique<FrameResource>(mGsx->mDevice.Get()));
	}
}
