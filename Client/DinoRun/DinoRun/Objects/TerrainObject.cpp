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
	//지형에 사용할 높이 맵의 가로, 세로의 크기이다. 
	m_nWidth = nWidth;
	m_nLength = nLength;
	//지형 객체는 격자 메쉬들의 배열로 만들 것이다. nBlockWidth, nBlockLength는 격자 메쉬 하나의 가로, 세로 크
	//기이다. cxQuadsPerBlock, czQuadsPerBlock은 격자 메쉬의 가로 방향과 세로 방향 사각형의 개수이다.
	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;
	//xmf3Scale는 지형을 실제로 몇 배 확대할 것인가를 나타낸다. 
	m_xmf3Scale = xmf3Scale;
	//지형에 사용할 높이 맵을 생성한다. 
	m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);

	//지형에서 가로 방향, 세로 방향으로 격자 메쉬가 몇 개가 있는 가를 나타낸다. 
	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;

	//지형 전체를 표현하기 위한 격자 메쉬에 대한 포인터 배열을 생성한다. 
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
			//지형의 일부분을 나타내는 격자 메쉬의 시작 위치(좌표)이다. 

			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);
			//지형의 일부분을 나타내는 격자 메쉬를 생성하여 지형 메쉬에 저장한다.
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
