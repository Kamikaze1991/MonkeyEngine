#include "FrameResource.h"

FrameResource::FrameResource(ID3D12Device* DeviceControl, UINT passCount, UINT objectCount, UINT materialCount)
{
	DeviceControl->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(FrameResourceCommandAllocator.GetAddressOf()));
}
