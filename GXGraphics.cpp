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
