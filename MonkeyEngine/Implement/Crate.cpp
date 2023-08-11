#include "Crate.h"

void Crate::OnUpdate()
{
	mCurrFrameResourceIndex = (mCurrFrameResourceIndex + 1) % 3;
	mCurrFrameResource = mFrameResources[mCurrFrameResourceIndex].get();
	

	if (mCurrFrameResource->mFrameFenceCount != 0 && mCoreGraphics->mFence->GetCompletedValue() < mCurrFrameResource->mFrameFenceCount)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
		ExceptionFuse(mCoreGraphics->mFence->SetEventOnCompletion(mCurrFrameResource->mFrameFenceCount, eventHandle));
		if (eventHandle) {
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}

	//mCoreGraphics->FlushCommandQueue(mCurrFrameResource->mFrameFenceCount);
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
	D3D12_RESOURCE_BARRIER rbInitial = CD3DX12_RESOURCE_BARRIER::Transition(mCoreGraphics->mRenderTargetBuffer[mCurrFrame].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	GetEngineGraphicsCommandList()->ResourceBarrier(1, &rbInitial);
	CD3DX12_CPU_DESCRIPTOR_HANDLE descriptor(mCoreGraphics->mRtvHeap->GetCPUDescriptorHandleForHeapStart(), mCurrFrame, mCoreGraphics->mRtvHeapSize);
	float clearColor[] = { 0.0f, 1.0f, 0.0f, 1.0f };
	GetEngineGraphicsCommandList()->ClearRenderTargetView(descriptor, clearColor, 0, nullptr);
	GetEngineGraphicsCommandList()->ClearDepthStencilView(mCoreGraphics->DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	D3D12_CPU_DESCRIPTOR_HANDLE currBackBuffer = mCoreGraphics->CurrentBackBufferView(mCurrFrame);
	D3D12_CPU_DESCRIPTOR_HANDLE currDepthStencil = mCoreGraphics->DepthStencilView();
	GetEngineGraphicsCommandList()->OMSetRenderTargets(1, &currBackBuffer, true, &currDepthStencil);

	ID3D12DescriptorHeap* descriptorHeaps[] = { mCoreGraphics->mCbvSrvUavHeap.Get() };
	GetEngineGraphicsCommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	//GetEngineGraphicsCommandList()->SetDescriptorHeaps(1, &mCoreGraphics->mCbvSrvUavHeap);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), GetEngineGraphicsCommandList().Get());
	rbInitial = CD3DX12_RESOURCE_BARRIER::Transition(mCoreGraphics->mRenderTargetBuffer[mCurrFrame].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	GetEngineGraphicsCommandList()->ResourceBarrier(1, &rbInitial);
	GetEngineGraphicsCommandList()->Close();

	ID3D12CommandList* mList[] = { GetEngineGraphicsCommandList().Get() };
	GetEngineCommandQueue()->ExecuteCommandLists(_countof(mList), mList);
	GetEngineSwapChain()->Present(1, 0);
	mCurrFrameResource->mFrameFenceCount = ++mCoreGraphics->mFenceCount;

	GetEngineCommandQueue()->Signal(mCoreGraphics->mFence.Get(), mCoreGraphics->mFenceCount);

	//mCoreGraphics->FlushCommandQueue(mCurrFrameResource->mFrameFenceCount);
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
	mCoreGraphics->FlushCommandQueue();
}

void Crate::BuildFrameResurces()
{
	for (int i = 0; i < 3; ++i)
	{
		mFrameResources.push_back(std::make_unique<FrameResource>(mCoreGraphics->mDevice.Get()));
	}
}
