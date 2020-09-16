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

	MotionBlurShader(const MotionBlurShader& rhs) = delete;
	MotionBlurShader& operator=(const MotionBlurShader& rhs) = delete;
	~MotionBlurShader() = default;

	ID3D12Resource* Output();

	void BuildDescriptors(
		D3D12_CPU_DESCRIPTOR_HANDLE hCpuDescriptor,
		D3D12_GPU_DESCRIPTOR_HANDLE hGpuDescriptor,
		UINT descriptorSize);

	void OnResize(UINT newWidth, UINT newHeight);

	void Dispatch(
		ID3D12GraphicsCommandList* pCommandList,
		ID3D12Resource* input, ID3D12Resource* VelocityMap,
		int blurCount);

	void ChangeResourceState(ID3D12GraphicsCommandList* pCommandList, ComPtr<ID3D12Resource> pResource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter);
private:
	void BuildDescriptors();
	void BuildResources();

private:
	const int BlurRadius = 5;

	ComPtr<ID3D12Device> m_pd3dDevice = nullptr;

	UINT mWidth = 0;
	UINT mHeight = 0;
	DXGI_FORMAT mFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	ComPtr<ID3D12DescriptorHeap> m_pCbvSrvUavDescriptorHeap = nullptr;

	D3D12_CPU_DESCRIPTOR_HANDLE m_hBlurCpuSrv;
	D3D12_CPU_DESCRIPTOR_HANDLE m_hBlurCpuUav;

	D3D12_CPU_DESCRIPTOR_HANDLE m_hRenderTargetCpuSrv;
	D3D12_CPU_DESCRIPTOR_HANDLE m_hRenderTargetCpuUav;

	D3D12_CPU_DESCRIPTOR_HANDLE m_hVelocityCpuSrv;
	D3D12_CPU_DESCRIPTOR_HANDLE m_hVelocityCpuUav;


	D3D12_GPU_DESCRIPTOR_HANDLE m_hBlurGpuUav;
	D3D12_GPU_DESCRIPTOR_HANDLE m_hRenderTargetGpuSrv;
	D3D12_GPU_DESCRIPTOR_HANDLE m_hVelocityGpuSrv;

	// Two for ping-ponging the textures.
	Microsoft::WRL::ComPtr<ID3D12Resource> mBlurMap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> mRenderTargetMap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> mVelocityMap = nullptr;

};

