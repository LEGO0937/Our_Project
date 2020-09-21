#include "GameManager.h"
#include "../Scenes/BaseScene.h"

GameManager::GameManager()
{
}

GameManager::~GameManager()
{
	Release();
	::CloseHandle(m_hFenceEvent);
}

bool GameManager::Initialize(HINSTANCE hInstance, HWND hWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hWnd;

	CreateDirect3dDevice();
	CreateCommandQueueAndList();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();
	CreateGraphicsRootSignature();
	CreateComputeRootSignature();

	// 윈도우 콜백으로 wm_size받을때 리사이즈함수호출하도록 해야함/
	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	m_nFenceValue[0] = 1;

	return true;
}

void GameManager::Release()
{
	HRESULT hResult;
	WaitForGpuComplete();

	hResult = m_pDxgiSwapChain->SetFullscreenState(FALSE, NULL);

	m_pD3dCommandQueue.Reset();
	m_pD3dCommandAllocator.Reset();
	m_pD3dCommandList.Reset();

	m_pD3dRtvDescriptorHeap.Reset();
	m_pD3dDsvDescriptorHeap.Reset();

	m_pD3dDepthStencilBuffer.Reset();
	m_pD3dShadowDepthBuffer.Reset();

	for (int i = 0; i < nRendertargetBuffers; ++i)
	{
		m_ppD3dPostProcessingBuffers[i].Reset();
	}

	for (int i = 0; i < nSwapchainBuffers; ++i)
	{
		m_ppD3dSwapChainBuffers[i].Reset();
	}

	m_pGraphicsRootSignature.Reset();

	m_pD3dFence.Reset();

	m_pDxgiSwapChain.Reset();
	m_pD3dDevice.Reset();
	m_pDxgiFactory.Reset();

	m_pLoadingScene.reset();
}

void GameManager::ChangeResourceState(ComPtr<ID3D12Resource> pResource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter)
{
	D3D12_RESOURCE_BARRIER resourceBarrier;
	resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	resourceBarrier.Transition.pResource = pResource.Get();
	resourceBarrier.Transition.StateBefore = stateBefore;
	resourceBarrier.Transition.StateAfter = stateAfter;
	resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pD3dCommandList->ResourceBarrier(1, &resourceBarrier);
}
void GameManager::RenderLoading()
{
	RenderLoading(m_pLoadingScene);
}

void GameManager::Render(shared_ptr<class BaseScene> pScene, float fTimeElapsed)
{
	RenderShadow(pScene);   // ShadowDraw
	RenderDefault(pScene, fTimeElapsed);   // BaseDraw
	RenderPostProcess(pScene);
	MoveToNextFrame();
}

void GameManager::RenderShadow(shared_ptr<class BaseScene> pScene)
{
	HRESULT hResult;
	WaitForGpuComplete();
	hResult = m_pD3dCommandList->Reset(m_pD3dCommandAllocator.Get(), NULL);

	m_pD3dCommandList->SetGraphicsRootSignature(m_pGraphicsRootSignature.Get());
	m_pD3dCommandList->SetComputeRootSignature(m_pComputeRootSignature.Get());
	//그림자 버퍼 상태 변경
	ChangeResourceState(m_pD3dShadowDepthBuffer, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE);

	m_pD3dCommandList->ClearDepthStencilView(m_dsvShadowBufferCPUHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	//랜더타겟은 Null이므로 그림자버퍼에만 기록이 됨.
	m_pD3dCommandList->OMSetRenderTargets(0, NULL, FALSE, &m_dsvShadowBufferCPUHandle);

	pScene->RenderShadow();

	ChangeResourceState(m_pD3dShadowDepthBuffer, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ);
	//------------------------------

	//속도맵 기록, 때문에 랜더타넷은 스왑체인의 버퍼가 아닌 랜더타겟버퍼로 사용
	ChangeResourceState(m_ppD3dPostProcessingBuffers[0], D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET);

	m_pD3dCommandList->ClearRenderTargetView(m_pRtvPostProcessingBufferCPUHandles[0], Colors::Black, 0, NULL);
	m_pD3dCommandList->ClearDepthStencilView(m_dsvDepthStencilBufferCPUHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	m_pD3dCommandList->OMSetRenderTargets(1, &m_pRtvPostProcessingBufferCPUHandles[0], TRUE, &m_dsvDepthStencilBufferCPUHandle);

	pScene->RenderVelocity();

	ChangeResourceState(m_ppD3dPostProcessingBuffers[0], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ);


	ExecuteCommandList();
}

void GameManager::RenderDefault(shared_ptr<class BaseScene> pScene, float fTimeElapsed)
{
	m_pD3dCommandList->Reset(m_pD3dCommandAllocator.Get(), NULL);

	m_pD3dCommandList->SetGraphicsRootSignature(m_pGraphicsRootSignature.Get());
	m_pD3dCommandList->SetComputeRootSignature(m_pComputeRootSignature.Get());

	ChangeResourceState(m_ppD3dSwapChainBuffers[m_nSwapChainBufferIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	m_pD3dCommandList->ClearDepthStencilView(m_dsvDepthStencilBufferCPUHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	m_pD3dCommandList->ClearRenderTargetView(m_pRtvSwapChainBufferCPUHandles[m_nSwapChainBufferIndex], Colors::LightGray, 0, NULL);
	m_pD3dCommandList->OMSetRenderTargets(1, &m_pRtvSwapChainBufferCPUHandles[m_nSwapChainBufferIndex],
		TRUE, &m_dsvDepthStencilBufferCPUHandle);

	if (pScene)
		pScene->Render();
}

void GameManager::RenderPostProcess(shared_ptr<class BaseScene> pScene)
{
	pScene->RenderPostProcess(m_ppD3dSwapChainBuffers[m_nSwapChainBufferIndex], m_ppD3dPostProcessingBuffers[0]);

	ChangeResourceState(m_ppD3dSwapChainBuffers[m_nSwapChainBufferIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	ExecuteCommandList();

	HRESULT hResult = m_pDxgiSwapChain->Present(0, 0);
}

void GameManager::RenderLoading(shared_ptr<class BaseScene> pScene)
{
	HRESULT hResult;
	m_pD3dCommandList->SetGraphicsRootSignature(m_pGraphicsRootSignature.Get());
	m_pD3dCommandList->SetComputeRootSignature(m_pComputeRootSignature.Get());

	ChangeResourceState(m_ppD3dSwapChainBuffers[m_nSwapChainBufferIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	m_pD3dCommandList->ClearDepthStencilView(m_dsvDepthStencilBufferCPUHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	m_pD3dCommandList->ClearRenderTargetView(m_pRtvSwapChainBufferCPUHandles[m_nSwapChainBufferIndex], Colors::Aqua, 0, NULL);
	m_pD3dCommandList->OMSetRenderTargets(1, &m_pRtvSwapChainBufferCPUHandles[m_nSwapChainBufferIndex],
		TRUE, &m_dsvDepthStencilBufferCPUHandle);

	if (pScene)
		pScene->Render();

	ChangeResourceState(m_ppD3dSwapChainBuffers[m_nSwapChainBufferIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	ExecuteCommandList();

	hResult = m_pDxgiSwapChain->Present(0, 0);
	MoveToNextFrame();

	hResult = m_pD3dCommandList->Reset(m_pD3dCommandAllocator.Get(), NULL);
}

void GameManager::ResetSize(int width, int height)
{
	m_nWndClientWidth = width;
	m_nWndClientHeight = height;

	OnResizeBackBuffers();
}

void GameManager::OnResizeBackBuffers()
{
	HRESULT hResult;

	ResetCommandList();

	for (int i = 0; i < nRendertargetBuffers; i++)
	{
		m_ppD3dPostProcessingBuffers[i].Reset();
	}

	for (int i = 0; i < nSwapchainBuffers; i++)
	{
		m_ppD3dSwapChainBuffers[i].Reset();
	}

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	hResult = m_pDxgiSwapChain->GetDesc(&swapChainDesc);

	hResult = m_pDxgiSwapChain->ResizeBuffers(nSwapchainBuffers, m_nWndClientWidth,
		m_nWndClientHeight, swapChainDesc.BufferDesc.Format, swapChainDesc.Flags);

	m_nSwapChainBufferIndex = 0;
	
	CreateSwapChainRenderTargetViews();
	CreatePostProcessingRenderTargetViews();
	CreateDepthStencilView();

	ExecuteCommandList();
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

void GameManager::MoveToNextFrame()
{
	m_nSwapChainBufferIndex = m_pDxgiSwapChain->GetCurrentBackBufferIndex();
}

void GameManager::ResetCommandAllocator()
{
	HRESULT hResult;
	hResult = m_pD3dCommandAllocator->Reset();
}

void GameManager::WaitForGpuComplete()
{
	HRESULT hResult;
	++m_nFenceValue[m_nSwapChainBufferIndex];
	UINT64 fenceValue = m_nFenceValue[m_nSwapChainBufferIndex];

	hResult = m_pD3dCommandQueue->Signal(m_pD3dFence.Get(), fenceValue);

	//GPU가 펜스의 값을 설정하는 명령을 명령 큐에 추가한다.
	if (m_pD3dFence->GetCompletedValue() < fenceValue)
	{
		//펜스의 현재 값이 설정한 값보다 작으면 펜스의 현재 값이 설정한 값이 될 때까지 기다린다.
		hResult = m_pD3dFence->SetEventOnCompletion(fenceValue, m_hFenceEvent);

		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void GameManager::ResetCommandList()
{
	WaitForGpuComplete();
	m_pD3dCommandList->Reset(m_pD3dCommandAllocator.Get(), NULL);
}

void GameManager::ExecuteCommandList()
{
	HRESULT hResult;

	hResult = m_pD3dCommandList->Close();

	ID3D12CommandList* ppCommandLists[] = { m_pD3dCommandList.Get() };
	m_pD3dCommandQueue->ExecuteCommandLists(1, ppCommandLists);

	WaitForGpuComplete();
}


void GameManager::CreateDirect3dDevice()
{
	HRESULT hResult;
	DWORD debugFactoryFlags{ NULL };

#if defined(_DEBUG) 
	D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void**)&m_pd3dDebugController);
	m_pd3dDebugController->EnableDebugLayer();
#endif

	::CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&m_pDxgiFactory); //DXGI 팩토리를 생성한다.

	const char factoryName[]{ "Factory" };
	//모든 하드웨어 어댑터 대하여 특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성한다.
	IDXGIAdapter1* pd3dAdapter = NULL;
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_pDxgiFactory->EnumAdapters1(i, &pd3dAdapter); i++)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc; pd3dAdapter->GetDesc1(&dxgiAdapterDesc);
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		if (SUCCEEDED(D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), (void**)&m_pD3dDevice))) break;
	}//모든 하드웨어 어댑터 대하여 특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성한다.
	if (!pd3dAdapter)
	{
		m_pDxgiFactory->EnumWarpAdapter(_uuidof(IDXGIFactory4), (void**)&pd3dAdapter);
		D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), (void**)&m_pD3dDevice);
	}//특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성할 수 없으면 WARP 디바이스를 생성한다.

	m_pD3dDevice->SetName(L"d3dDevice");

	// MSAA Check
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4; //Msaa4x 다중 샘플링
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE; d3dMsaaQualityLevels.NumQualityLevels = 0;
	m_pD3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels; //디바이스가 지원하는 다중 샘플의 품질 수준을 확인.
	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false; //다중 샘플의 품질 수준이 1보다 크면 다중 샘플링을 활성화. 

	m_pD3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&m_pD3dFence);

	hResult = m_pD3dFence->SetName(L"Fence");

	m_nRtvDescriptorIncrementSize = m_pD3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_cbvSrvDescriptorIncrementSize = m_pD3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	m_nDsvDescriptorIncrementSize = m_pD3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	if (pd3dAdapter) pd3dAdapter->Release();
}

void GameManager::CreateCommandQueueAndList()
{
	HRESULT hResult;

	// Create Queue
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	hResult = m_pD3dDevice->CreateCommandQueue(&d3dCommandQueueDesc,
		_uuidof(ID3D12CommandQueue), (void**)&m_pD3dCommandQueue);

	hResult = m_pD3dCommandQueue->SetName(L"CommandQueue");

	hResult = m_pD3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		__uuidof(ID3D12CommandAllocator), (void**)&m_pD3dCommandAllocator);

	hResult = m_pD3dCommandAllocator->SetName(L"CommandAllocator");

	hResult = m_pD3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_pD3dCommandAllocator.Get(), NULL, __uuidof(ID3D12GraphicsCommandList),
		(void**)&m_pD3dCommandList);

	hResult = m_pD3dCommandList->SetName(L"CommandList");

	hResult = m_pD3dCommandList->Close();
}

void GameManager::CreateSwapChain()
{
	// Create Swap Chain
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	m_nWndClientWidth = rcClient.right - rcClient.left;
	m_nWndClientHeight = rcClient.bottom - rcClient.top;

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	dxgiSwapChainDesc.BufferCount = nSwapchainBuffers;
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


	HRESULT hResult = m_pDxgiFactory->CreateSwapChain(m_pD3dCommandQueue.Get(),
		&dxgiSwapChainDesc, (IDXGISwapChain**)m_pDxgiSwapChain.GetAddressOf());

	//hResult = m_pdxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);
	//스왑체인의 현재 후면버퍼 인덱스를 저장한다.
	m_nSwapChainBufferIndex = m_pDxgiSwapChain->GetCurrentBackBufferIndex();
}

void GameManager::CreateRtvAndDsvDescriptorHeaps()
{
	HRESULT hResult;

	// Create Render Target View Descriptor Heap
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors = nSwapchainBuffers + nRendertargetBuffers;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask = 0;

	hResult = m_pD3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc,
		__uuidof(ID3D12DescriptorHeap), (void**)&m_pD3dRtvDescriptorHeap);

	hResult = m_pD3dRtvDescriptorHeap->SetName(L"RtvDescriptorHeap");

	d3dDescriptorHeapDesc.NumDescriptors = 2;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	hResult = m_pD3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc,
		__uuidof(ID3D12DescriptorHeap), (void**)&m_pD3dDsvDescriptorHeap);
	//깊이-스텐실 서술자 힙(서술자의 개수는 2,그림자 버퍼를 위한 힙 포함)을 생성한다.

	hResult = m_pD3dDsvDescriptorHeap->SetName(L"DsvDescriptorHeap");
}

void GameManager::CreateSwapChainRenderTargetViews()
{
	HRESULT hResult;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvCPUDescriptorHandle = m_pD3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = m_dxgiBackBufferFormat;
	renderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	renderTargetViewDesc.Texture2D.PlaneSlice = 0;

	for (UINT i = 0; i < nSwapchainBuffers; ++i)
	{
		m_pRtvSwapChainBufferCPUHandles[i] = rtvCPUDescriptorHandle;
		hResult = m_pDxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&m_ppD3dSwapChainBuffers[i]);

		m_pD3dDevice->CreateRenderTargetView(m_ppD3dSwapChainBuffers[i].Get(), &renderTargetViewDesc, m_pRtvSwapChainBufferCPUHandles[i]);
		rtvCPUDescriptorHandle.ptr += m_nRtvDescriptorIncrementSize;
		hResult = m_ppD3dSwapChainBuffers[i]->SetName(L"SwapChainBackBuffer");
	}
}

void GameManager::CreateDepthStencilView()
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

	// Depth Stencil버퍼 생성
	m_pD3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue, __uuidof(ID3D12Resource), (void**)&m_pD3dDepthStencilBuffer); //깊이-스텐실 버퍼를 생성한다.
	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pD3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pD3dDevice->CreateDepthStencilView(m_pD3dDepthStencilBuffer.Get(), NULL, d3dDsvCPUDescriptorHandle); //깊이-스텐실 버퍼 뷰를 생성한다.

	hResult = m_pD3dDepthStencilBuffer->SetName(L"DepthStencilBuffer");

	// 그림자버퍼 생성

	m_pD3dShadowDepthBuffer = CreateTexture2DResource(m_pD3dDevice.Get(), m_nWndClientWidth, m_nWndClientHeight, DXGI_FORMAT_R24G8_TYPELESS,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL,
		D3D12_RESOURCE_STATE_GENERIC_READ, &d3dClearValue);

	hResult = m_pD3dShadowDepthBuffer->SetName(L"ShadowDepthBuffer");

	// Depth DSV 설정
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.Texture2D.MipSlice = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE dsvShadowCPUDescriptorHandle{ m_pD3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
	dsvShadowCPUDescriptorHandle.ptr = dsvShadowCPUDescriptorHandle.ptr + m_nDsvDescriptorIncrementSize;

	m_pD3dDevice->CreateDepthStencilView(
		m_pD3dShadowDepthBuffer.Get(), &dsvDesc, dsvShadowCPUDescriptorHandle);

	m_dsvDepthStencilBufferCPUHandle = m_pD3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_dsvShadowBufferCPUHandle.ptr = m_dsvDepthStencilBufferCPUHandle.ptr + m_nDsvDescriptorIncrementSize;
}

void GameManager::CreatePostProcessingRenderTargetViews()
{
	//속도맵을 위한 리소스 생성
	HRESULT hResult;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvCPUDescriptorHandle = m_pD3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	rtvCPUDescriptorHandle.ptr += (nSwapchainBuffers * m_nRtvDescriptorIncrementSize);  //이미 스왑체인 버퍼를 생성했다는 전제하에 실행

	D3D12_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = m_dxgiPostProcessingBufferFormat;
	renderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	renderTargetViewDesc.Texture2D.PlaneSlice = 0;

	D3D12_CLEAR_VALUE clearValue = { m_dxgiPostProcessingBufferFormat,{ 0.0f, 0.0f, 0.0f, 1.0f } };

	for (UINT i = 0; i < nRendertargetBuffers; ++i)
	{
		m_ppD3dPostProcessingBuffers[i] = CreateTexture2DResource(m_pD3dDevice.Get(), m_nWndClientWidth, m_nWndClientHeight, m_dxgiPostProcessingBufferFormat, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
			D3D12_RESOURCE_STATE_GENERIC_READ, &clearValue);
		hResult = m_ppD3dPostProcessingBuffers[i]->SetName(L"RenderTargetBuffer");

		m_pRtvPostProcessingBufferCPUHandles[i] = rtvCPUDescriptorHandle;

		m_pD3dDevice->CreateRenderTargetView(m_ppD3dPostProcessingBuffers[i].Get(), &renderTargetViewDesc, m_pRtvPostProcessingBufferCPUHandles[i]);
		rtvCPUDescriptorHandle.ptr += m_nRtvDescriptorIncrementSize;
	}
}

void GameManager::CreateGraphicsRootSignature()
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
	pd3dDescriptorRanges[0].BaseShaderRegister = 2; //Diffuse
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = 0;

	pd3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[1].NumDescriptors = 1;
	pd3dDescriptorRanges[1].BaseShaderRegister = 3; //Detail
	pd3dDescriptorRanges[1].RegisterSpace = 0;
	pd3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = 0;

	pd3dDescriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[2].NumDescriptors = 1;
	pd3dDescriptorRanges[2].BaseShaderRegister = 4; //ShadowMap
	pd3dDescriptorRanges[2].RegisterSpace = 0;
	pd3dDescriptorRanges[2].OffsetInDescriptorsFromTableStart = 0;

	D3D12_ROOT_PARAMETER pd3dRootParameters[13];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 0; //Camera
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[1].Descriptor.ShaderRegister = 3; //Shadow
	pd3dRootParameters[1].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 5; //Particle or prevMatirx
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
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
	pd3dRootParameters[11].Descriptor.ShaderRegister = 6; //Particle or prevMatirx
	pd3dRootParameters[11].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[11].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[12].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	pd3dRootParameters[12].Descriptor.ShaderRegister = 1; //t1   Prev인스턴싱버퍼 묶음
	pd3dRootParameters[12].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[12].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
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

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;

	HRESULT h = D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		&pd3dSignatureBlob, &pd3dErrorBlob);

	h = m_pD3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(),
		pd3dSignatureBlob->GetBufferSize(), IID_PPV_ARGS(m_pGraphicsRootSignature.GetAddressOf()));

	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	m_pGraphicsRootSignature->SetName(L"m_pGraphicsRootSignature");
}
void GameManager::CreateComputeRootSignature()
{
	D3D12_DESCRIPTOR_RANGE srvTable;
	srvTable.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvTable.NumDescriptors = 1;
	srvTable.BaseShaderRegister = 2;
	srvTable.RegisterSpace = 0;
	srvTable.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_DESCRIPTOR_RANGE srvTable1;
	srvTable1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvTable1.NumDescriptors = 1;
	srvTable1.BaseShaderRegister = 3;
	srvTable1.RegisterSpace = 0;
	srvTable1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

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

	D3D12_ROOT_PARAMETER pd3dRootParameters[8];

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

	pd3dRootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[7].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[7].DescriptorTable.pDescriptorRanges = &srvTable1;
	pd3dRootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = nullptr;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;

	HRESULT h = D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		&pd3dSignatureBlob, &pd3dErrorBlob);

	m_pD3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(),
		pd3dSignatureBlob->GetBufferSize(), IID_PPV_ARGS(m_pComputeRootSignature.GetAddressOf()));

	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	m_pComputeRootSignature->SetName(L"m_pComputeRootSignature");
}

array<const D3D12_STATIC_SAMPLER_DESC, 5> GetStaticSamplers()
{
	const D3D12_STATIC_SAMPLER_DESC linearWrap{ D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		0, 16, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE
		, 0.f, D3D12_FLOAT32_MAX, 0, 0, D3D12_SHADER_VISIBILITY_ALL };

	const D3D12_STATIC_SAMPLER_DESC linearClamp{ D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		0, 16, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE
		, 0.f, D3D12_FLOAT32_MAX, 1, 0, D3D12_SHADER_VISIBILITY_ALL };

	const D3D12_STATIC_SAMPLER_DESC anisotropicWrap
	{ D3D12_FILTER_ANISOTROPIC,
	D3D12_TEXTURE_ADDRESS_MODE_WRAP,
	D3D12_TEXTURE_ADDRESS_MODE_WRAP,
	D3D12_TEXTURE_ADDRESS_MODE_WRAP,
	0.0f, 8, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE
	, 0.f, D3D12_FLOAT32_MAX, 2, 0, D3D12_SHADER_VISIBILITY_ALL };

	const D3D12_STATIC_SAMPLER_DESC anisotropicClamp
	{ D3D12_FILTER_ANISOTROPIC,
	D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
	D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
	D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
	0.0f, 8, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE
	, 0.f, D3D12_FLOAT32_MAX, 3, 0, D3D12_SHADER_VISIBILITY_ALL
	};
	const D3D12_STATIC_SAMPLER_DESC shadow{
		D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,
		0.0f,
		16,
		D3D12_COMPARISON_FUNC_LESS_EQUAL,
		D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE, 0.f, D3D12_FLOAT32_MAX, 4, 0, D3D12_SHADER_VISIBILITY_ALL };


	return {
		linearWrap, linearClamp,
		anisotropicWrap, anisotropicClamp,shadow };
}