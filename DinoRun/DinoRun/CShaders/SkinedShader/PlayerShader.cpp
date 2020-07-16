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

	//서버로부터 nLenght에 플레이어 수를 받음.
	// 자기 아이디랑 위치값 받아오겠다는 패킷 명령어 그걸 보내면
	// 여기서 다시 리시브해서 그 패킷에 들어있는 거는 상대방 아이디 & 포지션
	// 그 다음 배열로 이름, 포지션
	
	

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

	// 위의 작업으로 아이디, 초기위치 배분하고 다시 서버로 샌드
	// 여기가 룸씬 이후 로딩중 과정
}

void PlayerShader::Update(float fTimeElapsed)
{
	//이 구간에서 서버로부터 메시지 받고 특정 유저가 애니메이션 동작 실행 시 
	//리스트에서 아이디와 일치하는 오브젝트 찾아서 애니메이션 적용함.
}