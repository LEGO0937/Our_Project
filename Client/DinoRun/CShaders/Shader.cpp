//-----------------------------------------------------------------------------
// File: Shader.cpp
//-----------------------------------------------------------------------------

#include "../Common/stdafx.h"
#include "Shader.h"

#include "../Common/FrameWork/CreateManager.h"

CShader::CShader()
{
	//m_nReferences = 1;
}

CShader::~CShader()
{
	ReleaseShaderVariables();
	if (m_pd3dCbvSrvDescriptorHeap)
		m_pd3dCbvSrvDescriptorHeap->Release();
}


void CShader::OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList, int nPipelineState)
{
	if (m_pd3dCbvSrvDescriptorHeap)
		if (m_pd3dCbvSrvDescriptorHeap) pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);
}

void CShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	OnPrepareRender(pd3dCommandList);
}

void CShader::CreateCbvSrvDescriptorHeaps(CreateManager* pCreateManager, int nConstantBufferViews, int nShaderResourceViews)
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = nConstantBufferViews + nShaderResourceViews; //CBVs + SRVs 
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	pCreateManager->GetDevice().Get()->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dCbvSrvDescriptorHeap);

	m_d3dCbvCPUDescriptorNextHandle = m_d3dCbvCPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dCbvGPUDescriptorNextHandle = m_d3dCbvGPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	m_d3dSrvCPUDescriptorNextHandle.ptr = m_d3dSrvCPUDescriptorStartHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
	m_d3dSrvGPUDescriptorNextHandle.ptr = m_d3dSrvGPUDescriptorStartHandle.ptr = m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
}
void CShader::BackDescriptorHeapCount()
{
	m_d3dSrvGPUDescriptorNextHandle.ptr -= ::gnCbvSrvDescriptorIncrementSize;
	m_d3dSrvCPUDescriptorNextHandle.ptr -= ::gnCbvSrvDescriptorIncrementSize;
}

D3D12_GPU_DESCRIPTOR_HANDLE CShader::CreateConstantBufferViews(CreateManager* pCreateManager, int nConstantBufferViews, ID3D12Resource *pd3dConstantBuffers, UINT nStride)
{
	D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle = m_d3dCbvGPUDescriptorNextHandle;
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = pd3dConstantBuffers->GetGPUVirtualAddress();
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	d3dCBVDesc.SizeInBytes = nStride;
	for (int j = 0; j < nConstantBufferViews; j++)
	{
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (nStride * j);
		m_d3dCbvCPUDescriptorNextHandle.ptr = m_d3dCbvCPUDescriptorNextHandle.ptr + ::gnCbvSrvDescriptorIncrementSize;
		pCreateManager->GetDevice().Get()->CreateConstantBufferView(&d3dCBVDesc, m_d3dCbvCPUDescriptorNextHandle);
		m_d3dCbvGPUDescriptorNextHandle.ptr = m_d3dCbvGPUDescriptorNextHandle.ptr + ::gnCbvSrvDescriptorIncrementSize;
	}
	return(d3dCbvGPUDescriptorHandle);
}

D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc(D3D12_RESOURCE_DESC d3dResourceDesc, UINT nTextureType)
{
	
	D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc;
	d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
	d3dShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	switch (nTextureType)
	{
	case RESOURCE_TEXTURE2D: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 1)
	case RESOURCE_TEXTURE2D_ARRAY:
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		d3dShaderResourceViewDesc.Texture2D.MipLevels = -1;
		d3dShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
		d3dShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_TEXTURE2DARRAY: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize != 1)
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		d3dShaderResourceViewDesc.Texture2DArray.MipLevels = -1;
		d3dShaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2DArray.PlaneSlice = 0;
		d3dShaderResourceViewDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
		d3dShaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
		d3dShaderResourceViewDesc.Texture2DArray.ArraySize = d3dResourceDesc.DepthOrArraySize;
		break;
	case RESOURCE_TEXTURE_CUBE: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 6)
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		d3dShaderResourceViewDesc.TextureCube.MipLevels = -1;
		d3dShaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_BUFFER: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		d3dShaderResourceViewDesc.Buffer.FirstElement = 0;
		d3dShaderResourceViewDesc.Buffer.NumElements = 0;
		d3dShaderResourceViewDesc.Buffer.StructureByteStride = 0;
		d3dShaderResourceViewDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		break;
	}
	return(d3dShaderResourceViewDesc);
}

D3D12_GPU_DESCRIPTOR_HANDLE CShader::CreateShaderResourceViews(CreateManager* pCreateManager, CTexture *pTexture, UINT nRootParameter, bool bAutoIncrement)
{
	D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGPUDescriptorHandle = m_d3dSrvGPUDescriptorNextHandle;
	if (pTexture)
	{
		int nTextures = pTexture->GetTextures();
		int nTextureType = pTexture->GetTextureType();
		for (int i = 0; i < nTextures; i++)
		{
			ID3D12Resource *pShaderResource = pTexture->GetTexture(i);
			D3D12_RESOURCE_DESC d3dResourceDesc = pShaderResource->GetDesc();
			D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = GetShaderResourceViewDesc(d3dResourceDesc, nTextureType);
			pCreateManager->GetDevice().Get()->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_d3dSrvCPUDescriptorNextHandle);
			m_d3dSrvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

			pTexture->SetRootArgument(i, (bAutoIncrement) ? (nRootParameter + i) : nRootParameter, m_d3dSrvGPUDescriptorNextHandle);
			m_d3dSrvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
		}
	}
	return(d3dSrvGPUDescriptorHandle);
}

D3D12_GPU_DESCRIPTOR_HANDLE CShader::CreateShadowResourceViews(CreateManager* pCreateManager, CTexture *pTexture, UINT nRootParameter, bool bAutoIncrement)
{
	D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGPUDescriptorHandle = m_d3dSrvGPUDescriptorNextHandle;
	if (pTexture)
	{
		int nTextures = pTexture->GetTextures();
		int nTextureType = pTexture->GetTextureType();
		for (int i = 0; i < nTextures; i++)
		{
			ID3D12Resource *pShaderResource = pTexture->GetTexture(i);
			D3D12_RESOURCE_DESC d3dResourceDesc = pShaderResource->GetDesc();
			D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = GetShaderResourceViewDesc(d3dResourceDesc, nTextureType);
			d3dShaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			d3dShaderResourceViewDesc.Texture2D.MipLevels = 1;
			pCreateManager->GetDevice().Get()->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_d3dSrvCPUDescriptorNextHandle);
			m_d3dSrvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

			pTexture->SetRootArgument(i, (bAutoIncrement) ? (nRootParameter + i) : nRootParameter, m_d3dSrvGPUDescriptorNextHandle);
			m_d3dSrvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
		}
	}
	return(d3dSrvGPUDescriptorHandle);
}

CObjectsShader::CObjectsShader()
{
	m_nReferences = 1;
}
CObjectsShader::~CObjectsShader()
{

}

void CObjectsShader::ReleaseObjects()
{
	if (objectList.size())
	{
		for (CGameObject* ob : objectList)
		{
			ob->Release();
		}
		objectList.clear();
	}
	if (m_ppObjects)
	{
		m_ppObjects->Release();
		m_ppObjects = NULL;
	}
}

void CObjectsShader::AnimateObjects(float fTimeElapsed)
{
	if (objectList.size())
	{
		for (CGameObject* ob : objectList)
		{
			ob->Animate(fTimeElapsed);
		}
	}
}

void CObjectsShader::ReleaseUploadBuffers()
{
	if (objectList.size())
	{
		for (CGameObject* ob : objectList)
		{
			ob->ReleaseUploadBuffers();
		}
	}
	m_ppObjects->ReleaseUploadBuffers();
}

void CObjectsShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);
	if (objectList.size())
	{
		for (CGameObject* ob : objectList)
		{
			ob->Render(pd3dCommandList, pCamera, 0);
		}
	}

}

//----------------

CObInstancingShader::CObInstancingShader()
{
}
CObInstancingShader::~CObInstancingShader()
{
}

void CObInstancingShader::CreateShaderVariables(CreateManager* pCreateManager)
{
	m_ppObjects->CreateInstanceBuffer(pCreateManager, objectList.size(), instancedObjectInfo);
}
void CObInstancingShader::ReleaseShaderVariables()
{
	m_ppObjects->ReleaseShaderVariables();
}
//인스턴싱 정보(객체의 월드 변환 행렬과 색상)를 정점 버퍼에 복사한다.
void CObInstancingShader::UpdateShaderVariables(ID3D12GraphicsCommandList
	*pd3dCommandList , CCamera *pCamera)
{
	drawingCount = 0;
	if (objectList.size())
	{
		for (CGameObject* ob : objectList)
		{
			ob->UpdateTransform(NULL);
			if (ob->isEnable && ob->IsVisible_Ins(pCamera))
			{
				ob->UpdateTransform_Instancing(instancedObjectInfo, drawingCount, NULL);
				drawingCount++;
			}
		}
	}
}


void CObInstancingShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera
	*pCamera)
{
	UpdateShaderVariables(pd3dCommandList, pCamera);  //그림자 적용시 여기에선 더이상 사용안함
	if (objectList.size() > 0 && isEnable)
	{
		CShader::Render(pd3dCommandList, pCamera);
		//모든 게임 객체의 인스턴싱 데이터를 버퍼에 저장한다. 

		m_ppObjects->Render(pd3dCommandList, pCamera, drawingCount);
		//하나의 정점 데이터를 사용하여 모든 게임 객체(인스턴스)들을 렌더링한다. 
	}
}

void CObInstancingShader::ShadowRender(ID3D12GraphicsCommandList *pd3dCommandList, CCamera
	*pCamera)
{
	UpdateShaderVariables(pd3dCommandList, pCamera);
	if (objectList.size() > 0 && isEnable)
	{
		CShader::Render(pd3dCommandList, pCamera);
		//모든 게임 객체의 인스턴싱 데이터를 버퍼에 저장한다. 
		m_ppObjects->ShadowRender(pd3dCommandList, pCamera, drawingCount);
		//하나의 정점 데이터를 사용하여 모든 게임 객체(인스턴스)들을 렌더링한다. 
	}
}

#ifdef _WITH_BOUND_BOX
void CObInstancingShader::BbxRender(ID3D12GraphicsCommandList *pd3dCommandList, CCamera
	*pCamera)
{
	UpdateShaderVariables(pd3dCommandList, pCamera);  //그림자 적용시 여기에선 더이상 사용안함
	if (objectList.size() > 0 && isEnable)
	{
		CShader::Render(pd3dCommandList, pCamera);
		//모든 게임 객체의 인스턴싱 데이터를 버퍼에 저장한다. 

		m_ppObjects->BbxRender(pd3dCommandList, pCamera, drawingCount);
		//하나의 정점 데이터를 사용하여 모든 게임 객체(인스턴스)들을 렌더링한다. 
	}
}
#endif
void CObInstancingShader::ReleaseObjects()
{


	if (objectList.size())
	{
		for (CGameObject* ob : objectList)
		{
			ob->Release();
		}
		objectList.clear();
	}
	if (m_ppObjects)
	{
		m_ppObjects->Release();
		m_ppObjects = NULL;
	}
}


//----------------------

CSkinedObInstancingShader::CSkinedObInstancingShader()
{
}
CSkinedObInstancingShader::~CSkinedObInstancingShader()
{
}

void CSkinedObInstancingShader::CreateShaderVariables(CreateManager* pCreateManager)
{
	m_ppSkinedObjects->CreateSkinedInstanceBuffer(pCreateManager, m_vSkinedObjectList.size(), instancedObjectInfo);
}
void CSkinedObInstancingShader::ReleaseShaderVariables()
{
	m_ppSkinedObjects->ReleaseShaderVariables();
}
//인스턴싱 정보(객체의 월드 변환 행렬과 색상)를 정점 버퍼에 복사한다.
void CSkinedObInstancingShader::UpdateShaderVariables(ID3D12GraphicsCommandList
	*pd3dCommandList, CCamera *pCamera)
{

	drawingCount = 0;
	if (m_vSkinedObjectList.size())
	{
		for (CPlayer* ob : m_vSkinedObjectList)
		{
			ob->UpdateTransform(NULL);
			if (ob->isEnable && ob->IsVisible_Ins(pCamera))
			{
				ob->UpdateTransform_SkinedInstancing(instancedObjectInfo, drawingCount);
				drawingCount++;
			}
		}
	}
}

void CSkinedObInstancingShader::AnimateObjects(float fTimeElapsed)
{
	if (m_vSkinedObjectList.size())
	{
		for (CGameObject* ob : m_vSkinedObjectList)
		{
			ob->Animate(fTimeElapsed);
		}
	}
}

void CSkinedObInstancingShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera
	*pCamera)
{
	UpdateShaderVariables(pd3dCommandList, pCamera);  //그림자 적용시 여기에선 더이상 사용안함
	if (m_vSkinedObjectList.size() > 0 && isEnable)
	{
		CShader::Render(pd3dCommandList, pCamera);
		//모든 게임 객체의 인스턴싱 데이터를 버퍼에 저장한다. 
		
		//하나의 정점 데이터를 사용하여 모든 게임 객체(인스턴스)들을 렌더링한다. 
		
		m_ppSkinedObjects->Render(pd3dCommandList, pCamera, drawingCount);
	}
}

void CSkinedObInstancingShader::ShadowRender(ID3D12GraphicsCommandList *pd3dCommandList, CCamera
	*pCamera)
{
	UpdateShaderVariables(pd3dCommandList, pCamera);
	if (m_vSkinedObjectList.size() > 0 && isEnable)
	{
		CShader::Render(pd3dCommandList, pCamera);
		//모든 게임 객체의 인스턴싱 데이터를 버퍼에 저장한다. 
		
		//하나의 정점 데이터를 사용하여 모든 게임 객체(인스턴스)들을 렌더링한다. 
		m_ppSkinedObjects->ShadowRender(pd3dCommandList, pCamera, drawingCount);
	}
}

#ifdef _WITH_BOUND_BOX
void CSkinedObInstancingShader::BbxRender(ID3D12GraphicsCommandList *pd3dCommandList, CCamera
	*pCamera)
{
	if (m_vSkinedObjectList.size() > 0)
	{
		//모든 게임 객체의 인스턴싱 데이터를 버퍼에 저장한다. 
		UpdateShaderVariables(pd3dCommandList, pCamera);
		//하나의 정점 데이터를 사용하여 모든 게임 객체(인스턴스)들을 렌더링한다. 
		m_ppSkinedObjects->BbxRender(pd3dCommandList, pCamera, drawingCount);
	}
}
#endif
void CSkinedObInstancingShader::ReleaseObjects()
{
	if (m_vSkinedObjectList.size())
	{
		for (CGameObject* ob : m_vSkinedObjectList)
		{
			ob->Release();
		}
		m_vSkinedObjectList.clear();
	}
	if (m_ppSkinedObjects)
	{
		m_ppSkinedObjects->Release();
		m_ppSkinedObjects = NULL;
	}
}

void CSkinedObInstancingShader::ReleaseUploadBuffers()
{
	if (m_vSkinedObjectList.size())
	{
		for (CGameObject* ob : m_vSkinedObjectList)
		{
			ob->ReleaseUploadBuffers();
		}
	}
	m_ppSkinedObjects->ReleaseUploadBuffers();
}

CUiShader::CUiShader()
{
}
CUiShader::~CUiShader()
{
}


void CUiShader::BuildObjects(CreateManager* pCreateManager, void* pInformation)
{
}

void CUiShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	UpdateShaderVariables(pd3dCommandList, pCamera);
	if (objectList.size() > 0 && isEnable)
	{
		CShader::Render(pd3dCommandList, pCamera);
		//모든 게임 객체의 인스턴싱 데이터를 버퍼에 저장한다. 
		
		//하나의 정점 데이터를 사용하여 모든 게임 객체(인스턴스)들을 렌더링한다. 
		m_ppObjects->Render(pd3dCommandList, pCamera, objectList.size());
	}
}

void CUiShader::CreateShaderVariables(CreateManager* pCreateManager)
{
	if (objectList.size() > 0)
	{
		m_pd3dcbUis = ::CreateBufferResource(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), NULL,
			sizeof(CB_UI_INFO) * objectList.size(), D3D12_HEAP_TYPE_UPLOAD,
			D3D12_RESOURCE_STATE_GENERIC_READ, NULL);
		//정점 버퍼(업로드 힙)에 대한 포인터를 저장한다. 
		m_pd3dcbUis->Map(0, NULL, (void **)&m_pcbMappedUis);
	}
}
void CUiShader::ReleaseShaderVariables()
{
	if (m_pd3dcbUis)
	{
		m_pd3dcbUis->Unmap(0, NULL);
		m_pd3dcbUis->Release();
		m_pd3dcbUis = NULL;
	}
}

void CUiShader::UpdateShaderVariables(ID3D12GraphicsCommandList
	*pd3dCommandList, CCamera *pCamera)
{
	int idx = 0;
	for (CGameObject* ob : objectList)
	{
		XMStoreFloat4x4(&(m_pcbMappedUis[idx].m_xmf4x4World),
			XMMatrixTranspose(XMLoadFloat4x4(&ob->m_xmf4x4World)));
		m_pcbMappedUis[idx].uvX = uvX[idx];
		m_pcbMappedUis[idx].uvY = uvY[idx];
		idx++;
	}
	pd3dCommandList->SetGraphicsRootShaderResourceView(3,
		m_pd3dcbUis->GetGPUVirtualAddress());
}

void CUiShader::ReleaseObjects()
{
	if (objectList.size())
	{
		for (CGameObject* ob : objectList)
		{
			ob->Release();
		}
		objectList.clear();
		uvX.clear();
		uvY.clear();
	}

	if (m_ppObjects)
	{
		m_ppObjects->Release();
		m_ppObjects = NULL;
	}
}