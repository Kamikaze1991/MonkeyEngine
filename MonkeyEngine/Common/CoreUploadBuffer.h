#ifndef _CORE_UPLOAD_BUFFER_H_
#define _CORE_UPLOAD_BUFFER_H_
#include "CoreUtil.h"

template<typename T>
class CoreUploadBuffer
{
public:
    CoreUploadBuffer(ID3D12Device* device, UINT elementCount, bool isConstantBuffer) :
        mIsConstantBuffer(isConstantBuffer)
    {
        mElementByteSize = sizeof(T);
        if (isConstantBuffer)
            mElementByteSize = CoreUtil::CalcConstantBufferByteSize(sizeof(T));

        D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(mElementByteSize * elementCount);
        ExceptionFuse(device->CreateCommittedResource(
            &heapProp,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(mUploadBuffer.GetAddressOf())));
        ExceptionFuse(mUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mMappedData)));

        // We do not need to unmap until we are done with the resource.  However, we must not write to
        // the resource while it is in use by the GPU (so we must use synchronization techniques).
    }

    CoreUploadBuffer(const CoreUploadBuffer& rhs) = delete;
    CoreUploadBuffer& operator=(const CoreUploadBuffer& rhs) = delete;
    ~CoreUploadBuffer()
    {
        if (mUploadBuffer != nullptr)
            mUploadBuffer->Unmap(0, nullptr);

        mMappedData = nullptr;
    }

    ID3D12Resource* Resource()const
    {
        return mUploadBuffer.Get();
    }

    void CopyData(int elementIndex, const T& data)
    {
        memcpy(&mMappedData[elementIndex * mElementByteSize], &data, sizeof(T));
    }

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> mUploadBuffer;
    BYTE* mMappedData = nullptr;
    UINT mElementByteSize = 0;
    bool mIsConstantBuffer = false;
};
#endif


