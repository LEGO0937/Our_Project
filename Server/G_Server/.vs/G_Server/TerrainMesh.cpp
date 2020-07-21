#include <Windows.h>
#include "TerrainMesh.h"

CHeightMapImage::CHeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3
	xmf3Scale)
{
	m_nWidth = nWidth;
	m_nLength = nLength;
	m_xmf3Scale = xmf3Scale;

	BYTE* pHeightMapPixels = new BYTE[m_nWidth * m_nLength];
	////������ ���� �д´�. ���� �� �̹����� ���� ����� ���� RAW �̹����̴�. 
	//HANDLE hFile = ::CreateFile(pFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING,
	//	FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL);
	//DWORD dwBytesRead;
	//::ReadFile(hFile, pHeightMapPixels, (m_nWidth * m_nLength), &dwBytesRead, NULL);
	//::CloseHandle(hFile);
	ifstream in(pFileName, ios::binary);
	in.seekg(0, std::ios::end);
	int n2 = in.tellg();
	in.seekg(0, std::ios::beg);
	in.read((char*)pHeightMapPixels, n2);
	m_pHeightMapPixels = new BYTE[m_nWidth * m_nLength];
	in.close();
	for (int y = 0; y < m_nLength; y++)
	{
		for (int x = 0; x < m_nWidth; x++)
		{
			m_pHeightMapPixels[x + ((m_nLength - 1 - y) * m_nWidth)] = pHeightMapPixels[x +
				(y * m_nWidth)];
		}
	}
	if (pHeightMapPixels) delete[] pHeightMapPixels;
}
CHeightMapImage::~CHeightMapImage()
{
	if (m_pHeightMapPixels) delete[] m_pHeightMapPixels;
	m_pHeightMapPixels = NULL;
}


XMFLOAT3 CHeightMapImage::GetHeightMapNormal(int x, int z)
{
	//x-��ǥ�� z-��ǥ�� ���� ���� ������ ����� ������ ���� ���ʹ� y-�� ���� �����̴�. 
	if ((x < 0.0f) || (z < 0.0f) || (x >= m_nWidth) || (z >= m_nLength))
		return(XMFLOAT3(0.0f, 1.0f, 0.0f));

	int nHeightMapIndex = x + (z * m_nWidth);
	int xHeightMapAdd = (x < (m_nWidth - 1)) ? 1 : -1;
	int zHeightMapAdd = (z < (m_nLength - 1)) ? m_nWidth : -m_nWidth;
	//(x, z), (x+1, z), (z, z+1)�� �ȼ����� ������ ���̸� ���Ѵ�. 
	float y1 = (float)m_pHeightMapPixels[nHeightMapIndex] * m_xmf3Scale.y;
	float y2 = (float)m_pHeightMapPixels[nHeightMapIndex + xHeightMapAdd] * m_xmf3Scale.y;
	float y3 = (float)m_pHeightMapPixels[nHeightMapIndex + zHeightMapAdd] * m_xmf3Scale.y;
	//xmf3Edge1�� (0, y3, m_xmf3Scale.z) - (0, y1, 0) �����̴�. 
	XMFLOAT3 xmf3Edge1 = XMFLOAT3(0.0f, y3 - y1, m_xmf3Scale.z);
	//xmf3Edge2�� (m_xmf3Scale.x, y2, 0) - (0, y1, 0) �����̴�. 
	XMFLOAT3 xmf3Edge2 = XMFLOAT3(m_xmf3Scale.x, y2 - y1, 0.0f);
	//���� ���ʹ� xmf3Edge1�� xmf3Edge2�� ������ ����ȭ�ϸ� �ȴ�. 
	XMFLOAT3 xmf3Normal = Vector3::CrossProduct(xmf3Edge1, xmf3Edge2, true);
	return(xmf3Normal);
}


#define _WITH_APPROXIMATE_OPPOSITE_CORNER
float CHeightMapImage::GetHeight(float fx, float fz)
{
	//������ ��ǥ (fx, fz)�� �̹��� ��ǥ���̴�. ���� ���� x-��ǥ�� z-��ǥ�� ���� ���� ������ ����� ������ ���̴�
	//0�̴�.
	if ((fx < 0.0f) || (fz < 0.0f) || (fx >= m_nWidth) || (fz >= m_nLength)) return(0.0f);
	//���� ���� ��ǥ�� ���� �κа� �Ҽ� �κ��� ����Ѵ�. 
	int x = (int)fx;
	int z = (int)fz;
	float fxPercent = fx - x;
	float fzPercent = fz - z;
	float fBottomLeft = (float)m_pHeightMapPixels[x + (z * m_nWidth)];
	float fBottomRight = (float)m_pHeightMapPixels[(x + 1) + (z * m_nWidth)];
	float fTopLeft = (float)m_pHeightMapPixels[x + ((z + 1) * m_nWidth)];
	float fTopRight = (float)m_pHeightMapPixels[(x + 1) + ((z + 1) * m_nWidth)];
#ifdef _WITH_APPROXIMATE_OPPOSITE_CORNER
	//z-��ǥ�� 1, 3, 5, ...�� ��� �ε����� �����ʿ��� �������� �����ȴ�. 
	bool bRightToLeft = ((z % 2) != 0);
	if (bRightToLeft)
	{
		//������ �ﰢ������ �����ʿ��� ���� �������� �����Ǵ� ����̴�. ���� �׸��� �������� (fzPercent < fxPercent)
		//�� ����̴�. �� ��� TopLeft�� �ȼ� ���� (fTopLeft = fTopRight + (fBottomLeft - fBottomRight))�� �ٻ��Ѵ�.
		//���� �׸��� ������ (fzPercent �� fxPercent)�� ����̴�. �� ��� BottomRight�� �ȼ� ����
		//(fBottomRight = fBottomLeft + (fTopRight - fTopLeft))�� �ٻ��Ѵ�.

		if (fzPercent >= fxPercent)
			fBottomRight = fBottomLeft + (fTopRight - fTopLeft);
		else
			fTopLeft = fTopRight + (fBottomLeft - fBottomRight);
	}
	else
	{
		//������ �ﰢ������ ���ʿ��� ������ �������� �����Ǵ� ����̴�. ���� �׸��� ������ (fzPercent < (1.0f - fxPercent))�� ����̴�. �� ��� TopRight�� �ȼ� ���� (fTopRight = fTopLeft + (fBottomRight - fBottomLeft))��
		//�ٻ��Ѵ�. ���� �׸��� �������� (fzPercent �� (1.0f - fxPercent))�� ����̴�. �� ��� BottomLeft�� �ȼ� ����
		//(fBottomLeft = fTopLeft + (fBottomRight - fTopRight))�� �ٻ��Ѵ�.

		if (fzPercent < (1.0f - fxPercent))
			fTopRight = fTopLeft + (fBottomRight - fBottomLeft);
		else
			fBottomLeft = fTopLeft + (fBottomRight - fTopRight);
	}
#endif
	//�簢���� �� ���� �����Ͽ� ����(�ȼ� ��)�� ����Ѵ�. 
	float fTopHeight = fTopLeft * (1 - fxPercent) + fTopRight * fxPercent;
	float fBottomHeight = fBottomLeft * (1 - fxPercent) + fBottomRight * fxPercent;
	float fHeight = fBottomHeight * (1 - fzPercent) + fTopHeight * fzPercent;
	return(fHeight);
}


CHeightMapGridMesh::CHeightMapGridMesh(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList, int xStart, int zStart, int nWidth, int
	nLength, XMFLOAT3 xmf3Scale, void* pContext) : CMesh(pd3dDevice, pd3dCommandList)
{

	m_nType = VERTEXT_NORMAL_DETAIL;
	m_nVertices = nWidth * nLength;
	m_pxmf3Positions = new XMFLOAT3[m_nVertices];
	m_pxmf3Normals = new XMFLOAT3[m_nVertices];
	m_pxmf2Uvs = new XMFLOAT2[m_nVertices];
	m_pxmf2DetailedUvs = new XMFLOAT2[m_nVertices];

	m_nSubMeshes = 1;

	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	//D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	m_nWidth = nWidth;
	m_nLength = nLength;
	m_xmf3Scale = xmf3Scale;




	//xStart�� zStart�� ������ ���� ��ġ(x-��ǥ�� z-��ǥ)�� ��Ÿ����. Ŀ�ٶ� ������ ���ڵ��� ������ �迭�� ���� ��
	//�䰡 �ֱ� ������ ��ü �������� �� ������ ���� ��ġ�� ��Ÿ���� ������ �ʿ��ϴ�.
	float maxHeight = 0.0f;
	float fHeight = 0.0f, fMinHeight = +FLT_MAX, fMaxHeight = -FLT_MAX;
	CHeightMapImage* pHeightMapImage = (CHeightMapImage*)pContext;
	int cxHeightMap = pHeightMapImage->GetHeightMapWidth();
	int czHeightMap = pHeightMapImage->GetHeightMapLength();
	for (int i = 0, z = zStart; z < (zStart + nLength); z++)
	{
		for (int x = xStart; x < (xStart + nWidth); x++, i++)
		{
#ifndef _WITH_LEFT_HAND_COORDINATES
			float t = OnGetHeight(x, z, pContext);
#else
			float t = OnGetHeight(x, z, pContext);
#endif
			if (maxHeight < t)
				maxHeight = t;
			//������ ���̿� ������ ���� �����κ��� ���Ѵ�. 
			m_pxmf3Positions[i] = XMFLOAT3((x * m_xmf3Scale.x), t,
				(z * m_xmf3Scale.z));

#ifndef _WITH_LEFT_HAND_COORDINATES
			m_pxmf2Uvs[i] = XMFLOAT2(float(x) / float(cxHeightMap - 1), float(czHeightMap - 1 - z) /
				float(czHeightMap - 1));
#else
			m_pxmf2Uvs[i] = XMFLOAT2(float(x) / float(cxHeightMap - 1), float(czHeightMap - 1 - z) /
				float(czHeightMap - 1));
#endif
			m_pxmf2DetailedUvs[i] = XMFLOAT2(float(x) / float(m_xmf3Scale.x * 0.5f), float(z) /
				float(m_xmf3Scale.z * 0.5f));
			//m_pxmf2Uv1[i] = XMFLOAT2(float(x) / float(m_xmf3Scale.x*0.5f), float(z) / float(m_xmf3Scale.z*0.5f));
			//XMFLOAT4 xmf3Color = Vector4::Add(OnGetColor(x, z, pContext), xmf4Color);
			m_pxmf3Normals[i] = pHeightMapImage->GetHeightMapNormal(x, z);

			if (fHeight < fMinHeight) fMinHeight = fHeight;
			if (fHeight > fMaxHeight) fMaxHeight = fHeight;
		}
	}
	m_xmf3AABBCenter = XMFLOAT3(m_xmf3Scale.x * (xStart + nWidth / 2), 0.0f, m_xmf3Scale.z * (zStart + nLength / 2));
	m_xmf3AABBExtents = XMFLOAT3(m_xmf3Scale.x * nWidth / 2, maxHeight * m_xmf3Scale.y, m_xmf3Scale.z * nLength / 2);

	m_xmBoundingBox = BoundingOrientedBox(m_xmf3AABBCenter, m_xmf3AABBExtents, XMFLOAT4(0, 0, 0, 1));
	//vertices.assign(&pVertices[0], &pVertices[m_nVertices]);
	////���� �׸��� ������ ����(����)�� �����ϴ� ������ �����ش�.
	//delete[] pVertices;

	m_pnSubSetIndices = new int[m_nSubMeshes];
	m_ppnSubSetIndices = new UINT * [m_nSubMeshes];
	m_pnSubSetIndices[0] = (nWidth - 1) * (nLength - 1) * 6;
	m_ppnSubSetIndices[0] = new UINT[m_pnSubSetIndices[0]];

	m_ppd3dSubSetIndexBuffers = new ID3D12Resource * [m_nSubMeshes];
	m_ppd3dSubSetIndexUploadBuffers = new ID3D12Resource * [m_nSubMeshes];
	m_pd3dSubSetIndexBufferViews = new D3D12_INDEX_BUFFER_VIEW[m_nSubMeshes];

	//UINT *pnIndices = new UINT[m_nIndices];
	int k = 0;
	for (int i = 0; i < nLength - 1; ++i)
	{
		for (int j = 0; j < nWidth - 1; ++j)
		{
			m_ppnSubSetIndices[0][k] = i * nWidth + j;
			m_ppnSubSetIndices[0][k + 1] = (i + 1) * nWidth + j;
			m_ppnSubSetIndices[0][k + 2] = i * nWidth + j + 1;

			m_ppnSubSetIndices[0][k + 3] = (i + 1) * nWidth + j + 1;
			m_ppnSubSetIndices[0][k + 4] = i * nWidth + j + 1;
			m_ppnSubSetIndices[0][k + 5] = (i + 1) * nWidth + j;

			k += 6; // next quad
		}
	}
}

CHeightMapGridMesh::~CHeightMapGridMesh()
{
}


//���� �� �̹����� �ȼ� ���� ������ ���̷� ��ȯ�Ѵ�. 
float CHeightMapGridMesh::OnGetHeight(int x, int z, void* pContext)
{
	CHeightMapImage* pHeightMapImage = (CHeightMapImage*)pContext;
	BYTE* pHeightMapPixels = pHeightMapImage->GetHeightMapPixels();
	XMFLOAT3 xmf3Scale = pHeightMapImage->GetScale();
	int nWidth = pHeightMapImage->GetHeightMapWidth();
	float fHeight = pHeightMapPixels[x + (z * nWidth)] * xmf3Scale.y;
	return(fHeight);
}
