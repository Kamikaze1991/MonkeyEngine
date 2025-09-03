#include "../Common/CoreUtil.h"
#include "../Common/CoreUploadBuffer.h"

struct Light
{
    DirectX::XMFLOAT3 Strength = { 0.5f, 0.5f, 0.5f };
    float FalloffStart = 1.0f;                          // point/spot light only
    DirectX::XMFLOAT3 Direction = { 0.0f, -1.0f, 0.0f };// directional/spot light only
    float FalloffEnd = 10.0f;                           // point/spot light only
    DirectX::XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };  // point/spot light only
    float SpotPower = 64.0f;                            // spot light only
};

struct ObjectConstants
{
    DirectX::XMFLOAT4X4 World = CoreUtil::Identity4x4();
    DirectX::XMFLOAT4X4 TexTransform = CoreUtil::Identity4x4();
};

struct MaterialConstants
{
    DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
    DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
    float Roughness = 0.25f;

    // Used in texture mapping.
    DirectX::XMFLOAT4X4 MatTransform = CoreUtil::Identity4x4();
};

struct PassConstants
{
    DirectX::XMFLOAT4X4 View = CoreUtil::Identity4x4();
    DirectX::XMFLOAT4X4 InvView = CoreUtil::Identity4x4();
    DirectX::XMFLOAT4X4 Proj = CoreUtil::Identity4x4();
    DirectX::XMFLOAT4X4 InvProj = CoreUtil::Identity4x4();
    DirectX::XMFLOAT4X4 ViewProj = CoreUtil::Identity4x4();
    DirectX::XMFLOAT4X4 InvViewProj = CoreUtil::Identity4x4();
    DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
    float cbPerObjectPad1 = 0.0f;
    DirectX::XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
    DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
    float NearZ = 0.0f;
    float FarZ = 0.0f;
    float TotalTime = 0.0f;
    float DeltaTime = 0.0f;

    DirectX::XMFLOAT4 AmbientLight = { 0.0f, 0.0f, 0.0f, 1.0f };

    // Indices [0, NUM_DIR_LIGHTS) are directional lights;
    // indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
    // indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
    // are spot lights for a maximum of MaxLights per object.
    Light Lights[MaxLights];
};

struct Vertex
{
    DirectX::XMFLOAT3 Pos;
    DirectX::XMFLOAT3 Normal;
    DirectX::XMFLOAT2 TexC;
};

class FrameResource {
public:
	FrameResource(ID3D12Device* DeviceControl, UINT passCount, UINT objectCount, UINT materialCount);
	FrameResource(const FrameResource& rhs) = delete;
	FrameResource& operator=(const FrameResource& rhs) = delete;
	~FrameResource()=default;

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> FrameResourceCommandAllocator;
	UINT64 FrameResourceFenceCount=0;

    std::unique_ptr<CoreUploadBuffer<PassConstants>> PassCB = nullptr;
    std::unique_ptr<CoreUploadBuffer<MaterialConstants>> MaterialCB = nullptr;
    std::unique_ptr<CoreUploadBuffer<ObjectConstants>> ObjectCB = nullptr;
	
};
