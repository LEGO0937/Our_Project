#pragma once
#include "BaseObject.h"
#include "../Meshes/TerrainMesh.h"

class CHeightMapTerrain : public CGameObject
{
public:
	CHeightMapTerrain(shared_ptr<CreateManager> pCreateManager, LPCTSTR pFileName, int
		nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale);
	virtual ~CHeightMapTerrain();
private:
	//������ ���� ������ ����� �̹����̴�. 
	CHeightMapImage *m_pHeightMapImage;
	//���� ���� ���ο� ���� ũ���̴�. 
	int m_nWidth;
	int m_nLength;
	//������ ������ �� �� Ȯ���� ���ΰ��� ��Ÿ���� ������ �����̴�. 
	XMFLOAT3 m_xmf3Scale;
public:
	//������ ���̸� ����ϴ� �Լ��̴�(���� ��ǥ��). ���� ���� ���̿� �������� y�� ���� ���̴�. 
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);

	float GetHeight(float x, float z) {
		return(m_pHeightMapImage->GetHeight(x /
			m_xmf3Scale.x, m_nLength - (z / m_xmf3Scale.z) - 1) * m_xmf3Scale.y);
	}
	//������ ���� ���͸� ����ϴ� �Լ��̴�(���� ��ǥ��). ���� ���� ���� ���͸� ����Ѵ�. 
	XMFLOAT3 GetNormal(float x, float z) {
		return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z /
			m_xmf3Scale.z)));
	}
	int GetHeightMapWidth() { return(m_pHeightMapImage->GetHeightMapWidth()); }
	int GetHeightMapLength() { return(m_pHeightMapImage->GetHeightMapLength()); }
	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	//������ ũ��(����/����)�� ��ȯ�Ѵ�. ���� ���� ũ�⿡ �������� ���� ���̴�. 
	float GetWidth() { return(m_nWidth * m_xmf3Scale.x); }
	float GetLength() { return(m_nLength * m_xmf3Scale.z); }
};
