#include "SkinedShader.h"
#include "../../Common//FrameWork/CreateManager.h"


PlayerShader::PlayerShader()
{
}
PlayerShader::~PlayerShader()
{
}
void PlayerShader::Load(shared_ptr<CreateManager> pCreateManager, const char* filename, const char* Loadname)
{
	FILE *pInFile = NULL;
	::fopen_s(&pInFile, Loadname, "rb");
	if (!pInFile)
		return;
	CGameObject *pPlayerObject = NULL;
	UINT nReads;
	int nLength = 0;

	nReads = (UINT)::fread(&nLength, sizeof(int), 1, pInFile);
	for (int i = 0; i < nLength; ++i)
	{
		CLoadedModelInfo *pModel = CGameObject::LoadGeometryAndAnimationFromFile(pCreateManager, filename, NULL);
		pPlayerObject = pModel->m_pModelRootObject;
		pPlayerObject->AddRef();
		//이곳에서 findFrame을 통해 각 오브젝트에 질량 및 키네마틱 값 추가할 것 애니메이션도 포함.
		pPlayerObject->m_pSkinnedAnimationController = new CAnimationController(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), 2, pModel);
		pPlayerObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0); //left_turn_start
		pPlayerObject->m_pSkinnedAnimationController->SetTrackEnable(0, false);
		pPlayerObject->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1); //left_turn_start
		pPlayerObject->m_pSkinnedAnimationController->SetTrackEnable(1, false);

		nReads = (UINT)::fread(&(pPlayerObject->m_xmf4x4ToParent), sizeof(XMFLOAT4X4), 1, pInFile);
		objectList.emplace_back(pPlayerObject);
		if (pModel)
		{
			delete pModel;
			pModel = NULL;
		}
	}

	::fclose(pInFile);
}
