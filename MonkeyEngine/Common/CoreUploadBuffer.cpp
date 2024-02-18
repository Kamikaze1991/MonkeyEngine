#include "CoreUploadBuffer.h"

template<typename T>
CoreUploadBuffer<T>::CoreUploadBuffer(ID3D12Device *device, UINT elementCount, bool isConstantBuffer)
{
	mIsConstantBuffer = isConstantBuffer;
	mElementByteSize = sizeof(T);
	if (mIsConstantBuffer)
		mElementByteSize = CoreUtil::CalcConstantBufferByteSize(mElementByteSize);
	D3D12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	ExceptionFuse(
		device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(mElementByteSize * elementCount),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(UploadBuffer.GetAddressOf()))
	);
	
	ExceptionFuse(UploadBuffer->Map(0, nullptr, static_cast<void**>(&MappedData)));
}

template<typename T>
CoreUploadBuffer<T>::~CoreUploadBuffer()
{
	if (UploadBuffer)
		UploadBuffer->Unmap(0, nullptr);
	MappedData = nullptr;
}

template<typename T>
void CoreUploadBuffer<T>::CopyData(int elementIndex, const T& data)
{
	memcpy(&MappedData[elementIndex * mElementByteSize], &data, sizeof(T));
}

template<typename T>
ID3D12Resource* CoreUploadBuffer<T>::GetUploadBuffer()
{
	return UploadBuffer.Get();
}
