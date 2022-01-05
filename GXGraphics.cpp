#include "GXGraphics.h"

GXGraphics::GXGraphics(HWND hwnd, bool windowed, UINT width, UINT height)
{
	mMainHwnd = hwnd;
	mClientWidth = width;
	mCLientHeight = height;
	mWindowed = windowed;
}

/// <summary>
/// Begin of the Graphics Fun
/// </summary>
/// <returns>true or false</returns>
bool GXGraphics::initialize()
{
	///enable debug layer
#if defined(DEBUG)||defined(_DEBUG)
	{
		ComPtr<ID3D12Debug> debugController;
		GXManageException(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf())));
		debugController->EnableDebugLayer();

	}
#endif
	GXManageException(CreateDXGIFactory1(IID_PPV_ARGS(mFactory.GetAddressOf())));
	
	/// <summary>
	/// create device. 
	/// </summary>
	HRESULT hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(mDevice.GetAddressOf()));
	if (FAILED(hr)) {
		ComPtr<IDXGIAdapter> wrapAdapter;
		GXManageException(mFactory->EnumWarpAdapter(IID_PPV_ARGS(wrapAdapter.GetAddressOf())));
		GXManageException(D3D12CreateDevice(wrapAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(mDevice.GetAddressOf())));
	}

	GXManageException(mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(mFence.GetAddressOf())));

	mRtvHeapSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mDsvHeapSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	mCbvUavSrvSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

#ifdef _DEBUG
	graphicsLog();
#endif

	/// <summary>
	/// builders
	/// </summary>
	buildCommandObjects();
	buildSwapChain();
	buildDescriptorHeaps();
}

/// <summary>
/// Descriptor Heap Build
/// </summary>
void GXGraphics::buildDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC mRtvHeapDesc = {};
	mRtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	mRtvHeapDesc.NumDescriptors = mBufferCount;
	mRtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	GXManageException(mDevice->CreateDescriptorHeap(&mRtvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())))

	D3D12_DESCRIPTOR_HEAP_DESC mDsvHeapDesc = {};
	mDsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	mDsvHeapDesc.NumDescriptors = 1;
	mDsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	GXManageException(mDevice->CreateDescriptorHeap(&mDsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())));
	
}

/// <summary>
/// Swap chain constructor
/// </summary>
void GXGraphics::buildSwapChain()
{
	DXGI_SWAP_CHAIN_DESC sc = {};
	//Buffer Description Configuration
	sc.BufferDesc.Width = mClientWidth;
	sc.BufferDesc.Height = mCLientHeight;
	sc.BufferDesc.Format = mGraphicFormat;
	sc.BufferDesc.RefreshRate.Numerator = 60;
	sc.BufferDesc.RefreshRate.Denominator = 1;
	sc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	//Sample Description Configuration
	sc.SampleDesc.Count = 1;
	sc.SampleDesc.Quality = 0;

	//General SwapChain Configs
	sc.Windowed = mWindowed;
	sc.BufferCount = mBufferCount;
	sc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sc.OutputWindow = mMainHwnd;
	sc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	GXManageException(mFactory->CreateSwapChain(mDevice.Get(), &sc, mSwapChain.GetAddressOf()));
}

/// <summary>
/// Build Commands Components
/// </summary>
void GXGraphics::buildCommandObjects()
{
	///crerar los 2 commands
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	GXManageException(mDevice->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(mCommandQueue.GetAddressOf())));

	GXManageException(mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(mCommandAllocator.GetAddressOf())));
	GXManageException(mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), nullptr, IID_PPV_ARGS(mCommandList.GetAddressOf())));
	mCommandList->Close();
}

/// <summary>
/// Generig log for debug purposes
/// </summary>
void GXGraphics::graphicsLog()
{
	UINT counter = 0;
	IDXGIAdapter* adapter = nullptr;
	std::vector<IDXGIAdapter*> adapterStack;

	while (mFactory->EnumAdapters(counter, &adapter) != DXGI_ERROR_NOT_FOUND) {
		DXGI_ADAPTER_DESC adapterDesc;
		adapter->GetDesc(&adapterDesc);

		std::wstring text = L"Adapter:";
		text+= adapterDesc.Description;
		text+= L"\n";
		OutputDebugString(text.c_str());
		adapterStack.push_back(adapter);
		++counter;
	}
	for (int i = 0; i < adapterStack.size(); i++) {
		logAdapterOutputs(adapterStack[i]);
		ReleaseCom(adapterStack[i]);
	}
}

/// <summary>
/// Details device name
/// </summary>
/// <param name="_adapter">adapter artifact</param>
void GXGraphics::logAdapterOutputs(IDXGIAdapter* _adapter)
{
	UINT i = 0;
	IDXGIOutput* output=nullptr;

	while (_adapter->EnumOutputs(i,&output)!=DXGI_ERROR_NOT_FOUND)
	{
		DXGI_OUTPUT_DESC desc;
		output->GetDesc(&desc);

		std::wstring text = L"Output";
		text += desc.DeviceName;
		text += L"\n";
		logOutputDisplayMode(output, mGraphicFormat);
		ReleaseCom(output);
		i++;
	}

}

/// <summary>
/// Log details of device... refresh rate and screen sizes
/// </summary>
/// <param name="_output">output artifact</param>
/// <param name="_format">format</param>
void GXGraphics::logOutputDisplayMode(IDXGIOutput* _output, DXGI_FORMAT _format)
{
	UINT count = 0;
	UINT flags = 0;

	_output->GetDisplayModeList(_format, flags, &count, nullptr);
	std::vector<DXGI_MODE_DESC> modes(count);
	_output->GetDisplayModeList(_format, flags, &count, &modes[0]);

	for (auto& x : modes) {
		UINT numerator = x.RefreshRate.Numerator;
		UINT denominator = x.RefreshRate.Denominator;
		std::wstring text =
			L"Width" + std::to_wstring(x.Width) + L" " +
			L"Height" + std::to_wstring(x.Height) + L" " +
			L"RefreshRate" + std::to_wstring(numerator) + L"/" + std::to_wstring(denominator) +
			L"\n";
	}


}
