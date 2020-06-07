#include "LiePlaneMesh.h"
#include "../Common/stdafx.h"

LiePlaneMesh::LiePlaneMesh() : CMesh()
{
}
LiePlaneMesh::LiePlaneMesh(float fx, float fy, float fz, float minX, float maxX, float minY, float maxY) : CMesh()
{
	m_nType = VERTEXT_POSITION | VERTEXT_TEXTURE_COORD0;
	m_nVertices = 6;
	m_pxmf3Positions = new XMFLOAT3[m_nVertices];
	m_pxmf2Uvs = new XMFLOAT2[m_nVertices];

	m_nSubMeshes = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_xmf3AABBCenter = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3AABBExtents = XMFLOAT3(fx, 0.01f, fz);
	m_xmBoundingBox = BoundingOrientedBox(m_xmf3AABBCenter, m_xmf3AABBExtents, XMFLOAT4(0, 0, 0, 1));
	int i = 0;

	m_pxmf3Positions[i++] = XMFLOAT3(-fx, fy, fz);
	m_pxmf3Positions[i++] = XMFLOAT3(+fx, fy, fz);
	m_pxmf3Positions[i++] = XMFLOAT3(+fx, fy, -fz);
	m_pxmf3Positions[i++] = XMFLOAT3(+fx, fy, -fz);
	m_pxmf3Positions[i++] = XMFLOAT3(-fx, fy, -fz);
	m_pxmf3Positions[i++] = XMFLOAT3(-fx, fy, fz);


	i = 0;
	m_pxmf2Uvs[i++] = XMFLOAT2(minX, minY);
	m_pxmf2Uvs[i++] = XMFLOAT2(maxX, minY);
	m_pxmf2Uvs[i++] = XMFLOAT2(maxX, maxY);
	m_pxmf2Uvs[i++] = XMFLOAT2(maxX, maxY);
	m_pxmf2Uvs[i++] = XMFLOAT2(minX, maxY);
	m_pxmf2Uvs[i++] = XMFLOAT2(minX, minY);
}
LiePlaneMesh::~LiePlaneMesh()
{
}
