//-----------------------------------------------------------------------------
// File: CGameObject.cpp
//-----------------------------------------------------------------------------

#include "../Common/stdafx.h"
#include "Mesh.h"
#include "../Objects/BaseObject.h"

CMesh::CMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
}

CMesh::~CMesh()
{
	if (m_nSubMeshes > 0)
	{
		for (int i = 0; i < m_nSubMeshes; i++)
		{
			if (m_ppd3dSubSetIndexBuffers[i]) m_ppd3dSubSetIndexBuffers[i]->Release();
			if (m_ppnSubSetIndices[i]) delete[] m_ppnSubSetIndices[i];
		}
		if (m_ppd3dSubSetIndexBuffers)
		{
			delete[] m_ppd3dSubSetIndexBuffers;
			m_ppd3dSubSetIndexBuffers = NULL;
		}
		if (m_pd3dSubSetIndexBufferViews)
		{
			delete[] m_pd3dSubSetIndexBufferViews;
			m_pd3dSubSetIndexBufferViews = NULL;
		}
		if (m_pnSubSetIndices)
		{
			delete[] m_pnSubSetIndices;
			m_pnSubSetIndices = NULL;
		}
		if (m_ppnSubSetIndices)
		{
			delete[] m_ppnSubSetIndices;
			m_ppnSubSetIndices = NULL;
		}
	}

	if (m_pxmf3Positions) delete[] m_pxmf3Positions;
	if (m_pxmf3Normals) delete[] m_pxmf3Normals;
	if (m_pxmf2Uvs) delete[] m_pxmf2Uvs;
	if (m_pxmf2Uvs) delete[] m_pxmf2DetailedUvs;
}


void CMesh::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (m_nType & VERTEXT_POSITION)
	{
		m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);
		m_pd3dPositionBuffer->SetName(L"PositionBuf");
		m_pd3dPositionUploadBuffer->SetName(L"positionUpload");
		m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
		m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
		m_d3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
	}
	if (m_nType & VERTEXT_NORMAL)
	{
		m_pd3dNormalBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Normals, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dNormalUploadBuffer);
		m_pd3dNormalBuffer->SetName(L"NormalBuf");
		m_pd3dNormalUploadBuffer->SetName(L"NormalUpload");
		m_d3dNormalBufferView.BufferLocation = m_pd3dNormalBuffer->GetGPUVirtualAddress();
		m_d3dNormalBufferView.StrideInBytes = sizeof(XMFLOAT3);
		m_d3dNormalBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
	}
	if (m_nType & VERTEXT_TEXTURE_COORD0)
	{
		m_pd3dUvBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf2Uvs, sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dUvUploadBuffer);
		m_pd3dUvBuffer->SetName(L"UvBuf");
		m_pd3dUvUploadBuffer->SetName(L"Uv0Upload");
		m_d3dUvBufferView.BufferLocation = m_pd3dUvBuffer->GetGPUVirtualAddress();
		m_d3dUvBufferView.StrideInBytes = sizeof(XMFLOAT2);
		m_d3dUvBufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;
	}
	if (m_nType & VERTEXT_TEXTURE_COORD1)
	{
		m_pd3dDetailedUvBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf2DetailedUvs, sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dDetailedUvUploadBuffer);
		m_pd3dDetailedUvBuffer->SetName(L"DetailBuf");
		m_pd3dDetailedUvUploadBuffer->SetName(L"DetailUpload");
		m_d3dDetailedUvBufferView.BufferLocation = m_pd3dDetailedUvBuffer->GetGPUVirtualAddress();
		m_d3dDetailedUvBufferView.StrideInBytes = sizeof(XMFLOAT2);
		m_d3dDetailedUvBufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;
	}

	for (int i = 0; i < m_nSubMeshes; ++i)
	{

		if (m_pnSubSetIndices[i] > 0)
		{
			m_ppd3dSubSetIndexBuffers[i] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_ppnSubSetIndices[i], sizeof(UINT) * m_pnSubSetIndices[i], D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_ppd3dSubSetIndexUploadBuffers[i]);
			m_ppd3dSubSetIndexBuffers[i]->SetName(L"IndexBuf");
			m_ppd3dSubSetIndexUploadBuffers[i]->SetName(L"IndexUpload");
			
			m_pd3dSubSetIndexBufferViews[i].BufferLocation = m_ppd3dSubSetIndexBuffers[i]->GetGPUVirtualAddress();
			m_pd3dSubSetIndexBufferViews[i].Format = DXGI_FORMAT_R32_UINT;
			m_pd3dSubSetIndexBufferViews[i].SizeInBytes = sizeof(UINT) * m_pnSubSetIndices[i];
		}
	}
}
void CMesh::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (m_nType && VERTEXT_NORMAL_TEXTURE)
	{

	}
	else if (m_nType && VERTEXT_DEFAULT)
	{
	}
}
void CMesh::ReleaseShaderVariables()
{
	if (m_pd3dPositionBuffer)
	{
		m_pd3dPositionBuffer->Release();
		m_pd3dPositionBuffer = NULL;
	}
	if (m_pd3dNormalBuffer)
	{
		m_pd3dNormalBuffer->Release();
		m_pd3dNormalBuffer = NULL;
	}
	if (m_pd3dUvBuffer)
	{
		m_pd3dUvBuffer->Release();
		m_pd3dUvBuffer = NULL;
	}
	if (m_pd3dDetailedUvBuffer)
	{
		m_pd3dDetailedUvBuffer->Release();
		m_pd3dDetailedUvBuffer = NULL;
	}
}

void CMesh::ReleaseUploadBuffers()
{
	if (m_pd3dPositionUploadBuffer) m_pd3dPositionUploadBuffer->Release();
	m_pd3dPositionUploadBuffer = NULL;

	if (m_pd3dUvUploadBuffer)
		m_pd3dUvUploadBuffer->Release();
	m_pd3dUvUploadBuffer = NULL;

	if (m_pd3dNormalUploadBuffer) m_pd3dNormalUploadBuffer->Release();
	m_pd3dNormalUploadBuffer = NULL;

	if (m_pd3dDetailedUvUploadBuffer)	m_pd3dDetailedUvUploadBuffer->Release();
	m_pd3dDetailedUvUploadBuffer = NULL;

	if ((m_nSubMeshes > 0) && m_ppd3dSubSetIndexUploadBuffers)
	{
		for (int i = 0; i < m_nSubMeshes; i++)
		{
			if (m_ppd3dSubSetIndexUploadBuffers[i]) m_ppd3dSubSetIndexUploadBuffers[i]->Release();
		}
		if (m_ppd3dSubSetIndexUploadBuffers) delete[] m_ppd3dSubSetIndexUploadBuffers;
		m_ppd3dSubSetIndexUploadBuffers = NULL;
	}
}

void CMesh::OnPreRender(ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	if (m_nType == VERTEXT_POSITION)
	{
		D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[1] = { m_d3dPositionBufferView };
		pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, pVertexBufferViews);
	}
	else if (m_nType == VERTEXT_NORMAL_TEXTURE)
	{
		D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[3] = { m_d3dPositionBufferView, m_d3dNormalBufferView, m_d3dUvBufferView };
		pd3dCommandList->IASetVertexBuffers(m_nSlot, 3, pVertexBufferViews);
	}
	else if (m_nType == VERTEXT_NORMAL_DETAIL)
	{
		D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[4] = { m_d3dPositionBufferView, m_d3dNormalBufferView, m_d3dUvBufferView, m_d3dDetailedUvBufferView };
		pd3dCommandList->IASetVertexBuffers(m_nSlot, 4, pVertexBufferViews);
	}
	else if (m_nType == VERTEXT_TEXTURE)
	{
		D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[2] = { m_d3dPositionBufferView, m_d3dUvBufferView };
		pd3dCommandList->IASetVertexBuffers(m_nSlot, 2, pVertexBufferViews);
	}
	else
	{
		D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[2] = { m_d3dPositionBufferView, m_d3dNormalBufferView };
		pd3dCommandList->IASetVertexBuffers(m_nSlot, 2, pVertexBufferViews);
	}
}

void CMesh::Render(ID3D12GraphicsCommandList *pd3dCommandList, int nSubSet)
{
	UpdateShaderVariables(pd3dCommandList);

	OnPreRender(pd3dCommandList, NULL);

	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	if ((m_nSubMeshes > 0) && (nSubSet < m_nSubMeshes))
	{
		pd3dCommandList->IASetIndexBuffer(&(m_pd3dSubSetIndexBufferViews[nSubSet]));
		pd3dCommandList->DrawIndexedInstanced(m_pnSubSetIndices[nSubSet], 1, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}

void CMesh::Render(ID3D12GraphicsCommandList *pd3dCommandList, int nSubSet, int nInstance)
{
	UpdateShaderVariables(pd3dCommandList);

	OnPreRender(pd3dCommandList, NULL);

	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	if ((m_nSubMeshes > 0) && (nSubSet < m_nSubMeshes))
	{
		pd3dCommandList->IASetIndexBuffer(&(m_pd3dSubSetIndexBufferViews[nSubSet]));
		pd3dCommandList->DrawIndexedInstanced(m_pnSubSetIndices[nSubSet], nInstance, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, nInstance, m_nOffset, 0);
	}
}
#ifdef _WITH_BOUND_BOX
void CMesh::BbxRender(ID3D12GraphicsCommandList *pd3dCommandList)
{
	pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dBoundBoxBufferView);
	pd3dCommandList->IASetIndexBuffer(&(m_pd3dBbxIndexBufferView));
	pd3dCommandList->DrawIndexedInstanced(24, 1, 0, 0, 0);
}

void CMesh::BbxRender(ID3D12GraphicsCommandList *pd3dCommandList, int nInstance)
{
	pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dBoundBoxBufferView);
	pd3dCommandList->IASetIndexBuffer(&(m_pd3dBbxIndexBufferView));
	pd3dCommandList->DrawIndexedInstanced(24, nInstance, 0, 0, 0);

}

void CMesh::ComputeBoundBox(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	float x = m_xmf3AABBExtents.x;
	float y = m_xmf3AABBExtents.y;
	float z = m_xmf3AABBExtents.z;
	m_pxmf3BoundVertex = new XMFLOAT3[8];
	m_pxmf3BoundVertex[0] = Vector3::Add(m_xmf3AABBCenter, XMFLOAT3(x, -y, z));
	m_pxmf3BoundVertex[1] = Vector3::Add(m_xmf3AABBCenter, XMFLOAT3(-x, -y, z));
	m_pxmf3BoundVertex[2] = Vector3::Add(m_xmf3AABBCenter, XMFLOAT3(-x, y, z));
	m_pxmf3BoundVertex[3] = Vector3::Add(m_xmf3AABBCenter, XMFLOAT3(x, y, z));
	m_pxmf3BoundVertex[4] = Vector3::Add(m_xmf3AABBCenter, XMFLOAT3(x, -y, -z));
	m_pxmf3BoundVertex[5] = Vector3::Add(m_xmf3AABBCenter, XMFLOAT3(-x, -y, -z));
	m_pxmf3BoundVertex[6] = Vector3::Add(m_xmf3AABBCenter, XMFLOAT3(-x, y, -z));
	m_pxmf3BoundVertex[7] = Vector3::Add(m_xmf3AABBCenter, XMFLOAT3(x, y, -z));

	m_pd3dBoundBoxBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3BoundVertex, sizeof(XMFLOAT3) * 8, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dBoundBoxUploadBuffer);

	m_d3dBoundBoxBufferView.BufferLocation = m_pd3dBoundBoxBuffer->GetGPUVirtualAddress();
	m_d3dBoundBoxBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dBoundBoxBufferView.SizeInBytes = sizeof(XMFLOAT3) * 8;

	m_ppnBbxIndices = new UINT[24];
	m_ppnBbxIndices[0] = 0, m_ppnBbxIndices[1] = 1, m_ppnBbxIndices[2] = 0;
	m_ppnBbxIndices[3] = 3, m_ppnBbxIndices[4] = 0, m_ppnBbxIndices[5] = 4;
	m_ppnBbxIndices[6] = 1, m_ppnBbxIndices[7] = 2, m_ppnBbxIndices[8] = 1;
	m_ppnBbxIndices[9] = 5, m_ppnBbxIndices[10] = 2, m_ppnBbxIndices[11] = 3;
	m_ppnBbxIndices[12] = 3, m_ppnBbxIndices[13] = 7, m_ppnBbxIndices[14] = 4;
	m_ppnBbxIndices[15] = 7, m_ppnBbxIndices[16] = 5, m_ppnBbxIndices[17] = 6;
	m_ppnBbxIndices[18] = 2, m_ppnBbxIndices[19] = 6, m_ppnBbxIndices[20] = 5;
	m_ppnBbxIndices[21] = 4, m_ppnBbxIndices[22] = 6, m_ppnBbxIndices[23] = 7;

	m_ppd3dBbxIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_ppnBbxIndices, sizeof(XMFLOAT3) * 24, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_ppd3dBbxIndexUploadBuffer);

	m_pd3dBbxIndexBufferView.BufferLocation = m_ppd3dBbxIndexBuffer->GetGPUVirtualAddress();
	m_pd3dBbxIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_pd3dBbxIndexBufferView.SizeInBytes = sizeof(UINT) * 24;

}
#endif

void CMesh::LoadMeshFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, FILE *pInFile)
{
	char pstrToken[64] = { '\0' };
	BYTE nStrLength = 0;
	UINT nReads = 0;

	int nColors = 0, nIndices = 0, nSubMeshes = 0, nSubIndices = 0;

	::ReadStringFromFile(pInFile, m_pstrMeshName);

	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);

		if (!strcmp(pstrToken, "<Bounds>:"))
		{
			nReads = (UINT)::fread(&m_xmf3AABBCenter, sizeof(XMFLOAT3), 1, pInFile);
			nReads = (UINT)::fread(&m_xmf3AABBExtents, sizeof(XMFLOAT3), 1, pInFile);
			m_xmBoundingBox = BoundingOrientedBox(m_xmf3AABBCenter, m_xmf3AABBExtents, XMFLOAT4(0, 0, 0, 1));
#ifdef _WITH_BOUND_BOX
			ComputeBoundBox(pd3dDevice, pd3dCommandList);
#endif
		}
		else if (!strcmp(pstrToken, "<ControlPoints>:"))
		{
			nReads = (UINT)::fread(&m_nVertices, sizeof(int), 1, pInFile);
			if (m_nVertices > 0)
			{
				m_nType |= VERTEXT_POSITION;
				m_pxmf3Positions = new XMFLOAT3[m_nVertices];
				nReads = (UINT)::fread(m_pxmf3Positions, sizeof(XMFLOAT3), m_nVertices, pInFile);
				
				m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

				m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
				m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_d3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
			}

		}
		else if (!strcmp(pstrToken, "<Normals>:"))
		{
			nReads = (UINT)::fread(&m_nVertices, sizeof(int), 1, pInFile);
			if (m_nVertices > 0)
			{
				m_nType |= VERTEXT_NORMAL;
				m_pxmf3Normals = new XMFLOAT3[m_nVertices];
				nReads = (UINT)::fread(m_pxmf3Normals, sizeof(XMFLOAT3), m_nVertices, pInFile);

				m_pd3dNormalBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Normals, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dNormalUploadBuffer);

				m_d3dNormalBufferView.BufferLocation = m_pd3dNormalBuffer->GetGPUVirtualAddress();
				m_d3dNormalBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_d3dNormalBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "<UVs>:"))
		{
			nReads = (UINT)::fread(&m_nVertices, sizeof(int), 1, pInFile);
			if (m_nVertices > 0)
			{
				m_nType |= VERTEXT_TEXTURE_COORD0;
				m_pxmf2Uvs = new XMFLOAT2[m_nVertices];
				nReads = (UINT)::fread(m_pxmf2Uvs, sizeof(XMFLOAT2), m_nVertices, pInFile);
				
				m_pd3dUvBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf2Uvs, sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dUvUploadBuffer);

				m_d3dUvBufferView.BufferLocation = m_pd3dUvBuffer->GetGPUVirtualAddress();
				m_d3dUvBufferView.StrideInBytes = sizeof(XMFLOAT2);
				m_d3dUvBufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;
			}

		}
		else if (!strcmp(pstrToken, "<Polygons>:"))
		{
			int nPolygons = 0;
			nReads = (UINT)::fread(&nPolygons, sizeof(int), 1, pInFile);

			for (; ; )
			{
				::ReadStringFromFile(pInFile, pstrToken);

				if (!strcmp(pstrToken, "<Indices>:"))
				{
					
					nReads = (UINT)::fread(&nPolygons, sizeof(int), 1, pInFile);
					nReads = (UINT)::fread(&m_nSubMeshes, sizeof(int), 1, pInFile);

					m_pnSubSetIndices = new int[m_nSubMeshes];
					m_ppnSubSetIndices = new UINT*[m_nSubMeshes];

					m_ppd3dSubSetIndexBuffers = new ID3D12Resource*[m_nSubMeshes];
					m_ppd3dSubSetIndexUploadBuffers = new ID3D12Resource*[m_nSubMeshes];
					m_pd3dSubSetIndexBufferViews = new D3D12_INDEX_BUFFER_VIEW[m_nSubMeshes];

					for (int i = 0; i < m_nSubMeshes; ++i)
					{
						::ReadStringFromFile(pInFile, pstrToken);
						nReads = (UINT)::fread(&(m_pnSubSetIndices[i]), sizeof(int), 1, pInFile);
						if (m_pnSubSetIndices[i] > 0)
						{
							m_ppnSubSetIndices[i] = new UINT[m_pnSubSetIndices[i]];
							nReads = (UINT)::fread(m_ppnSubSetIndices[i], sizeof(UINT), m_pnSubSetIndices[i], pInFile);

							m_ppd3dSubSetIndexBuffers[i] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_ppnSubSetIndices[i], sizeof(UINT) * m_pnSubSetIndices[i], D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_ppd3dSubSetIndexUploadBuffers[i]);

							m_pd3dSubSetIndexBufferViews[i].BufferLocation = m_ppd3dSubSetIndexBuffers[i]->GetGPUVirtualAddress();
							m_pd3dSubSetIndexBufferViews[i].Format = DXGI_FORMAT_R32_UINT;
							m_pd3dSubSetIndexBufferViews[i].SizeInBytes = sizeof(UINT) * m_pnSubSetIndices[i];
						}
					}
				}
				else if (!strcmp(pstrToken, "<ControlPoints>:"))
				{
					nReads = (UINT)::fread(&m_nVertices, sizeof(int), 1, pInFile);
					if (m_nVertices > 0)
					{
						m_nType |= VERTEXT_POSITION;
						m_pxmf3Positions = new XMFLOAT3[m_nVertices];
						nReads = (UINT)::fread(m_pxmf3Positions, sizeof(XMFLOAT3), m_nVertices, pInFile);
						
						m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

						m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
						m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
						m_d3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

					}

				}
				else if (!strcmp(pstrToken, "<Normals>:"))
				{
					nReads = (UINT)::fread(&m_nVertices, sizeof(int), 1, pInFile);
					if (m_nVertices > 0)
					{
						m_nType |= VERTEXT_NORMAL;
						m_pxmf3Normals = new XMFLOAT3[m_nVertices];
						nReads = (UINT)::fread(m_pxmf3Normals, sizeof(XMFLOAT3), m_nVertices, pInFile);

						m_pd3dNormalBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Normals, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dNormalUploadBuffer);

						m_d3dNormalBufferView.BufferLocation = m_pd3dNormalBuffer->GetGPUVirtualAddress();
						m_d3dNormalBufferView.StrideInBytes = sizeof(XMFLOAT3);
						m_d3dNormalBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
					}
				}
				else if (!strcmp(pstrToken, "<UVs>:"))
				{
					nReads = (UINT)::fread(&m_nVertices, sizeof(int), 1, pInFile);
					if (m_nVertices > 0)
					{
						m_nType |= VERTEXT_TEXTURE_COORD0;
						m_pxmf2Uvs = new XMFLOAT2[m_nVertices];
						nReads = (UINT)::fread(m_pxmf2Uvs, sizeof(XMFLOAT2), m_nVertices, pInFile);
						
						m_pd3dUvBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf2Uvs, sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dUvUploadBuffer);

						m_d3dUvBufferView.BufferLocation = m_pd3dUvBuffer->GetGPUVirtualAddress();
						m_d3dUvBufferView.StrideInBytes = sizeof(XMFLOAT2);
						m_d3dUvBufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;
					}

				}
				else if (!strcmp(pstrToken, "</Polygons>"))
				{
					break;
				}
			}
		}
		else if (!strcmp(pstrToken, "</Mesh>"))
		{
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
CSkinnedMesh::CSkinnedMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) : CMesh(pd3dDevice, pd3dCommandList)
{
}

CSkinnedMesh::~CSkinnedMesh()
{
	if (m_pxmn4BoneIndices) delete[] m_pxmn4BoneIndices;
	if (m_pxmf4BoneWeights) delete[] m_pxmf4BoneWeights;

	if (m_ppSkinningBoneFrameCaches) delete[] m_ppSkinningBoneFrameCaches;
	if (m_pxmf4x4BindPoseBoneOffsets) delete[] m_pxmf4x4BindPoseBoneOffsets;
	if (m_pd3dcbBindPoseBoneOffsets)
	{
		m_pd3dcbBindPoseBoneOffsets->Unmap(0, NULL);
		m_pd3dcbBindPoseBoneOffsets->Release();
	}
	if (m_ppstrSkinningBoneNames) delete[] m_ppstrSkinningBoneNames;

	if (m_pd3dBoneIndexBuffer) m_pd3dBoneIndexBuffer->Release();
	if (m_pd3dBoneWeightBuffer) m_pd3dBoneWeightBuffer->Release();


	ReleaseShaderVariables();
}

void CSkinnedMesh::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
}

void CSkinnedMesh::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (m_pd3dcbBindPoseBoneOffsets)
	{
		D3D12_GPU_VIRTUAL_ADDRESS d3dcbBoneOffsetsGpuVirtualAddress = m_pd3dcbBindPoseBoneOffsets->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(6, d3dcbBoneOffsetsGpuVirtualAddress); //Skinned Bone Offsets
	}
}

void CSkinnedMesh::ReleaseShaderVariables()
{
	CMesh::ReleaseShaderVariables();
}

void CSkinnedMesh::ReleaseUploadBuffers()
{
	CMesh::ReleaseUploadBuffers();

	if (m_pd3dBoneIndexUploadBuffer) m_pd3dBoneIndexUploadBuffer->Release();
	m_pd3dBoneIndexUploadBuffer = NULL;

	if (m_pd3dBoneWeightUploadBuffer) m_pd3dBoneWeightUploadBuffer->Release();
	m_pd3dBoneWeightUploadBuffer = NULL;
}

void CSkinnedMesh::PrepareSkinning(CGameObject *pModelRootObject)
{
	m_ppSkinningBoneFrameCaches = new CGameObject*[m_nSkinningBones];
	for (int j = 0; j < m_nSkinningBones; j++)
	{
		m_ppSkinningBoneFrameCaches[j] = pModelRootObject->FindFrame(m_ppstrSkinningBoneNames[j]);
	}
}

void CSkinnedMesh::LoadSkinDeformationsFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, FILE *pInFile)
{
	char pstrToken[64] = { '\0' };
	BYTE nStrLength = 0;
	UINT nReads = 0;

	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);
		if (!strcmp(pstrToken, "<BonesPerVertex>:"))
		{
			m_nBonesPerVertex = ::ReadIntegerFromFile(pInFile);
		}
		else if (!strcmp(pstrToken, "<Bounds>:"))
		{
			nReads = (UINT)::fread(&m_xmf3AABBCenter, sizeof(XMFLOAT3), 1, pInFile);
			nReads = (UINT)::fread(&m_xmf3AABBExtents, sizeof(XMFLOAT3), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<BoneNames>:"))
		{
			m_nSkinningBones = ::ReadIntegerFromFile(pInFile);
			if (m_nSkinningBones > 0)
			{
				m_ppstrSkinningBoneNames = new char[m_nSkinningBones][64];
				m_ppSkinningBoneFrameCaches = new CGameObject*[m_nSkinningBones];
				for (int i = 0; i < m_nSkinningBones; i++)
				{
					::ReadStringFromFile(pInFile, m_ppstrSkinningBoneNames[i]);
					m_ppSkinningBoneFrameCaches[i] = NULL;
				}
			}
		}
		else if (!strcmp(pstrToken, "<BoneOffsets>:"))
		{
			m_nSkinningBones = ::ReadIntegerFromFile(pInFile);
			if (m_nSkinningBones > 0)
			{
				m_pxmf4x4BindPoseBoneOffsets = new XMFLOAT4X4[m_nSkinningBones];
				nReads = (UINT)::fread(m_pxmf4x4BindPoseBoneOffsets, sizeof(XMFLOAT4X4), m_nSkinningBones, pInFile);

				UINT ncbElementBytes = (((sizeof(XMFLOAT4X4) * SKINNED_ANIMATION_BONES) + 255) & ~255); //256ÀÇ ¹è¼ö
				m_pd3dcbBindPoseBoneOffsets = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
				m_pd3dcbBindPoseBoneOffsets->Map(0, NULL, (void **)&m_pcbxmf4x4MappedBindPoseBoneOffsets);

				for (int i = 0; i < m_nSkinningBones; i++)
				{
					XMStoreFloat4x4(&m_pcbxmf4x4MappedBindPoseBoneOffsets[i], XMMatrixTranspose(XMLoadFloat4x4(&m_pxmf4x4BindPoseBoneOffsets[i])));
				}
			}
		}
		else if (!strcmp(pstrToken, "<BoneIndices>:"))
		{
			m_nType |= VERTEXT_BONE_INDEX_WEIGHT;

			m_nVertices = ::ReadIntegerFromFile(pInFile);
			if (m_nVertices > 0)
			{
				m_pxmn4BoneIndices = new XMINT4[m_nVertices];

				nReads = (UINT)::fread(m_pxmn4BoneIndices, sizeof(XMINT4), m_nVertices, pInFile);
				m_pd3dBoneIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmn4BoneIndices, sizeof(XMINT4) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dBoneIndexUploadBuffer);

				m_d3dBoneIndexBufferView.BufferLocation = m_pd3dBoneIndexBuffer->GetGPUVirtualAddress();
				m_d3dBoneIndexBufferView.StrideInBytes = sizeof(XMINT4);
				m_d3dBoneIndexBufferView.SizeInBytes = sizeof(XMINT4) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "<BoneWeights>:"))
		{
			m_nType |= VERTEXT_BONE_INDEX_WEIGHT;

			m_nVertices = ::ReadIntegerFromFile(pInFile);
			if (m_nVertices > 0)
			{
				m_pxmf4BoneWeights = new XMFLOAT4[m_nVertices];

				nReads = (UINT)::fread(m_pxmf4BoneWeights, sizeof(XMFLOAT4), m_nVertices, pInFile);
				m_pd3dBoneWeightBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf4BoneWeights, sizeof(XMFLOAT4) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dBoneWeightUploadBuffer);

				m_d3dBoneWeightBufferView.BufferLocation = m_pd3dBoneWeightBuffer->GetGPUVirtualAddress();
				m_d3dBoneWeightBufferView.StrideInBytes = sizeof(XMFLOAT4);
				m_d3dBoneWeightBufferView.SizeInBytes = sizeof(XMFLOAT4) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "</SkinDeformations>"))
		{
			break;
		}
	}
}

void CSkinnedMesh::OnPreRender(ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[5] = { m_d3dPositionBufferView, m_d3dNormalBufferView, m_d3dUvBufferView ,m_d3dBoneIndexBufferView, m_d3dBoneWeightBufferView };
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 5, pVertexBufferViews);
}