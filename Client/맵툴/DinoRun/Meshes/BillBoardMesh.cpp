#include "BillBoardMesh.h"
#include "../Common/stdafx.h"

BillBoardMesh::BillBoardMesh(): CMesh()
{
	m_nType = VERTEXT_POSITION | VERTEXT_NORMAL | VERTEXT_TEXTURE_COORD0;
	m_nVertices = 1;
	m_pxmf3Positions = new XMFLOAT3[m_nVertices];
	m_pxmf3Normals = new XMFLOAT3[m_nVertices];
	m_pxmf2Uvs = new XMFLOAT2[m_nVertices];

	m_nSubMeshes = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	m_xmf3AABBCenter = XMFLOAT3(0, 0.0f, 0);
	m_xmf3AABBExtents = XMFLOAT3(150, 150, 10);

	m_pxmf3Positions[0] = XMFLOAT3(0, 0, 0);
	m_pxmf3Normals[0] = XMFLOAT3(0, 0, 1);
	m_pxmf2Uvs[0] = XMFLOAT2(0.5f, 0.5f);
}
BillBoardMesh::~BillBoardMesh()
{
}
