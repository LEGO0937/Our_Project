#include "ModelShader.h"
#include "../../Common//FrameWork/CreateManager.h"
#include "ItemObject.h"

BananaShader::BananaShader()
{
	shaderName = "BananaShader";
}
BananaShader::~BananaShader()
{

}

void BananaShader::Load(CreateManager* pCreateManager, const char* filename, const char* Loadname)
{
	isEnable = true;

	FILE *pInFile = NULL;
	::fopen_s(&pInFile, Loadname, "rb");
	if (!pInFile)
		return;
	ItemBanana* pItemObject = NULL;
	UINT nReads;
	int nLength = 0;

	const char* fileName = instancingModelName.c_str();

	nReads = (UINT)::fread(&nLength, sizeof(int), 1, pInFile);
	for (int i = 0; i < nLength; ++i)
	{
		CLoadedModelInfo *pModel = CGameObject::LoadGeometryAndAnimationFromFile(pCreateManager, fileName, NULL);
		pItemObject = new ItemBanana;
		pItemObject->SetChild(pModel->m_pModelRootObject->GetChild());
		pItemObject->AddRef();

		//이곳에서 findFrame을 통해 각 오브젝트에 질량 및 키네마틱 값 추가할 것.
		nReads = (UINT)::fread(&(pItemObject->m_xmf4x4ToParent), sizeof(XMFLOAT4X4), 1, pInFile);
		pItemObject->SetName(shaderName + to_string(m_iCurSerealNum));
		pItemObject->SetId(m_iCurSerealNum++);
		objectList.emplace_back(pItemObject);
		if (pModel)
		{
			delete pModel;
			pModel = NULL;
		}
	}

	::fclose(pInFile);
}

void BananaShader::Update(float fTimeElapsed)
{
	// 서버로부터 추가 신호를 받고 addObject(m_pCreateManager.get(), matrix)호출
	// 제거 명령도 여기서 받고 삭제할 것. 접근 방법은 아이디로 찾을 것.
	// find_if로 아이디와 일치하는 오브젝트의 반복자 찾고 erase로 삭제
	// objectList.erase(p);
	if (objectList.size())
	{
		for (vector<CGameObject*>::iterator it = objectList.begin(); it != objectList.end();)
		{
			if ((*it)->Update(fTimeElapsed, NULL))
			{
				(*it)->Release();
				it = objectList.erase(it);
				//바나나 경과시간이 초과되면 삭제하는 구간, 서버에 신호를 보내고 다음 프레임 업뎃에서 삭제함.
				//허나 모든 클라가 이 시간을 계산할텐데 그럼 신호는 클라수만큼 서버에 보내게 될텐데 한번으로
				//끝낼 방법 없을까?
			}
			else
				it++;
		}
	}
}

void BananaShader::addObject(CreateManager* pCreateManager, const XMFLOAT4X4& xmf3DepartPosition, const XMFLOAT4X4& xmf3ArrivePosition)
{
	ItemBanana* pItemObject = NULL;
	CLoadedModelInfo *pModel = CGameObject::LoadGeometryAndAnimationFromFile(pCreateManager, instancingModelName.c_str(), NULL);
	
	pItemObject = new ItemBanana;
	pItemObject->SetChild(pModel->m_pModelRootObject->GetChild());
	pItemObject->AddRef();

	pItemObject->SetMatrix(xmf3DepartPosition);
	pItemObject->SetName(shaderName + to_string(m_iCurSerealNum));
	pItemObject->SetId(m_iCurSerealNum++);
	pItemObject->UpdateTransform(NULL);

	objectList.emplace_back(pItemObject);
	if (pModel)
	{
		delete pModel;
		pModel = NULL;
	}
	ReleaseShaderVariables();
	CreateShaderVariables(pCreateManager);
}