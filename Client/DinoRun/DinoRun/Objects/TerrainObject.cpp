#include "TerrainObject.h"
#include "../CShaders/TerrainShader/TerrainShader.h"
#include "../Common/FrameWork/CreateManager.h"

CHeightMapTerrain::CHeightMapTerrain(CreateManager* pCreateManager, LPCTSTR pFileName, int
	nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale) : CGameObject(1)
{

	CTexture *pTerrainBaseTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pTerrainBaseTexture->LoadTextureFromFile(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), L"Resources/Images/First_Map_Texture.dds", 0);

	CTexture *pTerrainDetailTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pTerrainDetailTexture->LoadTextureFromFile(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), L"Resources/Images/Detail_Texture.dds", 0);

	CTexture *pShadowTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pShadowTexture->SetTexture(pCreateManager->GetShadowBuffer(),0);
	pCreateManager->GetShadowBuffer()->AddRef();
	//----------------------
	//������ ����� ���� ���� ����, ������ ũ���̴�. 
	m_nWidth = nWidth;
	m_nLength = nLength;
	//���� ��ü�� ���� �޽����� �迭�� ���� ���̴�. nBlockWidth, nBlockLength�� ���� �޽� �ϳ��� ����, ���� ũ
	//���̴�. cxQuadsPerBlock, czQuadsPerBlock�� ���� �޽��� ���� ����� ���� ���� �簢���� �����̴�.
	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;
	//xmf3Scale�� ������ ������ �� �� Ȯ���� ���ΰ��� ��Ÿ����. 
	m_xmf3Scale = xmf3Scale;
	//������ ����� ���� ���� �����Ѵ�. 
	m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);

	//�������� ���� ����, ���� �������� ���� �޽��� �� ���� �ִ� ���� ��Ÿ����. 
	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;

	//���� ��ü�� ǥ���ϱ� ���� ���� �޽��� ���� ������ �迭�� �����Ѵ�. 
	CGameObject* Object = NULL;
	CMaterial *material = NULL;
	TerrainShader *pShader = new TerrainShader();
	pShader->CreateCbvSrvDescriptorHeaps(pCreateManager, 0, 3);
	pShader->CreateShaderResourceViews(pCreateManager, pTerrainBaseTexture, 8, true);
	pShader->CreateShaderResourceViews(pCreateManager, pTerrainDetailTexture, 9, true);
	pShader->CreateShadowResourceViews(pCreateManager, pShadowTexture, 10, true);

	material = new CMaterial(3);
	material->m_xmf4AmbientColor = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	material->m_xmf4DiffuseColor = XMFLOAT4(0.5f, 0.7f, 0.5f, 1.0f);
	
	material->SetShader(pShader);
	material->SetTexture(pTerrainBaseTexture, 1);
	material->SetTexture(pTerrainDetailTexture, 0);
	material->SetTexture(pShadowTexture, 2);

	
	material->CreateShaderVariable(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get());

	SetMaterial(0, material);

	for (int z = 0, zStart = 0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			//������ �Ϻκ��� ��Ÿ���� ���� �޽��� ���� ��ġ(��ǥ)�̴�. 

			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);
			//������ �Ϻκ��� ��Ÿ���� ���� �޽��� �����Ͽ� ���� �޽��� �����Ѵ�.
			CHeightMapGridMesh *pMeshInfo = new CHeightMapGridMesh(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), xStart,
				zStart, nBlockWidth, nBlockLength, xmf3Scale, m_pHeightMapImage);

			pMeshInfo->CreateShaderVariables(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get());

			Object = new CGameObject(1);

			Object->SetMesh(pMeshInfo);
			SetChild(Object);

		}
	}
	CreateBuffer(pCreateManager);
}
CHeightMapTerrain::~CHeightMapTerrain(void)
{
	if (m_pHeightMapImage) 
		delete m_pHeightMapImage;

}

void CHeightMapTerrain::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	OnPrepareRender();

	if (m_nMaterials > 0)
	{
		for (int i = 0; i < m_nMaterials; ++i)
		{
			if (m_ppMaterials[i])
			{
				if (m_ppMaterials[i]->m_pShader)
				{
					m_ppMaterials[i]->m_pShader->Render(pd3dCommandList, pCamera);
				}

				m_ppMaterials[i]->UpdateShaderVariable(pd3dCommandList);
			}

			if (m_pMesh) m_pMesh->Render(pd3dCommandList, i);
		}
	}
	if (m_pSibling) m_pSibling->Render(pd3dCommandList, pCamera);
	if (m_pChild) m_pChild->Render(pd3dCommandList, pCamera);
}
