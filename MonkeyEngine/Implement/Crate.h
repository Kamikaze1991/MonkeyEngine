#ifndef _CRATE_H_
#define _CRATE_H_
#include "../Common/CoreUtil.h"
#include "../Common/CoreEngine.h"
#include "FrameResource.h"
#include "../Common/PipelineState/GraphicsPSO.h"

const int gNumFrameResources = 4;
struct Texture
{
	// Unique material name for lookup.
	std::string Name;

	std::wstring Filename;

	Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
};

struct Material
{
	std::string Name;
	int MatCBIndex = -1;
	int DiffuseSrvHeapIndex = -1;
	int NormalSrvHeapIndex = -1;
	int NumFramesDirty = gNumFrameResources;
	DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
	float Roughness = .25f;
	DirectX::XMFLOAT4X4 MatTransform = CoreUtil::Identity4x4();
};

struct SubmeshGeometry
{
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	INT BaseVertexLocation = 0;

	// Bounding box of the geometry defined by this submesh. 
	// This is used in later chapters of the book.
	DirectX::BoundingBox Bounds;
};

struct MeshGeometry
{
	// Give it a name so we can look it up by name.
	std::string Name;

	// System memory copies.  Use Blobs because the vertex/index format can be generic.
	// It is up to the client to cast appropriately.  
	Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> IndexBufferCPU = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;

	// Data about the buffers.
	UINT VertexByteStride = 0;
	UINT VertexBufferByteSize = 0;
	DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
	UINT IndexBufferByteSize = 0;

	// A MeshGeometry may store multiple geometries in one vertex/index buffer.
	// Use this container to define the Submesh geometries so we can draw
	// the Submeshes individually.
	std::unordered_map<std::string, SubmeshGeometry> DrawArgs;

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView()const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = VertexByteStride;
		vbv.SizeInBytes = VertexBufferByteSize;

		return vbv;
	}



	D3D12_INDEX_BUFFER_VIEW IndexBufferView()const
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
		ibv.Format = IndexFormat;
		ibv.SizeInBytes = IndexBufferByteSize;

		return ibv;
	}

	// We can free this memory after we finish upload to the GPU.
	void DisposeUploaders()
	{
		VertexBufferUploader = nullptr;
		IndexBufferUploader = nullptr;
	}
};

struct RenderItem
{
	RenderItem() = default;

	XMFLOAT4X4 World = CoreUtil::Identity4x4();

	XMFLOAT4X4 TexTransform = CoreUtil::Identity4x4();

	int NumFramesDirty = gNumFrameResources;

	UINT ObjCBIndex = -1;

	Material* Mat = nullptr;
	MeshGeometry* Geo = nullptr;

	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	int BaseVertexLocation = 0;
};
class Crate :public CoreEngine {
private:
	std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;
	std::unordered_map<std::string, std::unique_ptr<Material>> mMaterials;
	std::vector<RenderItem*> mOpaqueRitems;
	std::vector<std::unique_ptr<RenderItem>> mAllRitems;
	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
	ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;
	std::unordered_map<std::string, ComPtr<ID3DBlob>> mShaders;
	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
	UINT mCbvSrvDescriptorSize = 0;

	GraphicsPSO DefaultPPSO = { (L"Sponza: Depth PSO") };


	std::vector<std::unique_ptr<FrameResource>> FrameResources;
	FrameResource* CurrFrameResource = nullptr;
	ComPtr<ID3D12DescriptorHeap> ShadowResourceViewDescriptorHeap = nullptr;
	int CurrentFrameResourceIndex = 0;

	PassConstants mMainPassCB;

	XMFLOAT3 mEyePos = { 0.0f, 0.0f, 0.0f };
	XMFLOAT4X4 mView = CoreUtil::Identity4x4();
	XMFLOAT4X4 mProj = CoreUtil::Identity4x4();

	float mTheta = XM_PIDIV2;
	float mPhi = XM_PIDIV2;
	float mRadius = 2.0f;

	POINT mLastMousePos;

public:
	virtual void OnUpdate(const CoreTimer& gt);
	virtual void OnRender();
	virtual void OnInitialize();
	virtual void OnInitializeUi();
	
	void PopulateCommands();
	~Crate();
	Crate()=default;
	Crate(int among);
	
private:

	void UpdateCamera(const CoreTimer& gt);
	void UpdateObjectCBs(const CoreTimer& gt);
	void UpdateMaterialCBs(const CoreTimer& gt);
	void UpdateMainPassCB(const CoreTimer& gt);


	//specific funcions
	void LoadTextures();
	void BuildRootSignature();
	void BuildLocalDescriptorHeap();
	void BuildShadersAndInputLayout();
	void BuildShapeGeometry();
	void BuildMaterials();
	void BuildRenderItems();
	void BuildFrameResurces();
	void BuildPSOs();
	void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems);
	
	void BuilduserInterface();
	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();
};

#endif

