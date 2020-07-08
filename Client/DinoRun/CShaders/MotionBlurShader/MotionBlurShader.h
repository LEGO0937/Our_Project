#pragma once
#include "../../Common/stdafx.h"
#include "../../Common/FrameWork/CreateManager.h"

class MotionBlurShader
{
protected:
	int									m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

public:
	MotionBlurShader(CreateManager* pCreateManager);

	MotionBlurShader(const BlurShader& rhs) = delete;
	MotionBlurShader& operator=(const BlurShader& rhs) = delete;
	~MotionBlurShader() = default;

	ID3D12Resource* Output();

	void BuildDescriptors(
		D3D12_CPU_DESCRIPTOR_HANDLE hCpuDescriptor,
		D3D12_GPU_DESCRIPTOR_HANDLE hGpuDescriptor,
		UINT descriptorSize);

	void OnResize(UINT newWidth, UINT newHeight);

	void Dispatch(
		ID3D12GraphicsCommandList* pCommandList,
		ID3D12PipelineState* pHorzBlurPSO,
		ID3D12PipelineState* pVertBlurPSO,
		ID3D12Resource* input,
		int blurCount);

	void ChangeResourceState(ID3D12GraphicsCommandList* pCommandList, ComPtr<ID3D12Resource> pResource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter);
private:
	std::vector<float> CalcGaussWeights(float sigma);

	void BuildDescriptors();
	void BuildResources();

private:
	const int BlurRadius = 5;

	ComPtr<ID3D12Device> m_pd3dDevice = nullptr;

	UINT mWidth = 0;
	UINT mHeight = 0;
	DXGI_FORMAT mFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	ComPtr<ID3D12DescriptorHeap> m_pCbvSrvUavDescriptorHeap = nullptr;

	D3D12_CPU_DESCRIPTOR_HANDLE m_hFirstBlurCpuSrv;
	D3D12_CPU_DESCRIPTOR_HANDLE m_hFirstBlurCpuUav;

	D3D12_CPU_DESCRIPTOR_HANDLE m_hSecondBlurCpuSrv;
	D3D12_CPU_DESCRIPTOR_HANDLE m_hSecondBlurCpuUav;

	D3D12_GPU_DESCRIPTOR_HANDLE m_hFirstBlurGpuSrv;
	D3D12_GPU_DESCRIPTOR_HANDLE m_hFirstBlurGpuUav;

	D3D12_GPU_DESCRIPTOR_HANDLE m_hSecondBlurGpuSrv;
	D3D12_GPU_DESCRIPTOR_HANDLE m_hSecondBlurGpuUav;

	// Two for ping-ponging the textures.
	Microsoft::WRL::ComPtr<ID3D12Resource> mBlurMap0 = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> mBlurMap1 = nullptr;

};

