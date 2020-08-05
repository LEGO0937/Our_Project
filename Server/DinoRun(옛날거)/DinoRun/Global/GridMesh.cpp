#include "GridMesh.h"


GridMesh::GridMesh(float width, float height, int numX, int numY)
{

	m_nType = VERTEXT_NORMAL_TEXTURE;
	int vertexCount = numX * numY;
	m_nVertices = vertexCount;
	m_pxmf3Positions = new XMFLOAT3[m_nVertices];
	m_pxmf3Normals = new XMFLOAT3[m_nVertices];
	m_pxmf2Uvs = new XMFLOAT2[m_nVertices];

	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_nSubMeshes = 1;

	int faceCount = (numX - 1)*(numY - 1) * 2;

	//
	// Create the vertices.
	//

	float halfWidth = 0.5f*width;
	float halfHeight = 0.5f*height;

	float dx = width / (numY - 1);
	float dz = height / (numX - 1);

	float du = 1.0f / (numY - 1);
	float dv = 1.0f / (numX - 1);

	//meshData.Vertices.resize(vertexCount);
	for (int i = 0; i < numX; ++i)
	{
		float z = halfHeight - i * dz;
		for (int j = 0; j < numY; ++j)
		{
			float x = -halfWidth + j * dx;

			m_pxmf3Positions[i*numY + j] = XMFLOAT3(x, 0.0f, z);
			m_pxmf3Normals[i*numY + j] = XMFLOAT3(0.0f, 1.0f, 0.0f);
			

			// Stretch texture over grid.
			m_pxmf2Uvs[i*numY + j].x = j * du;
			m_pxmf2Uvs[i*numY + j].y = i * dv;
		}
	}

	//
	// Create the indices.
	//

	//meshData.Indices32.resize(faceCount * 3); // 3 indices per face

	// Iterate over each quad and compute indices.
	m_pnSubSetIndices = new int[m_nSubMeshes];
	m_ppnSubSetIndices = new UINT*[m_nSubMeshes];
	m_pnSubSetIndices[0] = faceCount * 3;
	m_ppnSubSetIndices[0] = new UINT[m_pnSubSetIndices[0]];

	m_ppd3dSubSetIndexBuffers = new ID3D12Resource*[m_nSubMeshes];
	m_ppd3dSubSetIndexUploadBuffers = new ID3D12Resource*[m_nSubMeshes];
	m_pd3dSubSetIndexBufferViews = new D3D12_INDEX_BUFFER_VIEW[m_nSubMeshes];

	int k = 0;
	for (int i = 0; i < numX - 1; ++i)
	{
		for (int j = 0; j < numY - 1; ++j)
		{
			m_ppnSubSetIndices[0][k] = i * numY + j;
			m_ppnSubSetIndices[0][k + 1] = i * numY + j + 1;
			m_ppnSubSetIndices[0][k + 2] = (i + 1)*numY + j;
			
			m_ppnSubSetIndices[0][k + 3] = (i + 1)*numY + j;
			m_ppnSubSetIndices[0][k + 4] = i * numY + j + 1;
			m_ppnSubSetIndices[0][k + 5] = (i + 1)*numY + j + 1;

			k += 6; // next quad
		}
	}

	//return meshData;
}