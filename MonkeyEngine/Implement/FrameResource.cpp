#include "FrameResource.h"

FrameResource::FrameResource(ID3D12Device* DeviceControl, UINT passCount, UINT objectCount, UINT materialCount)
{
    ExceptionFuse(DeviceControl->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(FrameResourceCommandAllocator.GetAddressOf())));

    //  FrameCB = std::make_unique<UploadBuffer<FrameConstants>>(device, 1, true);
    PassCB = std::make_unique<CoreUploadBuffer<PassConstants>>(DeviceControl, passCount, true);
    MaterialCB = std::make_unique<CoreUploadBuffer<MaterialConstants>>(DeviceControl, materialCount, true);
    ObjectCB = std::make_unique<CoreUploadBuffer<ObjectConstants>>(DeviceControl, objectCount, true);
}
