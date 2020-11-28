#pragma once
#include "../stdafx.h"
#include "SingleTon/Singleton.h"

class GameManager : public Singleton<GameManager>
{
public:
	GameManager();
	~GameManager();

private:
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	int m_nWndClientWidth{ FRAME_BUFFER_WIDTH };
	int m_nWndClientHeight{ FRAME_BUFFER_HEIGHT };

	ComPtr<IDXGIFactory4> m_pDxgiFactory;
	ComPtr<ID3D12Device> m_pD3dDevice;

	ComPtr<IDXGISwapChain3> m_pDxgiSwapChain;
	UINT m_nSwapChainBufferIndex = 0; //���� ���� ü���� �ĸ� ���� �ε���.

	DXGI_FORMAT m_dxgiBackBufferFormat{ DXGI_FORMAT_R8G8B8A8_UNORM };
	DXGI_FORMAT m_dxgiPostProcessingBufferFormat{ DXGI_FORMAT_R8G8B8A8_UNORM };

	bool m_bMsaa4xEnable = false;
	UINT m_nMsaa4xQualityLevels = 0; //MSAA ���� ���ø��� Ȱ��ȭ�ϰ� ���� ���ø� ������ �����Ѵ�.


	ComPtr<ID3D12DescriptorHeap> m_pD3dRtvDescriptorHeap;
	UINT m_nRtvDescriptorIncrementSize;

	ComPtr<ID3D12Resource> m_ppD3dPostProcessingBuffers[nRendertargetBuffers];
	ComPtr<ID3D12Resource> m_ppD3dSwapChainBuffers[nSwapchainBuffers];
	ComPtr<ID3D12Resource> m_pD3dVelocityBuffer;
	//�ӵ� ����
	ComPtr<ID3D12Resource> m_pD3dShadowDepthBuffer;
	//�׸��� ���� ����


	ComPtr<ID3D12Resource> m_pD3dDepthStencilBuffer;
	ComPtr<ID3D12DescriptorHeap> m_pD3dDsvDescriptorHeap;
	UINT m_nDsvDescriptorIncrementSize;
	//����-���ٽ� ����, ������ �� �������̽� ������, ����-���ٽ� ������ ������ ũ��.

	// RTV / DSV ���� ������
	D3D12_CPU_DESCRIPTOR_HANDLE m_pRtvPostProcessingBufferCPUHandles[nRendertargetBuffers];
	D3D12_CPU_DESCRIPTOR_HANDLE	m_pRtvSwapChainBufferCPUHandles[nSwapchainBuffers];

	D3D12_CPU_DESCRIPTOR_HANDLE	m_dsvDepthStencilBufferCPUHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE	m_dsvShadowBufferCPUHandle;


	ComPtr<ID3D12CommandQueue> m_pD3dCommandQueue;
	ComPtr<ID3D12CommandAllocator> m_pD3dCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_pD3dCommandList;

	//��� ť, ��� �Ҵ���, ��� ����Ʈ�� �������̽� ������.

	ComPtr<ID3D12Fence> m_pD3dFence;
	UINT64 m_nFenceValue[nSwapchainBuffers];
	HANDLE m_hFenceEvent; //�潺 �������̽� ������, �潺�� ��, �̺�Ʈ �ڵ�.

	UINT m_cbvSrvDescriptorIncrementSize{ 0 };

	ComPtr<ID3D12RootSignature> m_pGraphicsRootSignature;
	ComPtr<ID3D12RootSignature> m_pComputeRootSignature;

	shared_ptr<class BaseScene> m_pLoadingScene;


#if defined(_DEBUG)
	ID3D12Debug* m_pd3dDebugController;
#endif
private:
	void OnResizeBackBuffers();

	void CreateDirect3dDevice();
	void CreateCommandQueueAndList();
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateSwapChainRenderTargetViews();
	void CreatePostProcessingRenderTargetViews();
	void CreateDepthStencilView();
	void CreateGraphicsRootSignature();
	void CreateComputeRootSignature();

public:
	bool Initialize(HINSTANCE hInstance, HWND hWnd);
	void Release();

	void ResetSize(int width, int height);

	HWND GetHwnd() { return m_hWnd; }
	int GetWindowWidth() { return m_nWndClientWidth; }
	int GetWindowHeight() { return m_nWndClientHeight; }

	ComPtr<ID3D12Device> GetDevice() { return m_pD3dDevice; }
	ComPtr<ID3D12GraphicsCommandList> GetCommandList() { return m_pD3dCommandList; }

	ComPtr<ID3D12RootSignature> GetGraphicsRootSignature() { return m_pGraphicsRootSignature; }
	ComPtr<ID3D12RootSignature> GetComputeRootSignature() { return m_pComputeRootSignature; }

	UINT GetCbvSrvDescriptorIncrementSize() { return m_cbvSrvDescriptorIncrementSize; }

	ComPtr<ID3D12Resource> GetShadowBuffer() { return m_pD3dShadowDepthBuffer; }
	ComPtr<ID3D12Resource> GetVelocityBuffer() { return m_pD3dVelocityBuffer; }

	void ChangeResourceState(ComPtr<ID3D12Resource> pResource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter);

	void RenderLoading();

	void Render(shared_ptr<class BaseScene> pScene, float fTimeElapsed);
	void RenderShadow(shared_ptr<class BaseScene> pScene); // �׸��� ��� ����
	void RenderPostProcess(shared_ptr<class BaseScene> pScene); // ��ó�� ����
	void RenderDefault(shared_ptr<class BaseScene> pScene, float fTimeElapsed); //�⺻ ����
	void RenderLoading(shared_ptr<class BaseScene> pScene); // �ε��� ����

	void MoveToNextFrame();
	void ResetCommandAllocator();
	void ResetCommandList();
	void ExecuteCommandList();
	void WaitForGpuComplete();

	void SetComputeRootSignature()
	{
		m_pD3dCommandList->SetComputeRootSignature(m_pComputeRootSignature.Get());
	}
	void SetLoadingScene(shared_ptr<class BaseScene> pScene) { m_pLoadingScene = pScene; }
};

array<const D3D12_STATIC_SAMPLER_DESC, 5> GetStaticSamplers();