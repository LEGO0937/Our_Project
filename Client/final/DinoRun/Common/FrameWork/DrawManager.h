#pragma once
#include "../stdafx.h"
#include "../../Scenes/GameScene.h"
#include "../../Scenes/StartScene.h"
#include "../../Scenes/EndScene.h"
#include "../../Scenes/RoomScene.h"
#include "../../Scenes/ItemGameScene.h"
#include "../../Scenes/LoadingScene.h"

class DrawManager
{
public:	
	DrawManager();
	~DrawManager();

public:
	void Initialize();
	void Release();

	void Render(shared_ptr<BaseScene> pScene, float fTimeElapsed);
	void RenderDepth(shared_ptr<BaseScene> pScene); // 그림자 기록 랜더
	void RenderPostProcess(shared_ptr<BaseScene> pScene); // 후처리 랜더
	void RenderDefault(shared_ptr<BaseScene> pScene, float fTimeElapsed); //기본 랜더
	void RenderLoading(shared_ptr<BaseScene> pScene); // 로딩씬 전용

	void SetDsvCPUHandleWithDsvHeap(ComPtr<ID3D12DescriptorHeap> pDsvDescriptorHeap, UINT incrementSize);

	void WaitForGpuComplete();
	void MoveToNextFrame();

	void ResetCommandAllocator();
	void ResetCommandList();
	void ExecuteCommandList();

	void SetSwapChain(ComPtr<IDXGISwapChain3> pSwapChain) { m_pdxgiSwapChain = pSwapChain; }
	void SetSwapChainBufferIndex(UINT swapChainBufferIndex) { m_nSwapChainBufferIndex = swapChainBufferIndex; }

	void SetRenderTargetBuffers(ComPtr<ID3D12Resource>* ppRenderTargetBuffers) { m_ppd3dRenderTargetBuffers = ppRenderTargetBuffers; }
	void SetSwapChainBackBuffers(ComPtr<ID3D12Resource>* ppd3dSwapChainBackBuffers) { m_ppd3dSwapChainBuffers = ppd3dSwapChainBackBuffers; }
	void SetRtvRenderTargetBufferCPUHandles(D3D12_CPU_DESCRIPTOR_HANDLE* pRtvRenderTargetBufferCPUHandles) { m_pRtvRenderTargetBufferCPUHandles = pRtvRenderTargetBufferCPUHandles; }
	void SetRtvSwapChainBackBufferCPUHandles(D3D12_CPU_DESCRIPTOR_HANDLE *pRtvSwapChainBackBufferCPUHandles) { m_pRtvSwapChainBufferCPUHandles = pRtvSwapChainBackBufferCPUHandles; }

	void SetCommandQueue(ComPtr<ID3D12CommandQueue> pCommandQueue) { m_pd3dCommandQueue = pCommandQueue; }
	void SetCommandAllocator(ComPtr<ID3D12CommandAllocator> pCommandAllocator) { m_pd3dCommandAllocator = pCommandAllocator; }
	void SetCommandList(ComPtr<ID3D12GraphicsCommandList> pCommandList) { m_pd3dCommandList = pCommandList; }

	void SetFence(ComPtr<ID3D12Fence> pFence) { m_pd3dFence = pFence; }

	void SaveGraphicsRootSignature(ComPtr<ID3D12RootSignature> pGraphicsRootSignature) { m_pGraphicsRootSignature = pGraphicsRootSignature; }
	void SaveComputeRootSignature(ComPtr<ID3D12RootSignature> pComputeRootSignature) { m_pComputeRootSignature = pComputeRootSignature; }

	void SetShadowDepthBuffer(ComPtr<ID3D12Resource> pShadowDepthBuffer) { m_pd3dShadowDepthBuffer = pShadowDepthBuffer; }

	void SetLoadingScene(shared_ptr<BaseScene> pLoadingScene) { m_pLoadingScene = pLoadingScene; }

	void SetComputeRootSignature();
private:
	void ChangeResourceState(ComPtr<ID3D12Resource> pResource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter);

private:	
	ComPtr<IDXGISwapChain3> m_pdxgiSwapChain;
	UINT m_nSwapChainBufferIndex = 0; //현재 스왑 체인의 후면 버퍼 인덱스.

	ComPtr<ID3D12CommandQueue> m_pd3dCommandQueue;
	ComPtr<ID3D12CommandAllocator> m_pd3dCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_pd3dCommandList;

	ComPtr<ID3D12Fence> m_pd3dFence;
	UINT64 m_nFenceValue[nSwapchainBuffers];
	HANDLE m_hFenceEvent; //펜스 인터페이스 포인터, 펜스의 값, 이벤트 핸들.

	ComPtr<ID3D12Resource> *m_ppd3dRenderTargetBuffers{ NULL };  //여기서의 랜더타겟은 후처리용임
	ComPtr<ID3D12Resource> *m_ppd3dSwapChainBuffers{ NULL }; //화면에 그릴 Back Buffer
	ComPtr<ID3D12Resource> m_pd3dShadowDepthBuffer;

	D3D12_CPU_DESCRIPTOR_HANDLE	*m_pRtvRenderTargetBufferCPUHandles{ NULL };
	D3D12_CPU_DESCRIPTOR_HANDLE	*m_pRtvSwapChainBufferCPUHandles{ NULL };

	D3D12_CPU_DESCRIPTOR_HANDLE	m_dsvDepthStencilBufferCPUHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE	m_dsvShadowBufferCPUHandle;

	ComPtr<ID3D12RootSignature> m_pGraphicsRootSignature;
	ComPtr<ID3D12RootSignature> m_pComputeRootSignature;

	shared_ptr<BaseScene> m_pLoadingScene;
};

