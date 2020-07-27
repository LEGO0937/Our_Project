#include "ItemGameScene.h"
#include "FrameWork/CreateManager.h"
#include "FrameWork/NetWorkManager.h"
#include "FrameWork/SoundManager.h"

#include "PlayerObject.h"
#include "SkyBoxObject.h"
#include "TerrainObject.h"


#include "../CShaders/BillBoardShader/BillBoardShader.h"
#include "../CShaders/ModelShader/ModelShader.h"
#include "../CShaders/SkinedShader/SkinedShader.h"
#include "../CShaders/BlurShader/BlurShader.h"
#include "../CShaders/MotionBlurShader/MotionBlurShader.h"
#include "../CShaders/MinimapShader/MinimapShader.h"

#include "ParticleSystem/ParticleSystem.h"

#include "../CShaders/UiShader/UiShader.h"


#include "Camera/Camera.h"
#include "EventHandler/EventHandler.h"

#include <time.h>

#define ITEM_TILE 1
#define ITEM_UI 2   //아이템 틀안의 이미지의 쉐이더 리스트상에서의 인덱스
#define PLAYER_SHADER instancingAnimatedModelShaders[0]

char ItemShaderName[5] = { 0,_BANANA_SHADER,_MUD_SHADER,_STONE_SHADER,_METEORITE_SHADER };

ItemGameScene::ItemGameScene() :BaseScene()
{
	sceneType = SceneType::ItemGame_Scene;
}
ItemGameScene::~ItemGameScene()
{
	SoundManager::GetInstance()->Stop("InGame_BGM");
}
void ItemGameScene::ReleaseUploadBuffers()
{
	BaseScene::ReleaseUploadBuffers();
	if (m_pTerrain)
		m_pTerrain->ReleaseUploadBuffers();
	if (m_pSkyBox)
		m_pSkyBox->ReleaseUploadBuffers();
	if (m_pCheckPointShader)
		m_pCheckPointShader->ReleaseUploadBuffers();

	for (CObInstancingShader* shader : instancingBillBoardShaders)
		if (shader) shader->ReleaseUploadBuffers();
	for (CObInstancingShader* shader : instancingModelShaders)
		if (shader) shader->ReleaseUploadBuffers();
	for (CSkinedObInstancingShader* shader : instancingAnimatedModelShaders)
		if (shader) shader->ReleaseUploadBuffers();
	for (CUiShader* shader : instancingNumberUiShaders)
		if (shader) { shader->ReleaseUploadBuffers(); }
	for (CUiShader* shader : instancingImageUiShaders)
		if (shader) { shader->ReleaseUploadBuffers(); }
	if (m_pMinimapShader)
		m_pMinimapShader->ReleaseUploadBuffers();
	if (m_pIconShader)
		m_pIconShader->ReleaseUploadBuffers();
	if (m_pEffectShader)
		m_pEffectShader->ReleaseUploadBuffers();
	if (m_pCountDownShader)
		m_pCountDownShader->ReleaseUploadBuffers();
}
void ItemGameScene::ReleaseObjects()
{
	BaseScene::ReleaseObjects();
	if (m_pTerrain)
		m_pTerrain->Release();

	for (ParticleSystem* system : particleSystems)
	{
		system->Release();
	}
	if (m_pSkyBox)
	{
		m_pSkyBox->Release();
		m_pSkyBox = NULL;
	}

	if (m_pCheckPointShader)
	{
		m_pCheckPointShader->ReleaseShaderVariables();
		m_pCheckPointShader->ReleaseObjects();
		m_pCheckPointShader->Release();
	}
	for (CObjectsShader* shader : UpdatedShaders)
		if (shader) { shader->Release(); }

	for (CObInstancingShader* shader : instancingBillBoardShaders)
		if (shader) { shader->ReleaseShaderVariables(); shader->ReleaseObjects();  shader->Release(); }
	for (CObInstancingShader* shader : instancingModelShaders)
		if (shader) { shader->ReleaseShaderVariables(); shader->ReleaseObjects();  shader->Release(); }
	for (CSkinedObInstancingShader* shader : instancingAnimatedModelShaders)
		if (shader) { 
			shader->ReleaseShaderVariables(); shader->ReleaseObjects();  shader->Release(); 
		}
	for (CUiShader* shader : instancingNumberUiShaders)
		if (shader) { shader->ReleaseShaderVariables(); shader->ReleaseObjects();  shader->Release(); }
	for (CUiShader* shader : instancingImageUiShaders)
		if (shader) { shader->ReleaseShaderVariables(); shader->ReleaseObjects();  shader->Release(); }

	if (m_pMinimapCamera)
	{
		m_pMinimapCamera->ReleaseShaderVariables();
		delete m_pMinimapCamera;
	}

	if (blurShader)
		blurShader->Release();
	if (motionBlurShader)
		motionBlurShader->Release();

	if (m_pMinimapShader)
	{
		m_pMinimapShader->ReleaseShaderVariables();
		m_pMinimapShader->ReleaseObjects();
		m_pMinimapShader->Release();
	}
	if (m_pIconShader)
	{
		m_pIconShader->ReleaseShaderVariables();
		m_pIconShader->ReleaseObjects();
		m_pIconShader->Release();
	}
	if (m_pEffectShader)
	{
		m_pEffectShader->ReleaseShaderVariables();
		m_pEffectShader->ReleaseObjects();
		m_pEffectShader->Release();
	}
	if (m_pCountDownShader)
	{
		m_pCountDownShader->ReleaseShaderVariables();
		m_pCountDownShader->ReleaseObjects();
		m_pCountDownShader->Release();
	}

	UpdatedShaders.clear();
	instancingNumberUiShaders.clear();
	instancingImageUiShaders.clear();
	instancingBillBoardShaders.clear();
	instancingModelShaders.clear();
	instancingAnimatedModelShaders.clear();
}
void ItemGameScene::BuildObjects(shared_ptr<CreateManager> pCreateManager)
{
	m_pCreateManager = pCreateManager;
	m_fCountDownTime = 4.0f;  //임시로 클라에서 시간재기로함 서버 연동후 서버가 계산후 보내줄것.

	m_pd3dCommandList = pCreateManager->GetCommandList().Get();

	//NetWorkManager::GetInstance()->ConnectToServer();

	XMFLOAT3 xmf3Scale(TerrainScaleX, TerrainScaleY, TerrainScaleZ);

	m_pSkyBox = new SkyBoxObject(pCreateManager.get());
	m_pCreateManager->RenderLoading();
	m_pTerrain = new CHeightMapTerrain(pCreateManager.get(), _T("Resources\\Images\\First_Map.raw"), 257, 257, 7,
		7, xmf3Scale);
	m_pCreateManager->RenderLoading();
	m_pCreateManager->RenderLoading();
	m_pCreateManager->RenderLoading();
	CObInstancingShader* shader;
	BillBoardShader* bShader;
	CUiShader* uiShader;
	CSkinedObInstancingShader* animatedShader;

	UI_INFO view_info;    //게임중 or 대기중 뷰
	MODEL_INFO model_info;
	model_info.updatedContext = m_pTerrain;

	view_info.textureName = "Resources/Images/T_BlurEffect.dds";
	view_info.meshSize = XMFLOAT2(1.0f, 1.0f);
	view_info.positions.emplace_back(XMFLOAT3(0.0f, 0.0f, 0.1f));
	view_info.maxUv = XMFLOAT2(0.125f, 1.0f);
	view_info.minUv = XMFLOAT2(0.0f, 0.0f);
	view_info.f_uvY.emplace_back(0);
	m_pEffectShader = new ImageShader;
	m_pEffectShader->BuildObjects(pCreateManager.get(), &view_info);
	view_info.positions.clear();
	view_info.f_uvY.clear();

	view_info.textureName = "Resources/Images/T_Gauge_Frame.dds";
	view_info.meshSize = XMFLOAT2(0.07f, 0.05f);
	view_info.positions.emplace_back(XMFLOAT3(+0.93f, -0.95f, 0.0f));
	view_info.maxUv = XMFLOAT2(1.0f, 1.0f);
	view_info.minUv = XMFLOAT2(0.0f, 0.0f);
	view_info.f_uvY.emplace_back(0.0f);
	uiShader = new ImageShader;
	uiShader->BuildObjects(pCreateManager.get(), &view_info);
	instancingImageUiShaders.emplace_back(uiShader);
	view_info.positions.clear();
	view_info.f_uvY.clear();

	m_pCountDownShader = new CountDownShader;
	m_pCountDownShader->BuildObjects(pCreateManager.get(), NULL);
#ifdef isDebug
	bShader = new BillBoardShader;
	model_info.modelName = "Resources/Images/B_Tree.dds";
	model_info.dataFileName = "Resources/ObjectData/BillBoardData";
	model_info.size = 50;
	bShader->BuildObjects(pCreateManager.get(),&model_info);
	instancingBillBoardShaders.emplace_back(bShader);

	shader = new BillBoardShader;
	model_info.modelName = "Resources/Images/B_Stone.dds";
	model_info.dataFileName = "Resources/ObjectData/BillBoardStoneData";
	model_info.size = 50;
	shader->BuildObjects(pCreateManager.get(), &model_info);
	instancingBillBoardShaders.emplace_back(shader);

	shader = new BillBoardShader;
	model_info.modelName = "Resources/Images/B_Bush.dds";
	model_info.dataFileName = "Resources/ObjectData/BillBoardBushData";
	model_info.size = 50;
	shader->BuildObjects(pCreateManager.get(), &model_info);
	instancingBillBoardShaders.emplace_back(shader);
#endif
	shader = new FenceShader;
	model_info.modelName = "Resources/Models/M_Block.bin";
	model_info.dataFileName = "Resources/ObjectData/RectData(Fence)";
	model_info.useBillBoard = false;
	shader->BuildObjects(pCreateManager.get(), &model_info);
	instancingModelShaders.emplace_back(shader);
	shader->AddRef();
	UpdatedShaders.emplace_back(shader);

	shader = new BananaShader;
	model_info.modelName = "Resources/Models/M_Banana.bin";
	model_info.dataFileName = NULL;
	model_info.useBillBoard = true;
	shader->BuildObjects(pCreateManager.get(), &model_info);
	instancingModelShaders.emplace_back(shader);
	shader->AddRef();
	UpdatedShaders.emplace_back(shader);

	shader = new MudShader;
	model_info.modelName = "Resources/Models/M_Mud.bin";
	model_info.dataFileName = NULL;
	model_info.useBillBoard = false;
	shader->BuildObjects(pCreateManager.get(), &model_info);
	instancingModelShaders.emplace_back(shader);
	shader->AddRef();
	UpdatedShaders.emplace_back(shader);
	m_pCreateManager->RenderLoading();
	shader = new StoneShader;
	model_info.modelName = "Resources/Models/M_Stone.bin";
	model_info.dataFileName = NULL;
	model_info.useBillBoard = true;
	shader->BuildObjects(pCreateManager.get(), &model_info);
	instancingModelShaders.emplace_back(shader);
	shader->AddRef();
	UpdatedShaders.emplace_back(shader);

	shader = new MeteoriteShader;
	model_info.modelName = "Resources/Models/M_Meteorite.bin";
	model_info.dataFileName = NULL;
	model_info.useBillBoard = false;
	shader->BuildObjects(pCreateManager.get(), &model_info);
	instancingModelShaders.emplace_back(shader);
	shader->AddRef();
	UpdatedShaders.emplace_back(shader);

	shader = new MoundShader;
	model_info.modelName = "Resources/Models/M_RockRIP.bin";
	model_info.dataFileName = NULL;
	shader->BuildObjects(pCreateManager.get(), &model_info);
	instancingModelShaders.emplace_back(shader);
	shader->AddRef();
	UpdatedShaders.emplace_back(shader);

	m_pCreateManager->RenderLoading();

#ifdef isDebug
	shader = new TreeShader;
	model_info.modelName = "Resources/Models/M_Tree.bin";
	model_info.dataFileName = "Resources/ObjectData/TreeData";
	model_info.useBillBoard = true;
	shader->BuildObjects(pCreateManager.get(), &model_info);
	instancingModelShaders.emplace_back(shader);

	shader = new TreeShader;
	model_info.modelName = "Resources/Models/M_Stone.bin";
	model_info.dataFileName = "Resources/ObjectData/StoneData";
	shader->BuildObjects(pCreateManager.get(), &model_info);
	instancingModelShaders.emplace_back(shader);
	m_pCreateManager->RenderLoading();
	shader = new TreeShader;
	model_info.modelName = "Resources/Models/M_Bush.bin";
	model_info.dataFileName = "Resources/ObjectData/WeedData";
	shader->BuildObjects(pCreateManager.get(), &model_info);
	instancingModelShaders.emplace_back(shader);

	shader = new TreeShader;
	model_info.modelName = "Resources/Models/M_Tree2.bin";
	model_info.dataFileName = "Resources/ObjectData/Tree2Data";
	//model_info.useBillBoard = false;
	shader->BuildObjects(pCreateManager.get(), &model_info);
	instancingModelShaders.emplace_back(shader);

	shader = new TreeShader;
	model_info.modelName = "Resources/Models/M_Tree3.bin";
	model_info.dataFileName = "Resources/ObjectData/Tree3Data";
	shader->BuildObjects(pCreateManager.get(), &model_info);
	instancingModelShaders.emplace_back(shader);

	shader = new TreeShader;
	model_info.modelName = "Resources/Models/M_Tree4.bin";
	model_info.dataFileName = "Resources/ObjectData/Tree4Data";
	shader->BuildObjects(pCreateManager.get(), &model_info);
	instancingModelShaders.emplace_back(shader);

	//--
	shader = new TreeShader;
	model_info.modelName = "Resources/Models/M_Stone1.bin";
	model_info.dataFileName = "Resources/ObjectData/Stone1Data";
	shader->BuildObjects(pCreateManager.get(), &model_info);
	instancingModelShaders.emplace_back(shader);
	//
	shader = new TreeShader;
	model_info.modelName = "Resources/Models/M_Grass.bin";
	model_info.dataFileName = "Resources/ObjectData/Grass1Data";
	shader->BuildObjects(pCreateManager.get(), &model_info);
	instancingModelShaders.emplace_back(shader);
	//
	shader = new TreeShader;
	model_info.modelName = "Resources/Models/M_Stone3.bin";
	model_info.dataFileName = "Resources/ObjectData/Stone3Data";
	shader->BuildObjects(pCreateManager.get(), &model_info);
	instancingModelShaders.emplace_back(shader);
#endif

	shader = new ItemShader;
	model_info.modelName = "Resources/Models/M_Itembox.bin";
	model_info.dataFileName = "Resources/ObjectData/MeatData";
	model_info.useBillBoard = false;
	shader->BuildObjects(pCreateManager.get(), &model_info);
	instancingModelShaders.emplace_back(shader);
	shader->AddRef();
	UpdatedShaders.emplace_back(shader);

	m_pCheckPointShader = new BlockShader;
	model_info.modelName = "Resources/Models/M_Block.bin";
	model_info.dataFileName = "Resources/ObjectData/RectData(LineBox)";
	m_pCheckPointShader->BuildObjects(pCreateManager.get(),&model_info);
	//m_pCheckPointShader->AddRef();

	shader = new ModelShader;
	model_info.modelName = "Resources/Models/M_Cavern.bin";
	model_info.dataFileName = "Resources/ObjectData/CaveData";
	shader->BuildObjects(pCreateManager.get(), &model_info);
	instancingModelShaders.emplace_back(shader);

	uiShader = new TimeCountShader;
	uiShader->BuildObjects(pCreateManager.get(), NULL);
	instancingNumberUiShaders.emplace_back(uiShader);

	uiShader = new TrackCountShader;
	uiShader->BuildObjects(pCreateManager.get(), NULL);
	instancingNumberUiShaders.emplace_back(uiShader);

	uiShader = new RankCountShader;
	uiShader->BuildObjects(pCreateManager.get(), NULL);
	instancingNumberUiShaders.emplace_back(uiShader);

	uiShader = new VelocityCountShader;
	uiShader->BuildObjects(pCreateManager.get(), m_pTerrain);
	instancingNumberUiShaders.emplace_back(uiShader);


	UI_INFO ItemUi_info;

	//---- 아이템 틀
	ItemUi_info.textureName = "Resources/Images/T_Itemcast.dds";
	ItemUi_info.meshSize = XMFLOAT2(0.15f, 0.15f);
	ItemUi_info.positions.emplace_back(XMFLOAT3(0.0f, -0.8f, 0.0f));
	ItemUi_info.f_uvY.emplace_back(0.0f);
	ItemUi_info.maxUv = XMFLOAT2(1.0f, 1.0f);
	ItemUi_info.minUv = XMFLOAT2(0.0f, 0.0f);

	uiShader = new ImageShader;
	uiShader->BuildObjects(pCreateManager.get(), &ItemUi_info);
	instancingImageUiShaders.emplace_back(uiShader);

	//보유 아이템 사진
	ItemUi_info.textureName = "Resources/Images/T_Itemsprite.dds";
	ItemUi_info.maxUv = XMFLOAT2(0.125f, 1.0f);

	uiShader = new ImageShader;
	uiShader->BuildObjects(pCreateManager.get(), &ItemUi_info);
	uiShader->getUvXs()[0] = 0.0f;  //스프라이트의 간격은 0.125f
	instancingImageUiShaders.emplace_back(uiShader);

	animatedShader = new PlayerShader;
	model_info.modelName = "Resources/Models/M_DinoTest.bin";
	model_info.dataFileName = NULL;
	animatedShader->BuildObjects(pCreateManager.get(), &model_info);
	instancingAnimatedModelShaders.emplace_back(animatedShader);
	animatedShader->AddRef();
	UpdatedShaders.emplace_back(animatedShader);
	 
	m_pMinimapShader = new MinimapShader();
	m_pMinimapShader->BuildObjects(pCreateManager.get(), "Resources/Images/MiniMap.dds", NULL);

	string name = "Resources/Images/T_Faceicon.dds";
	m_pIconShader = new IconShader();
	m_pIconShader->BuildObjects(pCreateManager.get(), &name);

	blurShader = new BlurShader(pCreateManager.get());
	motionBlurShader = new MotionBlurShader(pCreateManager.get());

	XMFLOAT3 startPosition = m_pCheckPointShader->getList()[0]->GetPosition();
	particleSystems.emplace_back(new ParticleSystem(pCreateManager.get(),SPAWN, NULL, XMFLOAT3(startPosition.x, m_pTerrain->GetHeight(startPosition.x, startPosition.z), startPosition.z)));
	particleSystems.emplace_back(new ParticleSystem(pCreateManager.get(), SPAWN, NULL, XMFLOAT3(startPosition.x - 50, m_pTerrain->GetHeight(startPosition.x, startPosition.z), startPosition.z)));
	particleSystems.emplace_back(new ParticleSystem(pCreateManager.get(), SPAWN, NULL, XMFLOAT3(startPosition.x + 50, m_pTerrain->GetHeight(startPosition.x, startPosition.z), startPosition.z)));

	BuildLights();

	BuildSubCameras(pCreateManager);

	gameTexts.emplace_back(GameText(XMFLOAT2(0.05f, 0.19f))); //플레이어 명단
	gameTexts.emplace_back(GameText(XMFLOAT2(0.05f, 0.25f)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.05f, 0.31f)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.05f, 0.37f)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.05f, 0.43f)));

	CreateShaderVariables(pCreateManager.get());
	m_pCreateManager->RenderLoading();

	SoundManager::GetInstance()->Play("InGame_BGM", 0.2f);
	m_pCreateManager->RenderLoading();
}

void ItemGameScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
	lParam, float deltaTime)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		//마우스 캡쳐를 하고 현재 마우스 위치를 가져온다. 
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
		::ReleaseCapture();
		break;
	case WM_RBUTTONUP:
		//마우스 캡쳐를 해제한다. 

		::ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
}
void ItemGameScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
	lParam, float deltaTime)
{
	MessageStruct message;
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_LEFT:
			//if (m_pPlayer)  m_pPlayer->KeyUpLeft();
			break;
		case VK_RIGHT:
			//if (m_pPlayer)	m_pPlayer->KeyUpRight();
			break;
		case VK_UP:
			//if (m_pPlayer)	m_pPlayer->KeyUpUp();
			break;
		case VK_DOWN:
			//if (m_pPlayer)	m_pPlayer->KeyUpDown();
			break;
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		case VK_SHIFT:
			if (m_pPlayer)
				m_pPlayer->setShift(false);
			break;
		default:
			break;
		}
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_CONTROL:
			//바나나= -8   머드= -8
			switch (m_eCurrentItem)
			{
			case IconBanana:
			case IconStone:
			case IconMud:
				XMFLOAT4X4 matrix = m_pPlayer->m_xmf4x4ToParent;
				XMFLOAT3 pos = m_pPlayer->GetLook();
				pos = Vector3::ScalarProduct(pos, 40, false);
				
				matrix._41 -= pos.x;
				matrix._43 -= pos.z;
				matrix._42 = m_pTerrain->GetHeight(matrix._41, 256 * TerrainScaleZ - matrix._43)+1.0f;
				XMFLOAT3 up = m_pTerrain->GetNormal(matrix._41, matrix._43);
				

				matrix._21 = up.x;
				matrix._22 = up.y;
				matrix._23 = up.z;
				XMFLOAT3 look = Vector3::CrossProduct(m_pPlayer->GetRight(), up, true);
				
				matrix._31 = look.x;
				matrix._32 = look.y;
				matrix._33 = look.z;
				
				//이부분에도 바로 추가하지않고 신호를 보냄. 업데이트에서 신호를 받아서 추가하도록 한다.
				message.shaderName = ItemShaderName[m_eCurrentItem];
				message.departMat = matrix;
				message.msgName = _ADD_OBJECT;
				EventHandler::GetInstance()->RegisterEvent(message);
				break;
			case IconMeat:
				m_pPlayer->SetMaxVelocityXZ(50);
				m_fBoostTimer = 10;
				isBoost = true;
				break;
			case IconMeteorite:
				int checkCount = (m_pPlayer->GetCheckPoint() + 9) % 181;
				message.shaderName = _METEORITE_SHADER;
				CGameObject* checkPoint = m_pCheckPointShader->getList()[checkCount];
				XMFLOAT4X4 mat = checkPoint->m_xmf4x4World;
				mat._42 += 500;
				message.departMat = mat;
				message.msgName = _ADD_OBJECT;
				EventHandler::GetInstance()->RegisterEvent(message);
				break;
			}
			m_eCurrentItem = IconDefault;
			instancingImageUiShaders[ITEM_UI]->getUvXs()[0] = 0.125f * m_eCurrentItem;
			break;
		case VK_F2:
		case VK_F3:
			if (m_pPlayer) m_pCamera = m_pPlayer->ChangeCamera((wParam - VK_F1 + 1),
				deltaTime);
			break;
		case VK_F4:
			message.shaderName = _METEORITE_SHADER;
			XMFLOAT4X4 mat = m_pPlayer->m_xmf4x4World;
			mat._42 += 500;
			message.departMat = mat;
			message.msgName = _ADD_OBJECT;
			EventHandler::GetInstance()->RegisterEvent(message);
			break;
		case VK_LEFT:
			//if (m_pPlayer)	m_pPlayer->KeyDownLeft();
			break;
		case VK_RIGHT:
			//if (m_pPlayer)	m_pPlayer->KeyDownRight();
			break;
			break;
		case VK_UP:
			//if (m_pPlayer)	m_pPlayer->KeyDownUp();
			break;
		case VK_DOWN:
			//if (m_pPlayer)	m_pPlayer->KeyDownDown();
			break;
		case VK_SHIFT:
			if (m_pPlayer)
				m_pPlayer->setShift(true);
			break;
		default:
			break;
		}
	default:
		break;
	}
}
void ItemGameScene::ProcessInput(HWND hwnd, float deltaTime)
{
	static UCHAR pKeyBuffer[256];
	dwDirection = 0;

	if (!isStart)
		return;

	/*키보드의 상태 정보를 반환한다. 화살표 키(‘→’, ‘←’, ‘↑’, ‘↓’)를 누르면 플레이어를 오른쪽/왼쪽(로컬 x-축), 앞/
	뒤(로컬 z-축)로 이동한다. ‘Page Up’과 ‘Page Down’ 키를 누르면 플레이어를 위/아래(로컬 y-축)로 이동한다.*/
	if (::GetKeyboardState(pKeyBuffer))
	{
		// 찜
		if (pKeyBuffer[VK_UP] & 0xF0)
			dwDirection |= DIR_FORWARD;
		if (pKeyBuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_BACKWARD;
		if (pKeyBuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeyBuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_RIGHT;
		if (pKeyBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
		if (pKeyBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;
	}
	float cxDelta = 0.0f, cyDelta = 0.0f;
	
	if (::GetCapture() == hwnd)
	{
	}
	//마우스 또는 키 입력이 있으면 플레이어를 이동하거나(dwDirection) 회전한다(cxDelta 또는 cyDelta).
	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		
		if (dwDirection)
		{
			//m_pPlayer->Move(dwDirection, 20.0f, deltaTime, true);
			//((CPlayer*)PLAYER_SHADER->getSkiendList()[0])->Move(dwDirection, 20.0f, deltaTime, true);
		}

	}
	else
		m_pPlayer->m_fForce = 0;

	m_pPlayer->Move(dwDirection, 20.0f, deltaTime, true);
//	((CPlayer*)PLAYER_SHADER->getSkiendList()[0])->Move(dwDirection, 20.0f, deltaTime, true);
	//플레이어를 실제로 이동하고 카메라를 갱신한다. 중력과 마찰력의 영향을 속도 벡터에 적용한다. 
	//m_pPlayer->FixedUpdate(deltaTime);
}

void ItemGameScene::Render()
{
	BaseScene::Render();

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_CUBE_MAP]);
	if (m_pSkyBox) m_pSkyBox->Render(m_pd3dCommandList, m_pCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_SKIN_MESH]);
	m_pPlayer->Render(m_pd3dCommandList, m_pCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_MODEL_INSTANCING]);
	for (CObInstancingShader* shader : instancingModelShaders)
	{
		if (shader)
			shader->Render(m_pd3dCommandList, m_pCamera);
	}
	m_pCheckPointShader->Render(m_pd3dCommandList, m_pCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_BILLBOARD]);
	for (CObInstancingShader* shader : instancingBillBoardShaders)
		if (shader) shader->Render(m_pd3dCommandList, m_pCamera);
	for (CObInstancingShader* shader : instancingModelShaders)
	{
		if (shader)
			shader->BillBoardRender(m_pd3dCommandList, m_pCamera);
	}

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_TERRAIN]);
	if (m_pTerrain) m_pTerrain->Render(m_pd3dCommandList, m_pCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_SKIN_MESH_INSTANCING]);
	for (CSkinedObInstancingShader* shader : instancingAnimatedModelShaders)
		if (shader) {
			shader->Render(m_pd3dCommandList, m_pCamera);
		}


	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_PARTICLE]);

	m_pPlayer->m_pParticleSystem->Render(m_pd3dCommandList, m_pCamera);
	for (ParticleSystem* system : particleSystems)
	{
		system->Render(m_pd3dCommandList, m_pCamera);
	}
#ifdef _WITH_BOUND_BOX
	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_WIRE]);
	m_pPlayer->BbxRender(m_pd3dCommandList, m_pCamera);
	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_WIRE_INSTANCING]);
	for (CObInstancingShader* shader : instancingModelShaders)
		if (shader) shader->BbxRender(m_pd3dCommandList, m_pCamera);

#endif
}

void ItemGameScene::RenderShadow()
{
	BaseScene::RenderShadow();

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_SHADOW_SKIN_MESH]);
	m_pPlayer->Render(m_pd3dCommandList, m_pShadowCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_SHADOW_MODEL_INSTANCING]);
	for (CObInstancingShader* shader : instancingModelShaders)
		if (shader) shader->Render(m_pd3dCommandList, m_pShadowCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_SHADOW_BILLBOARD]);
	for (CObInstancingShader* shader : instancingBillBoardShaders)
		if (shader) shader->Render(m_pd3dCommandList, m_pShadowCamera);
	for (CObInstancingShader* shader : instancingModelShaders)
	{
		if (shader)
			shader->BillBoardRender(m_pd3dCommandList, m_pShadowCamera);
	}
}
void ItemGameScene::RenderVelocity()
{
	BaseScene::Render();

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_VELOCITY_SKIN_MESH]);
	m_pPlayer->Render(m_pd3dCommandList, m_pCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_VELOCITY_MODEL_INSTANCING]);
	for (CObInstancingShader* shader : instancingModelShaders)
		if (shader) shader->Render(m_pd3dCommandList, m_pCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_VELOCITY_BILLBOARD]);
	for (CObInstancingShader* shader : instancingBillBoardShaders)
		if (shader) shader->Render(m_pd3dCommandList, m_pCamera);
	for (CObInstancingShader* shader : instancingModelShaders)
	{
		if (shader)
			shader->BillBoardRender(m_pd3dCommandList, m_pCamera);
	}
	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_VELOCITY_TERRAIN]);
	if (m_pTerrain) m_pTerrain->Render(m_pd3dCommandList, m_pCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_VELOCITY_CUBEMAP]);
	//if (m_pSkyBox) m_pSkyBox->Render(m_pd3dCommandList, m_pCamera);
}

void ItemGameScene::RenderPostProcess(ComPtr<ID3D12Resource> curBuffer, ComPtr<ID3D12Resource> velocityMap)
{
	static float deltaUvX = 0.0f;
	XMFLOAT3 vel = m_pPlayer->GetVelocity();
	float length = sqrtf(vel.x * vel.x + vel.z * vel.z);
	if (length > 30)
	{
		int idx = length - 30;
		//blurShader->Dispatch(m_pd3dCommandList, m_ppd3dPipelineStates[PSO_HORZ_BLUR], m_ppd3dPipelineStates[PSO_VERT_BLUR], curBuffer.Get(), idx/10);
		m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_MOTION_BLUR]);
		motionBlurShader->Dispatch(m_pd3dCommandList, curBuffer.Get(), velocityMap.Get(), 10);
		m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_EFFECT]);
		m_pEffectShader->Render(m_pd3dCommandList, m_pCamera);
		m_pEffectShader->getUvXs()[0] = deltaUvX;
		deltaUvX += 0.125f;
		if (deltaUvX >= 1.0)
			deltaUvX = 0.0f;
	}
	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_UI_NUMBER]);
	for (CUiShader* shader : instancingNumberUiShaders)
		if (shader) shader->Render(m_pd3dCommandList, m_pCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_UI]);
	for (CUiShader* shader : instancingImageUiShaders)
		if (shader) shader->Render(m_pd3dCommandList, m_pCamera);
	if (m_pCountDownShader)
		m_pCountDownShader->Render(m_pd3dCommandList, m_pCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_PONT]);
	if (fontShader)
		fontShader->Render(m_pd3dCommandList, m_pCamera, gameTexts);


	m_pMinimapCamera->SetViewportsAndScissorRects(m_pd3dCommandList);
	m_pMinimapCamera->UpdateShaderVariables(m_pd3dCommandList);
	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_MINIMAP]);
	if (m_pMinimapShader)
		m_pMinimapShader->Render(m_pd3dCommandList, m_pMinimapCamera);
	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_UI]);
	if (m_pIconShader)
		m_pIconShader->Render(m_pd3dCommandList, m_pMinimapCamera);
	
}
void ItemGameScene::AnimateObjects(float fTimeElapsed)
{
	//말 그대로 애니메이션 update
	if (m_pPlayer)
		m_pPlayer->Animate(fTimeElapsed);

	for (CSkinedObInstancingShader* shader : instancingAnimatedModelShaders)
		if (shader) { shader->AnimateObjects(fTimeElapsed); }
}


void ItemGameScene::FixedUpdate(CreateManager* pCreateManager, float fTimeElapsed)
{
	//자기 정보 보내기
	/*
	SC_PACKET_PLAYER_INFO packet; 
	packet.checkPoints[0] = m_pPlayer->GetCheckPoint();
	packet.playerNames[0] = m_sPlayerId;
	packet.xmf4x4Parents[0] = m_pPlayer->m_xmf4x4ToParent;
	packet.keyState[0] = dwDirection;   //키입력정보, 이 정보로 타플레이어 애니메이션 적용
	packet.type = SC_MOVE_PLAYER;

	//자기정보 send
	//모든플레이어 정보 recv 변수 packet으로 받음
	//recv로 받은 패킷을 통해 위치 초기화

	for (int i = 0; i < NetWorkManager::GetInstance()->GetNumPlayer(); ++i)
	{
		if (packet.playerNames[i] == m_sPlayerId)
			continue;

		vector<CGameObject*> obList = PLAYER_SHADER->getList();
		auto obj = find_if(obList.begin(), obList.end(), [&](CGameObject* a) {
			return a->GetName() == packet.playerNames[i]; });
		if (obj != obList.end())
		{
			(*obj)->m_xmf4x4ToParent = packet.xmf4x4Parents[i];
			((CPlayer*)(*obj))->SetCheckPoint(packet.checkPoints[i]);
			((CPlayer*)(*obj))->Move(packet.keyState[i], 20.0f, fTimeElapsed, true);
		}
		else
		{
			auto findId = find_if(obList.begin(), obList.end(), [&](CGameObject* a) {
				return a->GetName() == "None"; });
			if (findId != obList.end())
			{
				(*findId)->SetName(packet.playerNames[i]);
				(*findId)->m_xmf4x4ToParent = packet.xmf4x4Parents[i];
				((CPlayer*)(*findId))->SetCheckPoint(packet.checkPoints[i]);
				((CPlayer*)(*obj))->Move(packet.keyState[i], 20.0f, fTimeElapsed, true);
					// 위치값 + 행렬
			}

		}
	}
	*/
	//서버가 recv, 자신까지 포함한 본인정보까지 받을것 이때 쓰는 패킷은 player_info 패킷
	//


	//명령 send(플레이어 애니메이션 혹은 행렬 최신화)
	//명령 recv
	//ProcessPacket(패킷)
	
	//물리
	{
		m_pPlayer->FixedUpdate(fTimeElapsed);

		for (CObjectsShader* shader : UpdatedShaders)
		{
			shader->FixedUpdate(fTimeElapsed);  //물리 적용할 것
		}
	}

	if (!isStart)
	{
		if (m_fCountDownTime > 0.0f)
		{
			m_fCountDownTime -= fTimeElapsed;
		}
		else
		{
			m_fCountDownTime = 0.0f;
			isStart = true;
		}
		/*  서버연동후에는 이거 하나로 돌아감.
		if (m_fCountDownTime < 0.0f)
		{
			m_fCountDownTime = 0.0f;
			isStart = true;
		}
		*/
	}
	m_pCountDownShader->Update(fTimeElapsed,&m_fCountDownTime);

}


SceneType ItemGameScene::Update(CreateManager* pCreateManager, float fTimeElapsed)
{

	if (sceneType != SceneType::ItemGame_Scene)
	{
		//서버와 연결 끊기, 엔드씬에서 룸씬으로 넘어가고 다시 시작하면 연결해야함
		return sceneType;
	}

	//----
	//명령 send(오브젝트 생성,삭제 및 파티클 추가, 오브젝트 비활성화)
	//명령 recv
	//ProcessPacket(패킷)
	//----
	EventHandler::GetInstance()->Update();

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_PARTICLE_CALC]);

	m_pPlayer->m_pParticleSystem->AnimateObjects(fTimeElapsed);

	for (list<ParticleSystem*>::iterator i = particleSystems.begin(); i != particleSystems.end();)
	{
		if ((*i)->AnimateObjects(fTimeElapsed))
		{
			(*i)->Release();
			i = particleSystems.erase(i);
			if (i == particleSystems.end())
				break;
		}
		else
			i++;
	}

	if (isStart)
	{
		if (m_pPlayer->GetCheckPoint() == CHECKPOINT_GOAL)
		{
			SoundManager::GetInstance()->AllStop();
			sceneType = End_Scene;  //멀티 플레이시 이 구간에서 서버로부터 골인한 플레이어를 확인후 씬 전환
		}
		else
		{
			int rank = 1;
			vector<CGameObject*> list = PLAYER_SHADER->getList();
			((CPlayer*)list[0])->SetCheckPoint(1);
			((CPlayer*)list[0])->SetName("player3");
			((CPlayer*)list[1])->SetCheckPoint(4);
			((CPlayer*)list[1])->SetName("player1");
			((CPlayer*)list[2])->SetCheckPoint(3);
			((CPlayer*)list[2])->SetName("player2");

			sort(list.begin(), list.end(), [](CGameObject* a, CGameObject* b) {
				return ((CPlayer*)a)->GetCheckPoint() > ((CPlayer*)b)->GetCheckPoint(); });

			int idx = 0;
			for (CGameObject* obj : list)
			{
				CPlayer* player = (CPlayer*)obj;
				if (m_pPlayer->GetCheckPoint() < player->GetCheckPoint())
				{
					gameTexts[idx++].text = player->GetName();
					rank++;
				}
				else
				{
					gameTexts[idx++].text = m_sPlayerId;
					for (int i = idx - 1; i < list.size(); ++i)
					{
						gameTexts[idx++].text = list[i]->GetName();
					}
					break;
				}
				if (list[list.size() - 1]->GetName() == obj->GetName())
					gameTexts[idx++].text = m_sPlayerId;
			}
			m_pPlayer->SetRank(rank);
		}

		//if (isMugen)
		//{
		//	m_fMugenTimer -= fTimeElapsed;
		//	if (m_fMugenTimer < 0)
		//		isMugen = false;
		//}
		if (isBoost)
		{
			m_fBoostTimer -= fTimeElapsed;
			if (m_fBoostTimer < 0)
			{
				m_pPlayer->SetMaxVelocityXZ(25);
				isBoost = false;
			}

		}
		//충돌을 위한 update
		if (sceneType != SceneType::ItemGame_Scene)
		{
			return sceneType;
		}
		

		MessageStruct message;

		for (CObjectsShader* shader : UpdatedShaders)
		{
			for (auto p = begin(shader->getList()); p < end(shader->getList()); ++p)
			{
				//플레이어 충돌처리할 곳
				if (m_pPlayer->IsCollide(*p))
				{
					if (m_pPlayer->Update(fTimeElapsed, *p))  //true반환 시 충돌된 오브젝트는 리스트에서 삭제
					{
						if ((*p)->GetModelType() == Item_Box)
						{
							auto time = std::chrono::system_clock::now();
							auto duration = time.time_since_epoch();
							auto randomSeed = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();

							std::mt19937 mtRand(randomSeed);
							std::uniform_int_distribution<int> randType(IconBanana, IconMeteorite);
							m_eCurrentItem = ItemIcon_type(randType(mtRand));
							instancingImageUiShaders[ITEM_UI]->getUvXs()[0] = 0.125f * m_eCurrentItem;

							// 이 곳에서도 서버연동시 바로 안만들고 신호부터 보낼것임.
							// 비활성화 시키는것 또한 신호를 보낼것임.
							
						}
					}
					
					//타겟 오브젝트 타입 구해오고 경우에 맞게 처리하도록 작성할 것
				}
			}
			shader->Update(fTimeElapsed);  //물리 적용할 것
		}

		//체크포인트 충돌처리
		if (m_pCheckPointShader)
		{
			UINT currentCheckPoint = m_pPlayer->GetCheckPoint();

			if (m_pPlayer->IsCollide(m_pCheckPointShader->getList()[currentCheckPoint % 181]))
				m_pPlayer->UpCheckPoint();
			else if (currentCheckPoint > 1) {
				if (m_pPlayer->IsCollide(m_pCheckPointShader->getList()[(currentCheckPoint - 2) % 181]))
					m_pPlayer->DownCheckPoint();
			}
		}

		for (CUiShader* shader : instancingNumberUiShaders)
			shader->Update(fTimeElapsed, m_pPlayer);

		XMFLOAT3 playerPosition = m_pPlayer->GetPosition();
		if (m_pMinimapCamera)
		{
			m_pMinimapCamera->SetPosition(XMFLOAT3(playerPosition.x, 300, playerPosition.z));
			m_pMinimapCamera->RegenerateViewMatrix();
		}
	}
	else
	{
		//isStart = true;
		//서버에서 스타트 신호를 받음. 모든 유저가 접속이 되었다면. start신호를 받고 isStart값을 true로 전환하고 
		//start대문이미지 출력할 것.
	}
	return ItemGame_Scene;
}

void ItemGameScene::BuildLights()
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));
	m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);

	m_pLights->m_pLights[0].m_bEnable = true;
	m_pLights->m_pLights[0].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.6f);
	m_pLights->m_pLights[0].m_xmf3Direction = XMFLOAT3(1.0f, -1.0f, 0.0f);
	
	m_pLights->m_pLights[1].m_bEnable = 1;
	m_pLights->m_pLights[1].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.5f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.2f, 0.2f, 0.5f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.6f);
	m_pLights->m_pLights[1].m_xmf3Position = XMFLOAT3(1342.0f, 208.0f, 5846.0f);
	m_pLights->m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[1].m_xmf3Attenuation = XMFLOAT3(0.7f, 0.0001f, 0.00001f);
	m_pLights->m_pLights[1].m_fFalloff = 1.0f;
	m_pLights->m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights->m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));
	m_pLights->m_pLights[1].m_fRange = 100.0f;
	m_pLights->m_pLights[1].padding = 0.0f;

	m_pLights->m_pLights[2].m_bEnable = 1;
	m_pLights->m_pLights[2].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.5f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.2f, 0.2f, 0.5f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.6f);
	m_pLights->m_pLights[2].m_xmf3Position = XMFLOAT3(1616.0f, 208.0f, 5931.0f);
	m_pLights->m_pLights[2].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[2].m_xmf3Attenuation = XMFLOAT3(0.7f, 0.0001f, 0.00001f);
	m_pLights->m_pLights[2].m_fFalloff = 1.0f;
	m_pLights->m_pLights[2].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights->m_pLights[2].m_fTheta = (float)cos(XMConvertToRadians(20.0f));
	m_pLights->m_pLights[2].m_fRange = 100.0f;
	m_pLights->m_pLights[2].padding = 0.0f;

	m_pLights->m_pLights[3].m_bEnable = 1;
	m_pLights->m_pLights[3].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[3].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.5f, 1.0f);
	m_pLights->m_pLights[3].m_xmf4Diffuse = XMFLOAT4(0.2f, 0.2f, 0.5f, 1.0f);
	m_pLights->m_pLights[3].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.6f);
	m_pLights->m_pLights[3].m_xmf3Position = XMFLOAT3(2163.0f, 208.0f, 5964.0f);
	m_pLights->m_pLights[3].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[3].m_xmf3Attenuation = XMFLOAT3(0.7f, 0.0001f, 0.00001f);
	m_pLights->m_pLights[3].m_fFalloff = 1.0f;
	m_pLights->m_pLights[3].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights->m_pLights[3].m_fTheta = (float)cos(XMConvertToRadians(20.0f));
	m_pLights->m_pLights[3].m_fRange = 100.0f;
	m_pLights->m_pLights[3].padding = 0.0f;

	m_pLights->m_pLights[4].m_bEnable = 1;
	m_pLights->m_pLights[4].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[4].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.5f, 1.0f);
	m_pLights->m_pLights[4].m_xmf4Diffuse = XMFLOAT4(0.2f, 0.2f, 0.5f, 1.0f);
	m_pLights->m_pLights[4].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.6f);
	m_pLights->m_pLights[4].m_xmf3Position = XMFLOAT3(1877.0f, 208.0f, 5800.0f);
	m_pLights->m_pLights[4].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[4].m_xmf3Attenuation = XMFLOAT3(0.7f, 0.0001f, 0.00001f);
	m_pLights->m_pLights[4].m_fFalloff = 1.0f;
	m_pLights->m_pLights[4].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights->m_pLights[4].m_fTheta = (float)cos(XMConvertToRadians(20.0f));
	m_pLights->m_pLights[4].m_fRange = 100.0f;
	m_pLights->m_pLights[4].padding = 0.0f;

	m_pLights->m_pLights[5].m_bEnable = 1;
	m_pLights->m_pLights[5].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[5].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.5f, 1.0f);
	m_pLights->m_pLights[5].m_xmf4Diffuse = XMFLOAT4(0.2f, 0.2f, 0.5f, 1.0f);
	m_pLights->m_pLights[5].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.6f);
	m_pLights->m_pLights[5].m_xmf3Position = XMFLOAT3(2490.0f, 208.0f, 5815.0f);
	m_pLights->m_pLights[5].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[5].m_xmf3Attenuation = XMFLOAT3(0.7f, 0.0001f, 0.00001f);
	m_pLights->m_pLights[5].m_fFalloff = 1.0f;
	m_pLights->m_pLights[5].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights->m_pLights[5].m_fTheta = (float)cos(XMConvertToRadians(20.0f));
	m_pLights->m_pLights[5].m_fRange = 100.0f;
	m_pLights->m_pLights[5].padding = 0.0f;
	m_pLights->fogstart = 25;
	m_pLights->fogrange = 30;

}
void ItemGameScene::BuildSubCameras(shared_ptr<CreateManager> pCreateManager)
{
	m_pMinimapCamera = new CMinimapCamera;
	m_pMinimapCamera->SetPosition(XMFLOAT3(0, 300, 0));
	m_pMinimapCamera->SetLookAt(XMFLOAT3(0, 0, 0));
	m_pMinimapCamera->GenerateOrthoProjectionMatrix(1000, 1000, 10, 1000.0f);
	float minimapWidth = pCreateManager->GetWindowWidth() / 5;
	float minimapHeight = pCreateManager->GetWindowHeight() / 4;
	m_pMinimapCamera->SetViewport(pCreateManager->GetWindowWidth() - minimapWidth, pCreateManager->GetWindowHeight() / 2 - (minimapHeight / 2), minimapWidth, minimapHeight, 0.0f, 1.0f);
	m_pMinimapCamera->SetScissorRect(0, 0, pCreateManager->GetWindowWidth(), pCreateManager->GetWindowHeight());

	m_pMinimapCamera->GenerateViewMatrix(m_pMinimapCamera->GetPosition(), XMFLOAT3(128 * TerrainScaleX, 0, 128 * TerrainScaleZ), XMFLOAT3(0, 0, 1));
	m_pMinimapCamera->CreateShaderVariables(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get());

	m_pShadowCamera = new CMinimapCamera;
	m_pShadowCamera->SetPosition(XMFLOAT3(0, 300, 0));
	m_pShadowCamera->SetLookAt(XMFLOAT3(0, 0, 0));
	m_pShadowCamera->SetViewport(0, 0, pCreateManager->GetWindowWidth(), pCreateManager->GetWindowHeight(), 0.0f, 1.0f);
	m_pShadowCamera->SetScissorRect(0, 0, pCreateManager->GetWindowWidth(), pCreateManager->GetWindowHeight());
}
void ItemGameScene:: ReBuildSubCameras(shared_ptr<CreateManager> pCreateManager)
{
	float minimapWidth = pCreateManager->GetWindowWidth() / 5;
	float minimapHeight = pCreateManager->GetWindowHeight() / 4;
	m_pMinimapCamera->SetViewport(pCreateManager->GetWindowWidth() - minimapWidth, pCreateManager->GetWindowHeight() / 2 - (minimapHeight / 2), minimapWidth, minimapHeight, 0.0f, 1.0f);
	m_pMinimapCamera->SetScissorRect(0, 0, pCreateManager->GetWindowWidth(), pCreateManager->GetWindowHeight());

	m_pShadowCamera->SetViewport(0, 0, pCreateManager->GetWindowWidth(), pCreateManager->GetWindowHeight(), 0.0f, 1.0f);
	m_pShadowCamera->SetScissorRect(0, 0, pCreateManager->GetWindowWidth(), pCreateManager->GetWindowHeight());
}
void ItemGameScene::UpdateShadow()
{
	XMFLOAT3 centerPosition(m_pPlayer->GetPosition());  //지형의 한 가운데
	float rad = 500;   // 지형을 담는 구의 반지름(ex 지구의 반지름)

	XMVECTOR lightDir = XMLoadFloat3(&m_pLights->m_pLights[0].m_xmf3Direction);
	lightDir = XMVector3Normalize(lightDir);

	XMVECTOR shadowCameraPosition = XMLoadFloat3(&centerPosition) - 2.0f*rad*lightDir;
	XMVECTOR targetPosition = XMLoadFloat3(&centerPosition);
	XMVECTOR shadowUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMFLOAT3 xmf3CameraPosition;
	XMFLOAT3 xmf3CameraTarget;
	XMStoreFloat3(&xmf3CameraPosition, shadowCameraPosition);
	XMStoreFloat3(&xmf3CameraTarget, targetPosition);

	m_pShadowCamera->GenerateViewMatrix(xmf3CameraPosition, xmf3CameraTarget, XMFLOAT3(0, 1, 0));
	m_pShadowCamera->GenerateFrustum();

#ifdef _WITH_LEFT_HAND_COORDINATES
	XMMATRIX shadowView = XMMatrixLookAtLH(shadowCameraPosition, targetPosition, shadowUp);
#else
	XMMATRIX shadowView = XMMatrixLookAtLH(shadowCameraPosition, targetPosition, shadowUp);
#endif

	XMStoreFloat3(&m_pcbMappedShadow->m_xmf3ShadowCameraPosition, shadowCameraPosition);

	// Transform bounding sphere to light space.
	XMFLOAT3 sphereCenterLS;
	XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPosition, shadowView));

	// Ortho frustum in light space encloses scene.
	float l = sphereCenterLS.x - rad;
	float b = sphereCenterLS.y - rad;
	float n = sphereCenterLS.z - rad;
	float r = sphereCenterLS.x + rad;
	float t = sphereCenterLS.y + rad;
	float f = sphereCenterLS.z + rad;

#ifdef _WITH_LEFT_HAND_COORDINATES
	XMMATRIX ShadowProj = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);
#else
	XMMATRIX ShadowProj = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);
#endif
	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX S = shadowView * ShadowProj*T;
	XMStoreFloat4x4(&m_pcbMappedShadow->m_xmf4x4ShadowView, XMMatrixTranspose(shadowView));
	XMStoreFloat4x4(&m_pcbMappedShadow->m_xmf4x4ShadowProjection, XMMatrixTranspose(ShadowProj));
	XMStoreFloat4x4(&m_pcbMappedShadow->m_xmf4x4InvShadowViewProjection, XMMatrixTranspose(S));
	XMFLOAT4X4 martrix;
	XMStoreFloat4x4(&martrix, ShadowProj);
	m_pShadowCamera->SetProjectionMatrix(martrix);
}
void ItemGameScene::CreateShaderVariables(CreateManager* pCreateManager)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = ::CreateBufferResource(pCreateManager->GetDevice().Get(), m_pd3dCommandList, NULL,
		ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbLights->Map(0, NULL, (void **)&m_pcbMappedLights);

	ncbElementBytes = ((sizeof(CB_GAME_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbShadow = ::CreateBufferResource(pCreateManager->GetDevice().Get(), m_pd3dCommandList, NULL,
		ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbShadow->Map(0, NULL, (void **)&m_pcbMappedShadow);

}


void ItemGameScene::UpdateShaderVariables()
{
	::memcpy(m_pcbMappedLights, m_pLights, sizeof(LIGHTS));

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbShadowGpuVirtualAddress =
		m_pd3dcbShadow->GetGPUVirtualAddress();

	m_pd3dCommandList->SetGraphicsRootConstantBufferView(1, d3dcbShadowGpuVirtualAddress);
	UpdateShadow();
}

void ItemGameScene::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}

	if (m_pd3dcbShadow)
	{
		m_pd3dcbShadow->Unmap(0, NULL);
		m_pd3dcbShadow->Release();
	}
}


void ItemGameScene::setPlayer(CPlayer* player)
{
	BaseScene::setPlayer(player);
	player->SetUpdatedContext((CHeightMapTerrain*)m_pTerrain);
}

void ItemGameScene::setCamera(CCamera* camera)
{
	BaseScene::setCamera(camera);
	m_pPlayer->SetCameraUpdatedContext((CHeightMapTerrain*)m_pTerrain);
}

void ItemGameScene::ResetShadowBuffer(CreateManager* pCreateManager)
{
	m_pTerrain->resetShadowTexture(pCreateManager);
}

void ItemGameScene::ProcessEvent(const MessageStruct& msg)
{
	if (msg.msgName == _ADD_OBJECT)
	{
		auto shader = find_if(instancingModelShaders.begin(), instancingModelShaders.end(), [&](CObInstancingShader* a) {
			return a->GetName() == msg.shaderName; });
		if (shader != instancingModelShaders.end())
			(*shader)->addObject(m_pCreateManager.get(), msg.departMat);
	}
	else if (msg.msgName == _DELETE_OBJECT)
	{
		auto shader = find_if(instancingModelShaders.begin(), instancingModelShaders.end(), [&](CObInstancingShader* a) {
			return a->GetName() == msg.shaderName; });
		if (shader != instancingModelShaders.end())
			(*shader)->DeleteObject(msg.objectName);
	}
	else if (msg.msgName == _ADD_PARTICLE)
	{
		XMFLOAT3 pos = XMFLOAT3(msg.departMat._41, msg.departMat._42, msg.departMat._43);
		particleSystems.emplace_back(new ParticleSystem(m_pCreateManager.get(), msg.shaderName, NULL, pos));
	}
	else if (msg.msgName == _DISENABLE_OBJECT)
	{
		auto shader = find_if(instancingModelShaders.begin(), instancingModelShaders.end(), [&](CObInstancingShader* a) {
			return a->GetName() == msg.shaderName; });
		if (shader != instancingModelShaders.end())
			(*shader)->DisEnableObject(msg.objectName);
	}
}

void ItemGameScene::ReSize(shared_ptr<CreateManager> pCreateManager)
{
	BaseScene::ReSize(pCreateManager);
	ResetShadowBuffer(m_pCreateManager.get());
	ReBuildSubCameras(pCreateManager);
}

void ItemGameScene::ProcessPacket(char* packet)
{


	SC_PACKET_PLAYER_ANIMATION t;
	//패킷의 switch문으로 행동 결정
	// 플레이어 위치값 갱신
	/*
	vector<CGameObject*> obList = PLAYER_SHADER->getList();
	auto obj = find_if(obList.begin(), obList.end(), [&](CGameObject* a) {
		return a->GetName() == t.Name; });
	if (obj != obList.end())
		(*obj)->m_xmf4x4ToParent = 행렬
	else
	{
		auto findId = find_if(obList.begin(), obList.end(), [&](CGameObject* a) {
			return a->GetName() == "None"; });
		if (findId != obList.end())
		{
			(*findId)->SetId(이름)
			(*findId)->m_xmf4x4ToParent = 행렬
			// 위치값 + 행렬
		}

	}
	*/

	// 플레이어 애니메이션
	/*
	vector<CGameObject*> obList = PLAYER_SHADER->getList();
	auto obj = find_if(obList.begin(), obList.end(), [&](CGameObject* a) {
		return a->GetName() == t.Name; });
	if (obj != obList.end())
	{
		CPlayer* player = (CPlayer*)(*obj);
		switch (t.animation)
		{
		case 'R':   //오른쪽 키 down
			player->KeyDownRight();
			break;
		default:
			break;
		}
	}
	*/
	//오브젝트 추가
	//패킷으로부터 MessageStruct의 정보를 받아야함. 물론 오브젝트를 생성하는 플레이어가
	//메시지 구조체 그대로 서버에 보내고 서버는 받은걸 그대로 타 플레이어에게 전해주면 좋을듯?


	/*
	MessageStruct를 이용하는 처리들은 이벤트 발생 시 클라에서 EventHandler의 callback함수로 해당 이벤트에
	대한 정보를 서버로 send하고 서버는 그 이벤트정보를 갖고 있다가 각 클라가 씬객체의 Update 
	최상단 부분에서 send로 이벤트정보를 요구할 때 넘겨주는건 어떨지? 
	*/
	/*
	MessageStruct message;   메시지 구조체 만들고
	message.shaderName = 오브젝트를 그리는 담당 셰이더 이름;
	message.departMat = 생성될 시 적용할 행령;
	message.arriveMat = 생성될 시 적용할 행령과 일치;
	message.msgName = "Add_Model";
	EventHandler::GetInstance()->CallBack(message); 클라 이벤트 핸들러에 등록  -끝-
	*/
	

	//오브젝트 삭제
	//얘 또한 추가처럼 메시지 구조체 정보 그대로만 받아올 수 있다면 쉬움
	/*
	MessageStruct message;   메시지 구조체 만들고
	message.objectName = 오브젝트 id(string);
	message.shaderName = 오브젝트를 그리는 담당 셰이더 이름;
	message.msgName = "DisEnable_Model";  명령어
	EventHandler::GetInstance()->CallBack(message); 클라 이벤트 핸들러에 등록  -끝-
	*/

	//파티클 생성
	//얘 또한 추가처럼 메시지 구조체 정보 그대로만 받아올 수 있다면 쉬움
	/*
	message.shaderName = 파티클 이름;
	message.departMat = 생성될 시 적용할 행령;
	message.msgName = "Add_Particle";
	EventHandler::GetInstance()->CallBack(message); 클라 이벤트 핸들러에 등록  -끝-
	

	//오브젝트 비활성화
	//위와 일치
	/*
	message.objectName = 오브젝트 id(string);
	message.shaderName = 오브젝트를 그리는 담당 셰이더 이름;
	message.msgName = "DisEnable_Model";
	EventHandler::GetInstance()->CallBack(message);클라 이벤트 핸들러에 등록  -끝-
	*/



	//끝났음을 알림
	//사운드 종료, 네트워크매니저에 패배 신호 주고 승리자는 이 패킷을 안받음.

}