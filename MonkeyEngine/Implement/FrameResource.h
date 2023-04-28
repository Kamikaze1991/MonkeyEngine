#include "../Common/CoreUtil.h"
struct FrameResource {
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mFrameCommandAllocator;
	UINT64 mFrameFenceCount=0;
	FrameResource(ID3D12Device* mDevice);
};
