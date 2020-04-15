//-----------------------------------------------------------------------------
// File: Shader.cpp
//-----------------------------------------------------------------------------

#include "../Common/stdafx.h"
#include "Shader.h"

CShader::CShader()
{
}

CShader::~CShader()
{
	ReleaseShaderVariables();
	if (m_pd3dCbvSrvDescriptorHeap)
		m_pd3dCbvSrvDescriptorHeap->Release();
}


void CShader::OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList, int nPipelineState)
{
	if(m_pd3dCbvSrvDescriptorHeap)
		if (m_pd3dCbvSrvDescriptorHeap) pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);
}

void CShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	OnPrepareRender(pd3dCommandList);

}



void CShader::CreateCbvSrvDescriptorHeaps(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, int nShaderResourceViews)
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = nConstantBufferViews + nShaderResourceViews; //CBVs + SRVs 
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dCbvSrvDescriptorHeap);

	m_d3dCbvCPUDescriptorNextHandle = m_d3dCbvCPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dCbvGPUDescriptorNextHandle = m_d3dCbvGPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	m_d3dSrvCPUDescriptorNextHandle.ptr = m_d3dSrvCPUDescriptorStartHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
	m_d3dSrvGPUDescriptorNextHandle.ptr = m_d3dSrvGPUDescriptorStartHandle.ptr = m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
}

D3D12_GPU_DESCRIPTOR_HANDLE CShader::CreateConstantBufferViews(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, ID3D12Resource *pd3dConstantBuffers, UINT nStride)
{
	D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle = m_d3dCbvGPUDescriptorNextHandle;
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = pd3dConstantBuffers->GetGPUVirtualAddress();
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	d3dCBVDesc.SizeInBytes = nStride;
	for (int j = 0; j < nConstantBufferViews; j++)
	{
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (nStride * j);
		m_d3dCbvCPUDescriptorNextHandle.ptr = m_d3dCbvCPUDescriptorNextHandle.ptr + ::gnCbvSrvDescriptorIncrementSize;
		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, m_d3dCbvCPUDescriptorNextHandle);
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

D3D12_GPU_DESCRIPTOR_HANDLE CShader::CreateShaderResourceViews(ID3D12Device *pd3dDevice, CTexture *pTexture, UINT nRootParameter, bool bAutoIncrement)
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
			pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_d3dSrvCPUDescriptorNextHandle);
			m_d3dSrvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

			pTexture->SetRootArgument(i, (bAutoIncrement) ? (nRootParameter + i) : nRootParameter, m_d3dSrvGPUDescriptorNextHandle);
			m_d3dSrvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
		}
	}
	return(d3dSrvGPUDescriptorHandle);
}

CObjectsShader::CObjectsShader()
{
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

void CObjectsShader::AnimateObjects(float fTimeElapsed, XMFLOAT4X4 *pxmf4x4Parent)
{
	if (objectList.size())
	{
		for (CGameObject* ob : objectList)
		{
			//ob->Animate(fTimeElapsed, pxmf4x4Parent);
		}
	}
}
void CObjectsShader::AnimateObjects(CGameObject *pOb, float fTimeElapsed)
{
	if (objectList.size())
	{
		for (CGameObject* ob : objectList)
		{
			//ob->Animate(pOb, fTimeElapsed);
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

void CObInstancingShader::CreateShaderVariables(ID3D12Device *pd3dDevice,
	ID3D12GraphicsCommandList *pd3dCommandList)
{
	m_ppObjects->CreateInstanceBuffer(pd3dDevice, pd3dCommandList, objectList.size(), instancedObjectInfo);
}
void CObInstancingShader::ReleaseShaderVariables()
{
	m_ppObjects->ReleaseShaderVariables();
}
//인스턴싱 정보(객체의 월드 변환 행렬과 색상)를 정점 버퍼에 복사한다.
void CObInstancingShader::UpdateShaderVariables(ID3D12GraphicsCommandList
	*pd3dCommandList)
{

	int i = 0;
	if (objectList.size())
	{
		for (CGameObject* ob : objectList)
		{
			ob->UpdateTransform_Instancing(instancedObjectInfo, i, NULL);
			i++;
		}
	}
}


void CObInstancingShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera
	*pCamera)
{
	if (objectList.size() > 0)
	{
		CShader::Render(pd3dCommandList, pCamera);
		//모든 게임 객체의 인스턴싱 데이터를 버퍼에 저장한다. 
		UpdateShaderVariables(pd3dCommandList);
		//하나의 정점 데이터를 사용하여 모든 게임 객체(인스턴스)들을 렌더링한다. 
		m_ppObjects->Render(pd3dCommandList, pCamera, objectList.size());
	}

}
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

//------------------------

CObInstancingMaptoolShader::CObInstancingMaptoolShader()
{
}
CObInstancingMaptoolShader::~CObInstancingMaptoolShader()
{
}

void CObInstancingMaptoolShader::CreateShaderVariables(ID3D12Device *pd3dDevice,
	ID3D12GraphicsCommandList *pd3dCommandList)
{
	for (int i = 0; i < nObject; ++i)
		m_ppObjectss[i]->CreateInstanceBuffer(pd3dDevice, pd3dCommandList, objectLists[i].size(), instancedObjectInfos[i]);

}
void CObInstancingMaptoolShader::ReleaseShaderVariables()
{
	for (int i = 0; i < nObject; ++i)
		m_ppObjectss[i]->ReleaseShaderVariables();

}
//인스턴싱 정보(객체의 월드 변환 행렬과 색상)를 정점 버퍼에 복사한다.
void CObInstancingMaptoolShader::UpdateShaderVariables(ID3D12GraphicsCommandList
	*pd3dCommandList)
{

	for (int i = 0; i < nObject; ++i)
	{
		int j = 0;
		if (objectLists[i].size())
		{
			for (CGameObject* ob : objectLists[i])
			{
				ob->UpdateTransform_Instancing(instancedObjectInfos[i], j, NULL);
				j++;
			}
		}
	}
}


void CObInstancingMaptoolShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera
	*pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);
	for (int i = 0; i < nObject; ++i)
	{
		if (objectLists[i].size() > 0)
		{
			//CShader::Render(pd3dCommandList, pCamera);
			//모든 게임 객체의 인스턴싱 데이터를 버퍼에 저장한다. 
			UpdateShaderVariables(pd3dCommandList);
			//하나의 정점 데이터를 사용하여 모든 게임 객체(인스턴스)들을 렌더링한다. 
			m_ppObjectss[i]->Render(pd3dCommandList, pCamera, objectLists[i].size());
		}
	}

}
void CObInstancingMaptoolShader::ReleaseObjects()
{

	for (int i = 0; i < nObject; ++i)
	{
		if (objectLists[i].size())
		{
			for (CGameObject* ob : objectLists[i])
			{
				ob->Release();
			}
			objectLists[i].clear();
		}
		if (m_ppObjectss[i])
		{
			m_ppObjectss[i]->Release();
			m_ppObjectss[i] = NULL;
		}
	}
}