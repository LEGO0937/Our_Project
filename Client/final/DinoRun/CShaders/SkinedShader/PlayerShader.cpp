#include "SkinedShader.h"
#include "../Common/FrameWork/GameManager.h"
#include "../../Common/Animation/Animation.h"
#include "../Common/FrameWork/NetworkManager.h"
PlayerShader::PlayerShader()
{
	shaderName = _PLAYER_SHADER;
}
PlayerShader::~PlayerShader()
{
}
void PlayerShader::BuildObjects(void* pInformation)
{
	MODEL_INFO* info = (MODEL_INFO*)pInformation;
	if (!info->modelName)
		return;
	if (info->updatedContext)
		m_pUpdatedContext = info->updatedContext;

	instancingModelName = info->modelName;
	instancingModelName.insert(instancingModelName.find("."), "_ins");  //인스턴싱 전용 모델파일을 불러온다

	CDinoRunPlayer *pModel = new CDinoRunPlayer(info->modelName);
	m_ppObjects = pModel;
	//m_ppSkinedObjects->AddRef();

	if (info->modelName)
		Load(info->modelName, info->dataFileName);

	CreateShaderVariables();
}

void PlayerShader::Load(const char* filename, const char* Loadname)
{
	CDinoRunPlayer *pPlayerObject = NULL;
	int nLength = NetWorkManager::GetInstance()->GetNumPlayer();

	//서버로부터 nLenght에 플레이어 수를 받음.
	
	nLength = 3;
	for (int i = 0; i < nLength; ++i)
	{
		pPlayerObject = new CDinoRunPlayer(instancingModelName);
		//동시에 아이디를 받아서 멤버에 셋팅.
		//pPlayerObject->AddRef();
		//pPlayerObject->SetPosition(XMFLOAT3(800.0f, 73.6, 920));
		//pPlayerObject->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
		//pPlayerObject->OnPrepareRender();
		//pPlayerObject->SetId(m_iCurSerealNum++);
		//XMFLOAT3(700.0f, 76.0f, 1150.0f)
		pPlayerObject->SetUpdatedContext(m_pUpdatedContext);

		if (i == 0)
			pPlayerObject->SetPosition(XMFLOAT3(650.0f, 70.7f, 1150.0f));
		else if (i == 1)
			pPlayerObject->SetPosition(XMFLOAT3(680.0f, 70.7f, 1150.0f));
		else if (i == 2)
			pPlayerObject->SetPosition(XMFLOAT3(710.0f, 70.7f, 1150.0f));
		//애니메이션 적용을 find_if를 통해서 아이디와 맞는 오브젝트 찾고 적용하도록 하자.
		objectList.emplace_back(pPlayerObject);
	}
}

void PlayerShader::Update(float fTimeElapsed)
{
	//이 구간에서 서버로부터 메시지 받고 특정 유저가 애니메이션 동작 실행 시 
	//리스트에서 아이디와 일치하는 오브젝트 찾아서 애니메이션 적용함.
}
