#include "SkinedShader.h"
#include "../../Common//FrameWork/CreateManager.h"
#include "../../Common/Animation/Animation.h"

PlayerShader::PlayerShader()
{
}
PlayerShader::~PlayerShader()
{
}
void PlayerShader::BuildObjects(CreateManager* pCreateManager, void* pInformation)
{
	MODEL_INFO* info = (MODEL_INFO*)pInformation;
	if (!info->modelName)
		return;
	if (info->updatedContext)
		m_pUpdatedContext = info->updatedContext;

	instancingModelName = info->modelName;
	instancingModelName.insert(instancingModelName.find("."), "_ins");  //인스턴싱 전용 모델파일을 불러온다

	CDinoRunPlayer *pModel = new CDinoRunPlayer(pCreateManager, info->modelName);
	m_ppSkinedObjects = pModel;
	//m_ppSkinedObjects->AddRef();

	if (info->modelName)
		Load(pCreateManager, info->modelName, info->dataFileName);

	CreateShaderVariables(pCreateManager);
}

void PlayerShader::Load(CreateManager* pCreateManager, const char* filename, const char* Loadname)
{
	CPlayer *pPlayerObject = NULL;
	int nLength = 1;

	// 서버로부터 nLenght에 플레이어 수를 받음.
	// 여기서 한번에 아이디를 넘겨준다 아이디를 제외한 다른 유저들 아이디 걔 유저들에 대한 아이디랑 
	// 포지션 받아온다 갯수만큼 만들고 있는데
	// 
	
	for (int i = 0; i < nLength; ++i)
	{
		pPlayerObject = new CDinoRunPlayer(pCreateManager, instancingModelName);
		//동시에 아이디를 받아서 멤버에 셋팅.
		//pPlayerObject->AddRef();
		//pPlayerObject->SetPosition(XMFLOAT3(800.0f, 73.6, 920));
		//pPlayerObject->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
		//pPlayerObject->OnPrepareRender();
		pPlayerObject->SetId(m_iCurSerealNum++);
		//애니메이션 적용을 find_if를 통해서 아이디와 맞는 오브젝트 찾고 적용하도록 하자.
		m_vSkinedObjectList.emplace_back(pPlayerObject);
	}
}

void PlayerShader::Update(float fTimeElapsed)
{
	//이 구간에서 서버로부터 메시지 받고 특정 유저가 애니메이션 동작 실행 시 
	//리스트에서 아이디와 일치하는 오브젝트 찾아서 애니메이션 적용함.
}
