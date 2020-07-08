#include "MotionBlurShader.h"


MotionBlurShader::MotionBlurShader(CreateManager* pCreateManager)
{
	m_pd3dDevice = pCreateManager->GetDevice();

	mWidth = pCreateManager->GetWindowWidth();
	mHeight = pCreateManager->GetWindowHeight();
	mFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 4;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	m_pd3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_pCbvSrvUavDescriptorHeap));

	BuildResources();
	BuildDescriptors(
		m_pCbvSrvUavDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		m_pCbvSrvUavDescriptorHeap->GetGPUDescriptorHandleForHeapStart(),
		pCreateManager->GetCbvSrvDescriptorIncrementSize());
}

ID3D12Resource* MotionBlurShader::Output()
{
	return mBlurMap0.Get();
}

void MotionBlurShader::BuildDescriptors(D3D12_CPU_DESCRIPTOR_HANDLE hCpuDescriptor,
	D3D12_GPU_DESCRIPTOR_HANDLE hGpuDescriptor,
	UINT descriptorSize)
{

	m_hFirstBlurCpuSrv = hCpuDescriptor;
	hCpuDescriptor.ptr += descriptorSize;
	m_hFirstBlurCpuUav = hCpuDescriptor;
	hCpuDescriptor.ptr += descriptorSize;
	m_hSecondBlurCpuSrv = hCpuDescriptor;
	hCpuDescriptor.ptr += descriptorSize;
	m_hSecondBlurCpuUav = hCpuDescriptor;

	m_hFirstBlurGpuSrv = hGpuDescriptor;
	hGpuDescriptor.ptr += descriptorSize;
	m_hFirstBlurGpuUav = hGpuDescriptor;
	hGpuDescriptor.ptr += descriptorSize;
	m_hSecondBlurGpuSrv = hGpuDescriptor;
	hGpuDescriptor.ptr += descriptorSize;
	m_hSecondBlurGpuUav = hGpuDescriptor;

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
	ID3D12PipelineState* pHorzBlurPSO,
	ID3D12PipelineState* pVertBlurPSO,
	ID3D12Resource* input,
	int blurCount)
{
	auto weights = CalcGaussWeights(2.5f);  // 2.5 x 2 Ä­¾¿ °è»ê
	int blurRadius = (int)weights.size() / 2;

	pCommandList->SetComputeRoot32BitConstants(0, 1, &blurRadius, 0);
	pCommandList->SetComputeRoot32BitConstants(0, (UINT)weights.size(), weights.data(), 1);

	ChangeResourceState(pCommandList, input, D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_COPY_SOURCE);

	ChangeResourceState(pCommandList, mBlurMap0.Get(), D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_RESOURCE_STATE_COPY_DEST);

	pCommandList->CopyResource(mBlurMap0.Get(), input);

	ChangeResourceState(pCommandList, mBlurMap0.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_GENERIC_READ);

	pCommandList->SetDescriptorHeaps(1, m_pCbvSrvUavDescriptorHeap.GetAddressOf());
	for (int i = 0; i < blurCount; ++i)
	{

		pCommandList->SetPipelineState(pHorzBlurPSO);

		pCommandList->SetComputeRootDescriptorTable(1, m_hFirstBlurGpuSrv);
		pCommandList->SetComputeRootDescriptorTable(2, m_hSecondBlurGpuUav);

		UINT numGroupsX = (UINT)ceilf(mWidth / 256.0f);
		pCommandList->Dispatch(numGroupsX, mHeight, 1);

		ChangeResourceState(pCommandList, mBlurMap0.Get(), D3D12_RESOURCE_STATE_GENERIC_READ,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		ChangeResourceState(pCommandList, mBlurMap1.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_GENERIC_READ);


		pCommandList->SetPipelineState(pVertBlurPSO);

		pCommandList->SetComputeRootDescriptorTable(1, m_hSecondBlurGpuSrv);
		pCommandList->SetComputeRootDescriptorTable(2, m_hFirstBlurGpuUav);

		UINT numGroupsY = (UINT)ceilf(mHeight / 256.0f);
		pCommandList->Dispatch(mWidth, numGroupsY, 1);

		ChangeResourceState(pCommandList, mBlurMap0.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_GENERIC_READ);

		ChangeResourceState(pCommandList, mBlurMap1.Get(), D3D12_RESOURCE_STATE_GENERIC_READ,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	}

	ChangeResourceState(pCommandList, input, D3D12_RESOURCE_STATE_COPY_SOURCE,
		D3D12_RESOURCE_STATE_COPY_DEST);

	pCommandList->CopyResource(input, Output());

	ChangeResourceState(pCommandList, input, D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
}

std::vector<float> MotionBlurShader::CalcGaussWeights(float sigma)
{
	float twoSigma2 = 2.0f*sigma*sigma;

	int blurRadius = (int)(2.0f * sigma);

	std::vector<float> weights;
	weights.resize(2 * blurRadius + 1);

	float weightSum = 0.0f;

	for (int i = -blurRadius; i <= blurRadius; ++i)
	{
		float x = (float)i;

		weights[i + blurRadius] = expf(-x * x / twoSigma2);

		weightSum += weights[i + blurRadius];
	}

	for (int i = 0; i < weights.size(); ++i)
	{
		weights[i] /= weightSum;
	}

	return weights;
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

	m_pd3dDevice->CreateShaderResourceView(mBlurMap0.Get(), &srvDesc, m_hFirstBlurCpuSrv);
	m_pd3dDevice->CreateUnorderedAccessView(mBlurMap0.Get(), nullptr, &uavDesc, m_hFirstBlurCpuUav);

	m_pd3dDevice->CreateShaderResourceView(mBlurMap1.Get(), &srvDesc, m_hSecondBlurCpuSrv);
	m_pd3dDevice->CreateUnorderedAccessView(mBlurMap1.Get(), nullptr, &uavDesc, m_hSecondBlurCpuUav);
}

void MotionBlurShader::BuildResources()
{
	mBlurMap0 = CreateTexture2DResource(m_pd3dDevice.Get(), mWidth, mHeight, mFormat,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);

	mBlurMap1 = CreateTexture2DResource(m_pd3dDevice.Get(), mWidth, mHeight, mFormat,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr);
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