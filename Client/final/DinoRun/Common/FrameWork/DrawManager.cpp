#include "DrawManager.h"

DrawManager::DrawManager()
{
	for (int i = 0; i < nRendertargetBuffers; i++)
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

void DrawManager::Render(shared_ptr<BaseScene> pScene, float fTimeElapsed)
{
	RenderDepth(pScene);   // ShadowDraw
	RenderDefault(pScene, fTimeElapsed);   // BaseDraw
	RenderPostProcess(pScene);
	MoveToNextFrame();
}

void DrawManager::RenderDepth(shared_ptr<BaseScene> pScene)
{
	HRESULT hResult;
	WaitForGpuComplete();
	hResult = m_pd3dCommandList->Reset(m_pd3dCommandAllocator.Get(), NULL);

	m_pd3dCommandList->SetGraphicsRootSignature(m_pGraphicsRootSignature.Get());
	m_pd3dCommandList->SetComputeRootSignature(m_pComputeRootSignature.Get());
	//�׸��� ���� ���� ����
	ChangeResourceState(m_pd3dShadowDepthBuffer, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE);

	m_pd3dCommandList->ClearDepthStencilView(m_dsvShadowBufferCPUHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	//����Ÿ���� Null�̹Ƿ� �׸��ڹ��ۿ��� ����� ��.
	m_pd3dCommandList->OMSetRenderTargets(0, NULL, FALSE, &m_dsvShadowBufferCPUHandle);

	pScene->RenderShadow();

	ChangeResourceState(m_pd3dShadowDepthBuffer, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ);
	//------------------------------
	
	//�ӵ��� ���, ������ ����Ÿ���� ����ü���� ���۰� �ƴ� ����Ÿ�ٹ��۷� ���
	ChangeResourceState(m_ppd3dRenderTargetBuffers[0], D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET);
	
	m_pd3dCommandList->ClearRenderTargetView(m_pRtvRenderTargetBufferCPUHandles[0], Colors::Black, 0, NULL);
	m_pd3dCommandList->ClearDepthStencilView(m_dsvDepthStencilBufferCPUHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	m_pd3dCommandList->OMSetRenderTargets(1, &m_pRtvRenderTargetBufferCPUHandles[0], TRUE, &m_dsvDepthStencilBufferCPUHandle);

	pScene->RenderVelocity();

	ChangeResourceState(m_ppd3dRenderTargetBuffers[0], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ);
	
	
	ExecuteCommandList();
}

void DrawManager::RenderDefault(shared_ptr<BaseScene> pScene, float fTimeElapsed)
{
	m_pd3dCommandList->Reset(m_pd3dCommandAllocator.Get(), NULL);

	m_pd3dCommandList->SetGraphicsRootSignature(m_pGraphicsRootSignature.Get());
	m_pd3dCommandList->SetComputeRootSignature(m_pComputeRootSignature.Get());

	ChangeResourceState(m_ppd3dSwapChainBuffers[m_nSwapChainBufferIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	m_pd3dCommandList->ClearDepthStencilView(m_dsvDepthStencilBufferCPUHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	m_pd3dCommandList->ClearRenderTargetView(m_pRtvSwapChainBufferCPUHandles[m_nSwapChainBufferIndex], Colors::LightGray, 0, NULL);
	m_pd3dCommandList->OMSetRenderTargets(1, &m_pRtvSwapChainBufferCPUHandles[m_nSwapChainBufferIndex],
		TRUE, &m_dsvDepthStencilBufferCPUHandle);

	if (pScene)
		pScene->Render();

}
void DrawManager::RenderLoading(shared_ptr<BaseScene> pScene)
{
	HRESULT hResult;
	m_pd3dCommandList->SetGraphicsRootSignature(m_pGraphicsRootSignature.Get());
	m_pd3dCommandList->SetComputeRootSignature(m_pComputeRootSignature.Get());
	
	ChangeResourceState(m_ppd3dSwapChainBuffers[m_nSwapChainBufferIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	m_pd3dCommandList->ClearDepthStencilView(m_dsvDepthStencilBufferCPUHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	m_pd3dCommandList->ClearRenderTargetView(m_pRtvSwapChainBufferCPUHandles[m_nSwapChainBufferIndex], Colors::Gray, 0, NULL);
	m_pd3dCommandList->OMSetRenderTargets(1, &m_pRtvSwapChainBufferCPUHandles[m_nSwapChainBufferIndex],
		TRUE, &m_dsvDepthStencilBufferCPUHandle);

	if (pScene)
		pScene->Render();

	ChangeResourceState(m_ppd3dSwapChainBuffers[m_nSwapChainBufferIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	ExecuteCommandList();

	hResult = m_pdxgiSwapChain->Present(0, 0);
	MoveToNextFrame();

	hResult = m_pd3dCommandList->Reset(m_pd3dCommandAllocator.Get(), NULL);
}
void DrawManager::RenderPostProcess(shared_ptr<BaseScene> pScene)
{
	pScene->RenderPostProcess(m_ppd3dSwapChainBuffers[m_nSwapChainBufferIndex],m_ppd3dRenderTargetBuffers[0]);

	ChangeResourceState(m_ppd3dSwapChainBuffers[m_nSwapChainBufferIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	ExecuteCommandList();

	HRESULT hResult = m_pdxgiSwapChain->Present(0, 0);
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

	//GPU�� �潺�� ���� �����ϴ� ����� ��� ť�� �߰��Ѵ�.
	if (m_pd3dFence->GetCompletedValue() < fenceValue)
	{
		//�潺�� ���� ���� ������ ������ ������ �潺�� ���� ���� ������ ���� �� ������ ��ٸ���.
		hResult = m_pd3dFence->SetEventOnCompletion(fenceValue, m_hFenceEvent);

		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void DrawManager::MoveToNextFrame()
{
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
}
void DrawManager::ResetCommandAllocator()
{
	HRESULT hResult;
	hResult = m_pd3dCommandAllocator->Reset();
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

void DrawManager::SetComputeRootSignature()
{
	m_pd3dCommandList->SetComputeRootSignature(m_pComputeRootSignature.Get());
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