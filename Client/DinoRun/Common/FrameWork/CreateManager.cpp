#include "CreateManager.h"

CreateManager::CreateManager()
{
}

CreateManager::~CreateManager()
{
}

void CreateManager::Initialize(HINSTANCE hInstance, HWND hWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hWnd;

	m_pDrawManager = shared_ptr<DrawManager>(new DrawManager);
	m_pNetWorkManager = shared_ptr<NetWorkManager>(new NetWorkManager);
	m_pSoundManager = shared_ptr<SoundManager>(new SoundManager);
	InitializeSound();

	CreateDirect3dDevice();
	CreateCommandQueueAndList();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();
	CreateGraphicsRootSignature();
	CreateComputeRootSignature();

	//CreateSwapChainRenderTargetViews();
	//CreateRenderTargetViews();
	//CreateDepthStencilView();

	m_pDrawManager->Initialize();
}
void CreateManager::InitializeSound()
{
	m_pSoundManager->AddSound("Start_BGM", "Resources/Sounds/Start_BGM.mp3", true);
	m_pSoundManager->AddSound("InGame_BGM", "Resources/Sounds/InGame_BGM.mp3", true);
	m_pSoundManager->AddSound("Mouse_Up", "Resources/Sounds/Mouse_Up.mp3", false);
	m_pSoundManager->AddSound("Mouse_Down", "Resources/Sounds/Mouse_Down.mp3", false);
	m_pSoundManager->AddSound("Gole", "Resources/Sounds/Gole.mp3", false);
	m_pSoundManager->AddSound("Heat", "Resources/Sounds/Heat.mp3", false);
	m_pSoundManager->AddSound("MeatEat", "Resources/Sounds/MeatEat.mp3", false);
	m_pSoundManager->AddSound("ItemBox", "Resources/Sounds/ItemBox.mp3", false);
}
void CreateManager::Release()
{
	HRESULT hResult;
	m_pDrawManager->WaitForGpuComplete();

	hResult = m_pdxgiSwapChain->SetFullscreenState(FALSE, NULL);

	m_pDrawManager->Release();
	m_pDrawManager.reset();

	m_pd3dCommandQueue.Reset();
	m_pd3dCommandAllocator.Reset();
	m_pd3dCommandList.Reset();

	m_pd3dRtvDescriptorHeap.Reset();
	m_pd3dDsvDescriptorHeap.Reset();

	m_pd3dDepthStencilBuffer.Reset();
	m_pd3dShadowDepthBuffer.Reset();

	for (int i = 0; i < N_RENDERTARGETBUFFERS; ++i)
	{
		m_ppd3dRenderTargetBuffers[i].Reset();
	}

	for (int i = 0; i < N_SWAPCHAINBUFFERS; ++i)
	{
		m_ppd3dSwapChainBackBuffers[i].Reset();
	}

	m_pGraphicsRootSignature.Reset();

	m_pd3dFence.Reset();

	m_pdxgiSwapChain.Reset();
	m_pd3dDevice.Reset();
	m_pdxgiFactory.Reset();
}

void CreateManager::RenderLoadingScreen(float loadingPercentage)
{
	m_pDrawManager->RenderLoadingScreen(loadingPercentage);
}

void CreateManager::Resize(int width, int height)
{
	m_nWndClientWidth = width;
	m_nWndClientHeight = height;

	OnResizeBackBuffers();
}

void CreateManager::OnResizeBackBuffers()
{
	HRESULT hResult;

	ResetCommandList();

	//m_pTextureToFullScreenShader.reset();
	for (int i = 0; i < N_RENDERTARGETBUFFERS; i++)
	{
		m_ppd3dRenderTargetBuffers[i].Reset();
	}

	for (int i = 0; i < N_SWAPCHAINBUFFERS; i++)
	{
		m_ppd3dSwapChainBackBuffers[i].Reset();
	}

	//if (m_pd3dDepthStencilBuffer) { m_pd3dDepthStencilBuffer.Reset(); }
	//if (m_pd3dShadowDepthBuffer) { m_pd3dShadowDepthBuffer.Reset(); }

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	hResult = m_pdxgiSwapChain->GetDesc(&swapChainDesc);

	hResult = m_pdxgiSwapChain->ResizeBuffers(N_SWAPCHAINBUFFERS, m_nWndClientWidth,
		m_nWndClientHeight, swapChainDesc.BufferDesc.Format, swapChainDesc.Flags);

	m_pDrawManager->SetSwapChainBufferIndex(0);

	CreateSwapChainRenderTargetViews();
	CreateRenderTargetViews();
	CreateDepthStencilView();
	//CreatePostprocessShader();

	ExecuteCommandList();
}

void CreateManager::ChangeScreenMode()
{
	HRESULT hResult;
	BOOL fullScreenState = FALSE;

	hResult = m_pdxgiSwapChain->GetFullscreenState(&fullScreenState, NULL);

	if (!fullScreenState)
	{
		DXGI_MODE_DESC targetParameters;
		targetParameters.Format = m_dxgiBackBufferFormat;
		targetParameters.Width = m_nWndClientWidth;
		targetParameters.Height = m_nWndClientHeight;
		targetParameters.RefreshRate.Numerator = 60;
		targetParameters.RefreshRate.Denominator = 1;
		targetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		targetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

		hResult = m_pdxgiSwapChain->ResizeTarget(&targetParameters);
	}
	hResult = m_pdxgiSwapChain->SetFullscreenState(!fullScreenState, NULL);

	m_pDrawManager->WaitForGpuComplete();
	OnResizeBackBuffers();
	m_pDrawManager->WaitForGpuComplete();
}

D3D12_HEAP_PROPERTIES CreateBufferHeapProperties(D3D12_HEAP_TYPE heapType)
{
	D3D12_HEAP_PROPERTIES heapPropertiesDesc;
	::ZeroMemory(&heapPropertiesDesc, sizeof(D3D12_HEAP_PROPERTIES));
	heapPropertiesDesc.Type = heapType;
	heapPropertiesDesc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapPropertiesDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapPropertiesDesc.CreationNodeMask = 1;
	heapPropertiesDesc.VisibleNodeMask = 1;

	return(heapPropertiesDesc);
}

D3D12_RESOURCE_DESC CreateBufferResourceDesc(UINT nBytes)
{
	D3D12_RESOURCE_DESC resourceDesc;
	::ZeroMemory(&resourceDesc, sizeof(D3D12_RESOURCE_DESC));
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Alignment = 0;
	resourceDesc.Width = nBytes;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	return(resourceDesc);
}

D3D12_RESOURCE_STATES CreateBufferInitialStates(D3D12_HEAP_TYPE heapType)
{
	if (heapType == D3D12_HEAP_TYPE_UPLOAD)
		return D3D12_RESOURCE_STATE_GENERIC_READ;

	if (heapType == D3D12_HEAP_TYPE_READBACK)
		return D3D12_RESOURCE_STATE_COPY_DEST;

	return D3D12_RESOURCE_STATE_COPY_DEST;
}

void CreateManager::ResetCommandList()
{
	m_pDrawManager->WaitForGpuComplete();
	m_pDrawManager->ResetCommandList();
}

void CreateManager::ExecuteCommandList()
{
	//HRESULT hResult = m_pd3dCommandList->Close();
	//
	//ID3D12CommandList *CommandLists[] = { m_pd3dCommandList.Get() };
	//m_pd3dCommandQueue->ExecuteCommandLists(1, CommandLists);

	m_pDrawManager->ExecuteCommandList();
}


void CreateManager::CreateDirect3dDevice()
{
	HRESULT hResult;
	DWORD debugFactoryFlags{ NULL };

#if defined(_DEBUG) 
	D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void **)&m_pd3dDebugController);
	m_pd3dDebugController->EnableDebugLayer();
#endif

	::CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void **)&m_pdxgiFactory); //DXGI 팩토리를 생성한다.

	const char factoryName[]{ "m_pFactory" };
	//모든 하드웨어 어댑터 대하여 특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성한다.
	IDXGIAdapter1 *pd3dAdapter = NULL;
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_pdxgiFactory->EnumAdapters1(i, &pd3dAdapter); i++)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc; pd3dAdapter->GetDesc1(&dxgiAdapterDesc);
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		if (SUCCEEDED(D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), (void **)&m_pd3dDevice))) break;
	}//모든 하드웨어 어댑터 대하여 특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성한다.
	if (!pd3dAdapter)
	{
		m_pdxgiFactory->EnumWarpAdapter(_uuidof(IDXGIFactory4), (void **)&pd3dAdapter);
		D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), (void **)&m_pd3dDevice);
	}//특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성할 수 없으면 WARP 디바이스를 생성한다.
	
	m_pd3dDevice->SetName(L"d3dDevice");

	// MSAA Check
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4; //Msaa4x 다중 샘플링
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE; d3dMsaaQualityLevels.NumQualityLevels = 0;
	m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels; //디바이스가 지원하는 다중 샘플의 품질 수준을 확인.
	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false; //다중 샘플의 품질 수준이 1보다 크면 다중 샘플링을 활성화. 

	m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void **)&m_pd3dFence);

	hResult = m_pd3dFence->SetName(L"m_pFence");

	m_pDrawManager->SetFence(m_pd3dFence);

	m_nRtvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_cbvSrvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	m_nDsvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	hResult = m_pd3dDevice->SetName(L"m_pDevice");

	if (pd3dAdapter) pd3dAdapter->Release();
}

void CreateManager::CreateCommandQueueAndList()
{
	HRESULT hResult;

	// Create Queue
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	hResult = m_pd3dDevice->CreateCommandQueue(&d3dCommandQueueDesc, 
		_uuidof(ID3D12CommandQueue), (void **)&m_pd3dCommandQueue); 

	hResult = m_pd3dCommandQueue->SetName(L"m_pCommandQueue");

	m_pDrawManager->SetCommandQueue(m_pd3dCommandQueue);

	hResult = m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, 
		__uuidof(ID3D12CommandAllocator), (void **)&m_pd3dCommandAllocator); 

	hResult = m_pd3dCommandAllocator->SetName(L"m_pCommandAllocator");

	m_pDrawManager->SetCommandAllocator(m_pd3dCommandAllocator);

	hResult = m_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, 
		m_pd3dCommandAllocator.Get(), NULL, __uuidof(ID3D12GraphicsCommandList), 
		(void **)&m_pd3dCommandList); 

	hResult = m_pd3dCommandList->SetName(L"m_pCommandList");

	m_pDrawManager->SetCommandList(m_pd3dCommandList);

	hResult = m_pd3dCommandList->Close();
}

void CreateManager::CreateSwapChain()
{
	// Create Swap Chain
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	dxgiSwapChainDesc.BufferCount = N_SWAPCHAINBUFFERS;
	dxgiSwapChainDesc.BufferDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.BufferDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.BufferDesc.Format = m_dxgiBackBufferFormat;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.OutputWindow = m_hWnd;
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.Windowed = TRUE;
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;


	HRESULT hResult = m_pdxgiFactory->CreateSwapChain(m_pd3dCommandQueue.Get(),
		&dxgiSwapChainDesc, (IDXGISwapChain * *)m_pdxgiSwapChain.GetAddressOf());

	m_pDrawManager->SetSwapChain(m_pdxgiSwapChain);

	hResult = m_pdxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);
	//스왑체인의 현재 후면버퍼 인덱스를 저장한다.
	m_pDrawManager->SetSwapChainBufferIndex(m_pdxgiSwapChain->GetCurrentBackBufferIndex());
}

void CreateManager::CreateRtvAndDsvDescriptorHeaps()
{
	HRESULT hResult;

	// Create Render Target View Descriptor Heap
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors = N_SWAPCHAINBUFFERS + N_RENDERTARGETBUFFERS;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask = 0;

	hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc,
		__uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dRtvDescriptorHeap);

	hResult = m_pd3dRtvDescriptorHeap->SetName(L"m_pRtvDescriptorHeap");

	d3dDescriptorHeapDesc.NumDescriptors = 2;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc,
		__uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dDsvDescriptorHeap);
	//깊이-스텐실 서술자 힙(서술자의 개수는 2,그림자 버퍼를 위한 힙 포함)을 생성한다.

	hResult = m_pd3dDsvDescriptorHeap->SetName(L"m_pDsvDescriptorHeap");
}

void CreateManager::CreateSwapChainRenderTargetViews()
{
	HRESULT hResult;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = m_dxgiBackBufferFormat;
	renderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	renderTargetViewDesc.Texture2D.PlaneSlice = 0;

	for (UINT i = 0; i < N_SWAPCHAINBUFFERS; ++i)
	{
		m_pRtvSwapChainBackBufferCPUHandles[i] = rtvCPUDescriptorHandle;
		hResult = m_pdxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void **)&m_ppd3dSwapChainBackBuffers[i]);
		
		m_pd3dDevice->CreateRenderTargetView(m_ppd3dSwapChainBackBuffers[i].Get(), &renderTargetViewDesc, m_pRtvSwapChainBackBufferCPUHandles[i]);
		rtvCPUDescriptorHandle.ptr += m_nRtvDescriptorIncrementSize;
		hResult = m_ppd3dSwapChainBackBuffers[i]->SetName(L"m_ppSwapChainBackBuffers");
	}
	m_pDrawManager->SetSwapChainBackBuffers(m_ppd3dSwapChainBackBuffers);
	m_pDrawManager->SetRtvSwapChainBackBufferCPUHandles(m_pRtvSwapChainBackBufferCPUHandles);
}

void CreateManager::CreateDepthStencilView()
{
	HRESULT hResult;

	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask = 1;
	d3dHeapProperties.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = m_nWndClientWidth;
	d3dResourceDesc.Height = m_nWndClientHeight;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality =
		(m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE d3dClearValue;
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;

	// Create Depth Stencil Buffer
	m_pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue, __uuidof(ID3D12Resource), (void **)&m_pd3dDepthStencilBuffer); //깊이-스텐실 버퍼를 생성한다.
	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer.Get(), NULL, d3dDsvCPUDescriptorHandle); //깊이-스텐실 버퍼 뷰를 생성한다.

	hResult = m_pd3dDepthStencilBuffer->SetName(L"m_pDepthStencilBuffer");

	// Create Depth Stencil Buffer For ShadowMap

	m_pd3dShadowDepthBuffer = CreateTexture2DResource(m_pd3dDevice.Get(), m_nWndClientWidth, m_nWndClientHeight, DXGI_FORMAT_R24G8_TYPELESS,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL,
		D3D12_RESOURCE_STATE_GENERIC_READ, &d3dClearValue);

	hResult = m_pd3dShadowDepthBuffer->SetName(L"m_pShadowDepthBuffer");

	// Depth DSV 설정
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.Texture2D.MipSlice = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE dsvShadowCPUDescriptorHandle{ m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };	
	dsvShadowCPUDescriptorHandle.ptr = dsvShadowCPUDescriptorHandle.ptr + m_nDsvDescriptorIncrementSize;
	
	m_pd3dDevice->CreateDepthStencilView(
		m_pd3dShadowDepthBuffer.Get(), &dsvDesc, dsvShadowCPUDescriptorHandle);

	m_pDrawManager->SetDsvCPUHandleWithDsvHeap(m_pd3dDsvDescriptorHeap, m_nDsvDescriptorIncrementSize);
	m_pDrawManager->SetShadowDepthBuffer(m_pd3dShadowDepthBuffer);
}

void CreateManager::CreateRenderTargetViews()
{
	
}

/*
void CreateManager::CreatePostprocessShader()
{
	m_pTextureToFullScreenShader = shared_ptr<CTextureToFullScreenShader>(new CTextureToFullScreenShader(shared_from_this()));
	m_pTextureToFullScreenShader->CreateGraphicsRootSignature(shared_from_this());
	m_pTextureToFullScreenShader->CreateShader(shared_from_this(), m_pTextureToFullScreenShader->GetGraphicsRootSignature());
	m_pTextureToFullScreenShader->BuildObjects(shared_from_this(), m_pTexture);
	
	m_pRenderMgr->SetTextureToFullScreenShader(m_pTextureToFullScreenShader);
}
*/
void CreateManager::CreateGraphicsRootSignature()
{
	D3D12_DESCRIPTOR_RANGE texTable;
	texTable.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	texTable.NumDescriptors = 2;
	texTable.BaseShaderRegister = 1;
	texTable.RegisterSpace = 0;
	texTable.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[3];


	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[0].NumDescriptors = 1;
	pd3dDescriptorRanges[0].BaseShaderRegister = 1; //Diffuse
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = 0;

	pd3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[1].NumDescriptors = 1;
	pd3dDescriptorRanges[1].BaseShaderRegister = 2; //Detail
	pd3dDescriptorRanges[1].RegisterSpace = 0;
	pd3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = 0;

	pd3dDescriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[2].NumDescriptors = 1;
	pd3dDescriptorRanges[2].BaseShaderRegister = 3; //ShadowMap
	pd3dDescriptorRanges[2].RegisterSpace = 0;
	pd3dDescriptorRanges[2].OffsetInDescriptorsFromTableStart = 0;

	D3D12_ROOT_PARAMETER pd3dRootParameters[12];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 0; //Camera
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[1].Descriptor.ShaderRegister = 3; //Shadow
	pd3dRootParameters[1].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[2].Constants.Num32BitValues = 32;
	pd3dRootParameters[2].Constants.ShaderRegister = 5; //인스턴싱이 아닌 물체들을 사용
	pd3dRootParameters[2].Constants.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	pd3dRootParameters[3].Descriptor.ShaderRegister = 0; //t0   인스턴싱버퍼 묶음
	pd3dRootParameters[3].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[4].Descriptor.ShaderRegister = 1; //Materials   메터리얼 버퍼
	pd3dRootParameters[4].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[5].Descriptor.ShaderRegister = 2; //Lights     라이트 버퍼
	pd3dRootParameters[5].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[6].Descriptor.ShaderRegister = 7; //Skinned Bone Offsets
	pd3dRootParameters[6].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[7].Descriptor.ShaderRegister = 8; //Skinned Bone Transforms
	pd3dRootParameters[7].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[8].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[8].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[0]; //Base Texture
	pd3dRootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[9].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[9].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[1]; //Detail Texture
	pd3dRootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[10].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[10].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[2]; //Shadow map
	pd3dRootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[11].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[11].Descriptor.ShaderRegister = 6; //Particle
	pd3dRootParameters[11].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[11].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	auto staticSamplers = GetStaticSamplers();

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = staticSamplers.size();
	d3dRootSignatureDesc.pStaticSamplers = staticSamplers.data();
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob *pd3dSignatureBlob = NULL;
	ID3DBlob *pd3dErrorBlob = NULL;

	HRESULT h = D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		&pd3dSignatureBlob, &pd3dErrorBlob);

	m_pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(),
		pd3dSignatureBlob->GetBufferSize(), IID_PPV_ARGS(m_pGraphicsRootSignature.GetAddressOf()));

	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	m_pGraphicsRootSignature->SetName(L"m_pGraphicsRootSignature");

	m_pDrawManager->SaveGraphicsRootSignature(m_pGraphicsRootSignature);
}
void CreateManager::CreateComputeRootSignature()
{
	D3D12_DESCRIPTOR_RANGE srvTable;
	srvTable.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvTable.NumDescriptors = 1;
	srvTable.BaseShaderRegister = 0;
	srvTable.RegisterSpace = 0;
	srvTable.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_DESCRIPTOR_RANGE uavTable1;
	uavTable1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	uavTable1.NumDescriptors = 1;
	uavTable1.BaseShaderRegister = 0;
	uavTable1.RegisterSpace = 0;
	uavTable1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_DESCRIPTOR_RANGE uavTable2;
	uavTable2.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	uavTable2.NumDescriptors = 1;
	uavTable2.BaseShaderRegister = 1;
	uavTable2.RegisterSpace = 0;
	uavTable2.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER pd3dRootParameters[7];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[0].Constants.Num32BitValues = 12;
	pd3dRootParameters[0].Constants.ShaderRegister = 0;
	pd3dRootParameters[0].Constants.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[1].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[1].DescriptorTable.pDescriptorRanges = &srvTable;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[2].DescriptorTable.pDescriptorRanges = &uavTable1;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[3].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[3].DescriptorTable.pDescriptorRanges = &uavTable2;
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	pd3dRootParameters[4].Descriptor.ShaderRegister = 1; //t1   인스턴싱버퍼 묶음
	pd3dRootParameters[4].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
	pd3dRootParameters[5].Descriptor.ShaderRegister = 2; //u2   인스턴싱버퍼 묶음
	pd3dRootParameters[5].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[6].Descriptor.ShaderRegister = 1; // particle structure
	pd3dRootParameters[6].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = nullptr;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob *pd3dSignatureBlob = NULL;
	ID3DBlob *pd3dErrorBlob = NULL;

	HRESULT h = D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		&pd3dSignatureBlob, &pd3dErrorBlob);

	m_pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(),
		pd3dSignatureBlob->GetBufferSize(), IID_PPV_ARGS(m_pComputeRootSignature.GetAddressOf()));

	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	m_pComputeRootSignature->SetName(L"m_pComputeRootSignature");

	m_pDrawManager->SaveComputeRootSignature(m_pComputeRootSignature);
}
//정적 샘플러
array<const D3D12_STATIC_SAMPLER_DESC, 7> GetStaticSamplers()
{
	const D3D12_STATIC_SAMPLER_DESC pointWrap{ D3D12_FILTER_MIN_MAG_MIP_POINT,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		0, 16, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE
		, 0.f, D3D12_FLOAT32_MAX, 0, 0, D3D12_SHADER_VISIBILITY_ALL };

	const D3D12_STATIC_SAMPLER_DESC pointClamp{ D3D12_FILTER_MIN_MAG_MIP_POINT,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		0, 16, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE
		, 0.f, D3D12_FLOAT32_MAX, 1, 0, D3D12_SHADER_VISIBILITY_ALL };

	const D3D12_STATIC_SAMPLER_DESC linearWrap{ D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		0, 16, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE
		, 0.f, D3D12_FLOAT32_MAX, 2, 0, D3D12_SHADER_VISIBILITY_ALL };

	const D3D12_STATIC_SAMPLER_DESC linearClamp{ D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		0, 16, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE
		, 0.f, D3D12_FLOAT32_MAX, 3, 0, D3D12_SHADER_VISIBILITY_ALL };

	const D3D12_STATIC_SAMPLER_DESC anisotropicWrap
	{ D3D12_FILTER_ANISOTROPIC,
	D3D12_TEXTURE_ADDRESS_MODE_WRAP,
	D3D12_TEXTURE_ADDRESS_MODE_WRAP,
	D3D12_TEXTURE_ADDRESS_MODE_WRAP,
	0.0f, 8, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE
	, 0.f, D3D12_FLOAT32_MAX, 4, 0, D3D12_SHADER_VISIBILITY_ALL };

	const D3D12_STATIC_SAMPLER_DESC anisotropicClamp
	{ D3D12_FILTER_ANISOTROPIC,
	D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
	D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
	D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
	0.0f, 8, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE
	, 0.f, D3D12_FLOAT32_MAX, 5, 0, D3D12_SHADER_VISIBILITY_ALL
	};
	const D3D12_STATIC_SAMPLER_DESC shadow{
		D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressW
		0.0f,                               // mipLODBias
		16,                                 // maxAnisotropy
		D3D12_COMPARISON_FUNC_LESS_EQUAL,
		D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE, 0.f, D3D12_FLOAT32_MAX, 6, 0, D3D12_SHADER_VISIBILITY_ALL };


	return {
		pointWrap, pointClamp,
		linearWrap, linearClamp,
		anisotropicWrap, anisotropicClamp,shadow };
}