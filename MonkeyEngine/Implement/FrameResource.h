#include "../Common/CoreUtil.h"
class FrameResource {
public:
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> FrameResourceCommandAllocator;
	UINT64 FrameResourceFenceCount=0;
	FrameResource(ID3D12Device* DeviceControl);
};
