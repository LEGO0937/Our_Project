#include "MotionBlurShader.h"
#include "../Common/FrameWork/GameManager.h"

MotionBlurShader::MotionBlurShader()
{
	m_pd3dDevice = GameManager::GetInstance()->GetDevice();

	mWidth = GameManager::GetInstance()->GetWindowWidth();
	mHeight = GameManager::GetInstance()->GetWindowHeight();
	mFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 3;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	m_pd3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_pCbvSrvUavDescriptorHeap));

	BuildResources();
	BuildDescriptors(
		m_pCbvSrvUavDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		m_pCbvSrvUavDescriptorHeap->GetGPUDescriptorHandleForHeapStart(),
		GameManager::GetInstance()->GetCbvSrvDescriptorIncrementSize());
}

ID3D12Resource* MotionBlurShader::Output()
{
	return mBlurMap.Get();
}

void MotionBlurShader::BuildDescriptors(D3D12_CPU_DESCRIPTOR_HANDLE hCpuDescriptor,
	D3D12_GPU_DESCRIPTOR_HANDLE hGpuDescriptor,
	UINT descriptorSize)
{

	m_hBlurCpuUav = hCpuDescriptor;
	hCpuDescriptor.ptr += descriptorSize;
	m_hRenderTargetCpuSrv = hCpuDescriptor;
	hCpuDescriptor.ptr += descriptorSize;
	m_hVelocityCpuSrv = hCpuDescriptor;
	hCpuDescriptor.ptr += descriptorSize;

	m_hBlurGpuUav = hGpuDescriptor;
	hGpuDescriptor.ptr += descriptorSize;
	m_hRenderTargetGpuSrv = hGpuDescriptor;
	hGpuDescriptor.ptr += descriptorSize;
	m_hVelocityGpuSrv = hGpuDescriptor;
	hGpuDescriptor.ptr += descriptorSize;

	BuildDescriptors();
}

void MotionBlurShader::OnResize(UINT newWidth, UINT newHeight)
{
	if ((mWidth != newWidth) || (mHeight != newHeight))
	{
		mWidth = newWidth;
		mHeight = newHeight;

		BuildResources();

		BuildDescriptors();
	}
}

void MotionBlurShader::Dispatch(ID3D12GraphicsCommandList* pCommandList,
	ID3D12Resource* input, ID3D12Resource* VelocityMap,
	int blurCount)
{

	ChangeResourceState(pCommandList, input, D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_COPY_SOURCE);
	ChangeResourceState(pCommandList, VelocityMap, D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_RESOURCE_STATE_COPY_SOURCE);

	pCommandList->CopyResource(mRenderTargetMap.Get(), input);
	pCommandList->CopyResource(mVelocityMap.Get(), VelocityMap);

	//-----
	ChangeResourceState(pCommandList, mRenderTargetMap.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_GENERIC_READ);
	ChangeResourceState(pCommandList, mVelocityMap.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_GENERIC_READ);


	pCommandList->SetDescriptorHeaps(1, m_pCbvSrvUavDescriptorHeap.GetAddressOf());
	{
		pCommandList->SetComputeRootDescriptorTable(1, m_hRenderTargetGpuSrv);
		pCommandList->SetComputeRootDescriptorTable(7, m_hVelocityGpuSrv);
		pCommandList->SetComputeRootDescriptorTable(2, m_hBlurGpuUav);

		UINT numGroupsX = (UINT)ceilf(mWidth / 256.0f);
		pCommandList->Dispatch(numGroupsX, mHeight, 1);


		ChangeResourceState(pCommandList, mBlurMap.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_COPY_SOURCE);

		ChangeResourceState(pCommandList, mRenderTargetMap.Get(), D3D12_RESOURCE_STATE_GENERIC_READ,
			D3D12_RESOURCE_STATE_COPY_DEST);
		ChangeResourceState(pCommandList, mVelocityMap.Get(), D3D12_RESOURCE_STATE_GENERIC_READ,
			D3D12_RESOURCE_STATE_COPY_DEST);
	}

	ChangeResourceState(pCommandList, input, D3D12_RESOURCE_STATE_COPY_SOURCE,
		D3D12_RESOURCE_STATE_COPY_DEST);

	pCommandList->CopyResource(input, mBlurMap.Get());



	ChangeResourceState(pCommandList, mBlurMap.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	ChangeResourceState(pCommandList, input, D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	ChangeResourceState(pCommandList, VelocityMap, D3D12_RESOURCE_STATE_COPY_SOURCE,
		D3D12_RESOURCE_STATE_GENERIC_READ);
}

void MotionBlurShader::BuildDescriptors()
{

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = mFormat;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};

	uavDesc.Format = mFormat;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	m_pd3dDevice->CreateUnorderedAccessView(mBlurMap.Get(), nullptr, &uavDesc, m_hBlurCpuUav);
	m_pd3dDevice->CreateShaderResourceView(mRenderTargetMap.Get(), &srvDesc, m_hRenderTargetCpuSrv);
	m_pd3dDevice->CreateShaderResourceView(mVelocityMap.Get(), &srvDesc, m_hVelocityCpuSrv);
}

void MotionBlurShader::BuildResources()
{
	mBlurMap = CreateTexture2DResource(m_pd3dDevice.Get(), mWidth, mHeight, mFormat,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr);
	mBlurMap->SetName(L"MotionBlurResource");  //블러링 처리된 리소스

	mRenderTargetMap = CreateTexture2DResource(m_pd3dDevice.Get(), mWidth, mHeight, mFormat,
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_COPY_DEST, nullptr);
	mRenderTargetMap->SetName(L"RenderTargerResource"); //입력으로 받는 그려진 랜더타겟.

	mVelocityMap = CreateTexture2DResource(m_pd3dDevice.Get(), mWidth, mHeight, mFormat,
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_COPY_DEST, nullptr);
	mVelocityMap->SetName(L"VelocityMapResource"); //속도맵
}


void MotionBlurShader::ChangeResourceState(ID3D12GraphicsCommandList* pCommandList, ComPtr<ID3D12Resource> pResource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter)
{
	D3D12_RESOURCE_BARRIER resourceBarrier;
	resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	resourceBarrier.Transition.pResource = pResource.Get();
	resourceBarrier.Transition.StateBefore = stateBefore;
	resourceBarrier.Transition.StateAfter = stateAfter;
	resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	pCommandList->ResourceBarrier(1, &resourceBarrier);
}