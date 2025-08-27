#include "Crate.h"

void Crate::OnUpdate()
{
	CurrentFrameResourceIndex = (CurrentFrameResourceIndex + 1) % 3;
	CurrFrameResource = FrameResources[CurrentFrameResourceIndex].get();
	

	if (CurrFrameResource->FrameResourceFenceCount != 0 && mCoreGraphics->FenceControl->GetCompletedValue() < CurrFrameResource->FrameResourceFenceCount)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
		ExceptionFuse(mCoreGraphics->FenceControl->SetEventOnCompletion(CurrFrameResource->FrameResourceFenceCount, eventHandle));
		if (eventHandle) {
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}

	//mCoreGraphics->FlushCommandQueue(CurrFrameResource->FrameResourceFenceCount);
}

void Crate::OnRender()
{
	PopulateCommands();
}

void Crate::PopulateCommands()
{
	auto mCurrCmd = CurrFrameResource->FrameResourceCommandAllocator;
	mCurrCmd->Reset();
	GetEngineGraphicsCommandList()->Reset(mCurrCmd.Get(), nullptr);
	D3D12_RESOURCE_BARRIER rbInitial = CD3DX12_RESOURCE_BARRIER::Transition(mCoreGraphics->RenderTargetBuffer[CurrFrame].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	GetEngineGraphicsCommandList()->ResourceBarrier(1, &rbInitial);
	CD3DX12_CPU_DESCRIPTOR_HANDLE descriptor(mCoreGraphics->RenderTargetViewHeap->GetCPUDescriptorHandleForHeapStart(), CurrFrame, mCoreGraphics->RenderTargetViewHeapSize);
	float clearColor[] = { ClearColor.x/ ClearColor.w,ClearColor.y / ClearColor.w, ClearColor.z / ClearColor.w, ClearColor.w };
	GetEngineGraphicsCommandList()->ClearRenderTargetView(descriptor, clearColor, 0, nullptr);
	GetEngineGraphicsCommandList()->ClearDepthStencilView(mCoreGraphics->DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	D3D12_CPU_DESCRIPTOR_HANDLE currBackBuffer = mCoreGraphics->CurrentBackBufferView(CurrFrame);
	D3D12_CPU_DESCRIPTOR_HANDLE currDepthStencil = mCoreGraphics->DepthStencilView();
	GetEngineGraphicsCommandList()->OMSetRenderTargets(1, &currBackBuffer, true, &currDepthStencil);

	ID3D12DescriptorHeap* descriptorHeaps[] = { ShadowResourceViewDescriptorHeap.Get() };
	GetEngineGraphicsCommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	//GetEngineGraphicsCommandList()->SetDescriptorHeaps(1, &mCoreGraphics->mCbvSrvUavHeap);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), GetEngineGraphicsCommandList().Get());
	rbInitial = CD3DX12_RESOURCE_BARRIER::Transition(mCoreGraphics->RenderTargetBuffer[CurrFrame].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	GetEngineGraphicsCommandList()->ResourceBarrier(1, &rbInitial);
	GetEngineGraphicsCommandList()->Close();

	ID3D12CommandList* mList[] = { GetEngineGraphicsCommandList().Get() };
	GetEngineCommandQueue()->ExecuteCommandLists(_countof(mList), mList);
	GetEngineSwapChain()->Present(1, 0);
	CurrFrameResource->FrameResourceFenceCount = ++mCoreGraphics->FenceControlCount;

	GetEngineCommandQueue()->Signal(mCoreGraphics->FenceControl.Get(), mCoreGraphics->FenceControlCount);

	//mCoreGraphics->FlushCommandQueue(CurrFrameResource->FrameResourceFenceCount);
}

Crate::~Crate()
{
		
}

Crate::Crate(int among):CoreEngine()
{
	
}

void Crate::BuildRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE texTable;
	texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER slotRootParameter[4];

	// Perfomance TIP: Order from most frequent to least frequent.
	slotRootParameter[0].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);
	slotRootParameter[1].InitAsConstantBufferView(0);
	slotRootParameter[2].InitAsConstantBufferView(1);
	slotRootParameter[3].InitAsConstantBufferView(2);

	auto staticSamplers = GetStaticSamplers();

	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(4, slotRootParameter,
		(UINT)staticSamplers.size(), staticSamplers.data(),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer


	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ExceptionFuse(hr);

	ExceptionFuse(mCoreGraphics->DeviceControl->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(mRootSignature.GetAddressOf())));
}


void Crate::OnInitialize()
{
	LoadTextures();
	BuildRootSignature();
	BuildLocalDescriptorHeap();
	BuildShadersAndInputLayout();
	BuildShapeGeometry();
	BuildMaterials();
	BuildRenderItems();
	BuildFrameResurces();
	BuilduserInterface();
	mCoreGraphics->FlushCommandQueue();
}

void Crate::BuildRenderItems()
{
	auto boxRitem = std::make_unique<RenderItem>();
	boxRitem->ObjCBIndex = 0;
	boxRitem->Mat = mMaterials["woodCrate"].get();
	boxRitem->Geo = mGeometries["boxGeo"].get();
	boxRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
	boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
	boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;
	mAllRitems.push_back(std::move(boxRitem));

	// All the render items are opaque.
	for (auto& e : mAllRitems)
		mOpaqueRitems.push_back(e.get());
}

void Crate::OnInitializeUi()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	if (ShowDemoWindow)
		ImGui::ShowDemoWindow(&ShowDemoWindow);

	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!x");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &ShowDemoWindow);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &ShowAnotherWindow);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&ClearColor); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);
		ImGui::End();
	}

	if (ShowAnotherWindow)
	{
		ImGui::Begin("Another Window", &ShowAnotherWindow);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			ShowAnotherWindow = false;
		ImGui::End();
	}
	ImGui::Render();
	//ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pd3dCommandList);

}

void Crate::BuildFrameResurces()
{
	for (int i = 0; i < gNumFrameResources; ++i)
	{
		FrameResources.push_back(std::make_unique<FrameResource>(mCoreGraphics->DeviceControl.Get(),
			1, (UINT)mAllRitems.size(), (UINT)mMaterials.size()));
	}
}

void Crate::LoadTextures()
{
	auto woodCrateTex = std::make_unique<Texture>();
	woodCrateTex->Name = "woodCrateTex";
	woodCrateTex->Filename = L"Textures/robo.dds";
	ExceptionFuse(DirectX::CreateDDSTextureFromFile12(mCoreGraphics->DeviceControl.Get(),
		GetEngineGraphicsCommandList().Get(), woodCrateTex->Filename.c_str(),
		woodCrateTex->Resource, woodCrateTex->UploadHeap));

	mTextures[woodCrateTex->Name] = std::move(woodCrateTex);
}

void Crate::BuildMaterials()
{
	auto woodCrate = std::make_unique<Material>();
	woodCrate->Name = "woodCrate";
	woodCrate->MatCBIndex = 0;
	woodCrate->DiffuseSrvHeapIndex = 0;
	woodCrate->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	woodCrate->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	woodCrate->Roughness = 0.2f;

	mMaterials["woodCrate"] = std::move(woodCrate);
}

void Crate::BuildShapeGeometry()
{

	GeometryGenerator geoGen;
	MeshData box = geoGen.CreateBox(2.0f, 2.0f, 2.0f, 0);
	SubmeshGeometry boxSubmesh;
	boxSubmesh.IndexCount = (UINT)box.Indices32.size();
	boxSubmesh.StartIndexLocation = 0;
	boxSubmesh.BaseVertexLocation = 0;



	std::vector<Vertex> vertices(box.Vertices.size());

	for (size_t i = 0; i < box.Vertices.size(); ++i)
	{
		vertices[i].Pos = box.Vertices[i].Position;
		vertices[i].Normal = box.Vertices[i].Normal;
		vertices[i].TexC = box.Vertices[i].TexC;
	}

	std::vector<std::uint16_t> indices = box.GetIndices16();

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "boxGeo";

	ExceptionFuse(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ExceptionFuse(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = CoreUtil::CreateDefaultBuffer(GetEngineGraphicsCommandList().Get(), 
		mCoreGraphics->DeviceControl.Get(), vbByteSize, vertices.data(), geo->VertexBufferUploader);

	geo->IndexBufferGPU = CoreUtil::CreateDefaultBuffer(GetEngineGraphicsCommandList().Get(),
		mCoreGraphics->DeviceControl.Get(), ibByteSize, indices.data(), geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Vertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	geo->DrawArgs["box"] = boxSubmesh;

	mGeometries[geo->Name] = std::move(geo);
}

void Crate::BuildShadersAndInputLayout()
{
	mShaders["standardVS"] = CoreUtil::CompileShader(L"Resources\\Shaders\\Default.hlsl", nullptr, "VS", "vs_5_0");
	mShaders["opaquePS"] = CoreUtil::CompileShader(L"Resources\\Shaders\\Default.hlsl", nullptr, "PS", "ps_5_0");

	mInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
}

void Crate::BuildLocalDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC mSrvDesc = {};
	mSrvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	mSrvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	mSrvDesc.NumDescriptors = 1;
	ExceptionFuse(mCoreGraphics->DeviceControl->CreateDescriptorHeap(&mSrvDesc, IID_PPV_ARGS(ShadowResourceViewDescriptorHeap.GetAddressOf())));

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ExceptionFuse(mCoreGraphics->DeviceControl->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&mSrvDescriptorHeap)));

	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(mSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = mTextures["woodCrateTex"]->Resource->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = mTextures["woodCrateTex"]->Resource->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	mCoreGraphics->DeviceControl->CreateShaderResourceView(mTextures["woodCrateTex"]->Resource.Get(), &srvDesc, hDescriptor);

}

void Crate::BuilduserInterface()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO(); (void)io;
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(MainHwnd);
	ImGui_ImplDX12_Init(mCoreGraphics->DeviceControl.Get(), 3,
		DXGI_FORMAT_R8G8B8A8_UNORM, ShadowResourceViewDescriptorHeap.Get(),
		ShadowResourceViewDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		ShadowResourceViewDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
}

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> Crate::GetStaticSamplers()
{
	// Applications usually only need a handful of samplers.  So just define them all up front
	// and keep them available as part of the root signature.  

	const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
		0, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
		1, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		2, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
		3, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
		4, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
		0.0f,                             // mipLODBias
		8);                               // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
		5, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
		0.0f,                              // mipLODBias
		8);                                // maxAnisotropy

	return {
		pointWrap, pointClamp,
		linearWrap, linearClamp,
		anisotropicWrap, anisotropicClamp };
}
