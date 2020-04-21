#include "DrawManager.h"

DrawManager::DrawManager()
{
	for (int i = 0; i < N_RENDERTARGETBUFFERS; i++)
	{
		m_nFenceValue[i] = 0;
	}
}

DrawManager::~DrawManager()
{
}

void DrawManager::Initialize()
{
	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	m_nFenceValue[0] = 1;
}

void DrawManager::Release()
{
	::CloseHandle(m_hFenceEvent);
}

void DrawManager::Render(shared_ptr<BaseScene> pScene)
{
	RenderDepth(pScene);   // ShadowDraw
	RenderLight(pScene);   // BaseDraw
	RenderPostProcess(pScene);
	MoveToNextFrame();
}

void DrawManager::RenderDepth(shared_ptr<BaseScene> pScene)
{
	HRESULT hResult;
	WaitForGpuComplete();
	hResult = m_pd3dCommandAllocator->Reset();
	hResult = m_pd3dCommandList->Reset(m_pd3dCommandAllocator.Get(), NULL);

	m_pd3dCommandList->SetGraphicsRootSignature(m_pGraphicsRootSignature.Get());

	// Change to DEPTH_WRITE
	ChangeResourceState(m_pd3dShadowDepthBuffer, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE);

	// Clear Depth Stencil View
	m_pd3dCommandList->ClearDepthStencilView(m_dsvShadowBufferCPUHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	// Set Render Target and Depth Stencil
	m_pd3dCommandList->OMSetRenderTargets(0, NULL, FALSE, &m_dsvShadowBufferCPUHandle);

	// Render Scene
	pScene->RenderShadow();

	ChangeResourceState(m_pd3dShadowDepthBuffer, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ);
	//ExecuteCommandList();
}

void DrawManager::RenderLight(shared_ptr<BaseScene> pScene)
{
	HRESULT hResult;

	ChangeResourceState(m_ppd3dSwapChainBackBuffers[m_nSwapChainBufferIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	m_pd3dCommandList->ClearDepthStencilView(m_dsvDepthStencilBufferCPUHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	m_pd3dCommandList->ClearRenderTargetView(m_pRtvSwapChainBackBufferCPUHandles[m_nSwapChainBufferIndex], Colors::Gray, 0, NULL);
	m_pd3dCommandList->OMSetRenderTargets(1, &m_pRtvSwapChainBackBufferCPUHandles[m_nSwapChainBufferIndex],
		TRUE, &m_dsvDepthStencilBufferCPUHandle);

	if (pScene)
		pScene->Render();

}

void DrawManager::RenderPostProcess(shared_ptr<BaseScene> pScene)
{
	m_pd3dCommandList->SetComputeRootSignature(m_pComputeRootSignature.Get());

	pScene->RenderPostProcess(m_ppd3dSwapChainBackBuffers[m_nSwapChainBufferIndex]);

	ChangeResourceState(m_ppd3dSwapChainBackBuffers[m_nSwapChainBufferIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	ExecuteCommandList();

	HRESULT hResult = m_pdxgiSwapChain->Present(0, 0);
}
void DrawManager::RenderLoadingScreen(float loadingPercentage)
{
}

void DrawManager::SetDsvCPUHandleWithDsvHeap(ComPtr<ID3D12DescriptorHeap> pDsvDescriptorHeap, UINT incrementSize)
{
	m_dsvDepthStencilBufferCPUHandle = pDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_dsvShadowBufferCPUHandle.ptr = m_dsvDepthStencilBufferCPUHandle.ptr + incrementSize;
}

void DrawManager::WaitForGpuComplete()
{
	HRESULT hResult;
	++m_nFenceValue[m_nSwapChainBufferIndex];
	UINT64 fenceValue = m_nFenceValue[m_nSwapChainBufferIndex];

	hResult = m_pd3dCommandQueue->Signal(m_pd3dFence.Get(), fenceValue);

	//GPU가 펜스의 값을 설정하는 명령을 명령 큐에 추가한다.
	if (m_pd3dFence->GetCompletedValue() < fenceValue)
	{
		//펜스의 현재 값이 설정한 값보다 작으면 펜스의 현재 값이 설정한 값이 될 때까지 기다린다.
		hResult = m_pd3dFence->SetEventOnCompletion(fenceValue, m_hFenceEvent);

		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void DrawManager::MoveToNextFrame()
{
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
	WaitForGpuComplete();
}

void DrawManager::ResetCommandList()
{
	m_pd3dCommandList->Reset(m_pd3dCommandAllocator.Get(), NULL);
}

void DrawManager::ExecuteCommandList()
{
	HRESULT hResult;

	hResult = m_pd3dCommandList->Close();

	ID3D12CommandList *ppCommandLists[] = { m_pd3dCommandList.Get() };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppCommandLists);

	WaitForGpuComplete();
}

void DrawManager::ChangeResourceState(ComPtr<ID3D12Resource> pResource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter)
{
	D3D12_RESOURCE_BARRIER resourceBarrier;
	resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	resourceBarrier.Transition.pResource = pResource.Get();
	resourceBarrier.Transition.StateBefore = stateBefore;
	resourceBarrier.Transition.StateAfter = stateAfter;
	resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3dCommandList->ResourceBarrier(1, &resourceBarrier);
}