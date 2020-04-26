#pragma once
#include "../stdafx.h"
#include "DrawManager.h"
#include "FontManager.h"
class CreateManager : public enable_shared_from_this< CreateManager>
{

public:
	CreateManager();
	~CreateManager();

public:
	void Initialize(HINSTANCE hInstance, HWND hWnd);
	void Release();

	void RenderLoadingScreen(float loadingPercentage = 0.f);

	void Resize(int width, int height);

	void OnResizeBackBuffers();
	void ChangeScreenMode();

	void ResetCommandList();
	void ExecuteCommandList();

	shared_ptr<DrawManager> GetDrawMgr() { return m_pDrawManager; }
	shared_ptr<CreateManager> getThisSharedPtr() {return shared_from_this();}

	ComPtr<ID3D12Device> GetDevice() { return m_pd3dDevice; }
	ComPtr<ID3D12GraphicsCommandList> GetCommandList() { return m_pd3dCommandList; }

	ComPtr<ID3D12RootSignature> GetGraphicsRootSignature() { return m_pGraphicsRootSignature; }
	ComPtr<ID3D12RootSignature> GetComputeRootSignature() { return m_pComputeRootSignature; }

	HWND GetHwnd() { return m_hWnd; }

	int GetWindowWidth() { return m_nWndClientWidth; }
	int GetWindowHeight() { return m_nWndClientHeight; }

	UINT GetCbvSrvDescriptorIncrementSize() { return m_cbvSrvDescriptorIncrementSize; }

	ComPtr<ID3D12Resource> GetShadowBuffer() { return m_pd3dShadowDepthBuffer; }
	
private:  //d3d ComObject
	void CreateDirect3dDevice();
	void CreateCommandQueueAndList();
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateSwapChainRenderTargetViews();
	void CreateRenderTargetViews();
	void CreateDepthStencilView();
	//void CreatePostprocessShader();
	void CreateGraphicsRootSignature();
	void CreateComputeRootSignature();

private:
	HINSTANCE m_hInstance;
	HWND m_hWnd;

#if defined(_DEBUG)
	ID3D12Debug *m_pd3dDebugController;
#endif

	// Window Size
	int m_nWndClientWidth{ FRAME_BUFFER_WIDTH };
	int m_nWndClientHeight{ FRAME_BUFFER_HEIGHT };

	ComPtr<IDXGIFactory4> m_pdxgiFactory; 
	ComPtr<ID3D12Device> m_pd3dDevice; 

	ComPtr<IDXGISwapChain3> m_pdxgiSwapChain; 

	DXGI_FORMAT m_dxgiBackBufferFormat{ DXGI_FORMAT_R8G8B8A8_UNORM };
	DXGI_FORMAT m_dxgiRenderBufferFormat{ DXGI_FORMAT_R32G32B32A32_FLOAT };

	bool m_bMsaa4xEnable = false;
	UINT m_nMsaa4xQualityLevels = 0; //MSAA 다중 샘플링을 활성화하고 다중 샘플링 레벨을 설정한다.

	ComPtr<ID3D12Resource> m_ppd3dRenderTargetBuffers[N_RENDERTARGETBUFFERS];
	ComPtr<ID3D12Resource> m_ppd3dSwapChainBackBuffers[N_SWAPCHAINBUFFERS];
	ComPtr<ID3D12DescriptorHeap> m_pd3dRtvDescriptorHeap;
	UINT m_nRtvDescriptorIncrementSize;
	//렌더 타겟 버퍼, 서술자 힙 인터페이스 포인터, 렌더 타겟 서술자 원소의 크기.

	ComPtr<ID3D12Resource> m_pd3dDepthStencilBuffer;
	ComPtr<ID3D12DescriptorHeap> m_pd3dDsvDescriptorHeap;
	UINT m_nDsvDescriptorIncrementSize;
	//깊이-스텐실 버퍼, 서술자 힙 인터페이스 포인터, 깊이-스텐실 서술자 원소의 크기.

	ComPtr<ID3D12Resource> m_pd3dShadowDepthBuffer;
	//그림자 깊이 버퍼

	// RTV / DSV 버퍼 생성용
	D3D12_CPU_DESCRIPTOR_HANDLE m_pRtvRenderTargetBufferCPUHandles[N_RENDERTARGETBUFFERS];
	D3D12_CPU_DESCRIPTOR_HANDLE	m_pRtvSwapChainBackBufferCPUHandles[N_SWAPCHAINBUFFERS];

	ComPtr<ID3D12CommandQueue> m_pd3dCommandQueue;
	ComPtr<ID3D12CommandAllocator> m_pd3dCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_pd3dCommandList; 
	//명령 큐, 명령 할당자, 명령 리스트의 인터페이스 포인터.

	ComPtr<ID3D12Fence> m_pd3dFence;

	UINT m_cbvSrvDescriptorIncrementSize{ 0 }; 

	ComPtr<ID3D12RootSignature> m_pGraphicsRootSignature;
	ComPtr<ID3D12RootSignature> m_pComputeRootSignature;

	shared_ptr<DrawManager> m_pDrawManager;
};

array<const D3D12_STATIC_SAMPLER_DESC, 7> GetStaticSamplers();
