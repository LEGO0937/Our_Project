#include "ItemGameScene.h"
#include "FrameWork/GameManager.h"
#include "FrameWork/NetWorkManager.h"
#include "FrameWork/SoundManager.h"

#include "PlayerObject.h"
#include "SkyBoxObject.h"
#include "TerrainObject.h"


#include "../CShaders/BillBoardShader/BillBoardShader.h"
#include "../CShaders/ModelShader/ModelShader.h"
#include "../CShaders/SkinedShader/SkinedShader.h"
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
#define TIME_COUNT_SHADER instancingNumberUiShaders[0]

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
void ItemGameScene::BuildObjects()
{
	m_fCountDownTime = 4.0f;  //임시로 클라에서 시간재기로함 서버 연동후 서버가 계산후 보내줄것.

	m_pd3dCommandList = GameManager::GetInstance()->GetCommandList().Get();

#ifdef isConnectedToServer
	//NetWorkManager::GetInstance()->ConnectToServer();
#endif
	XMFLOAT3 xmf3Scale(TerrainScaleX, TerrainScaleY, TerrainScaleZ);

	m_pSkyBox = new SkyBoxObject();
	GameManager::GetInstance()->RenderLoading();
	m_pTerrain = new CHeightMapTerrain(_T("Resources\\Images\\First_Map.raw"), 257, 257, 7,
		7, xmf3Scale);
	GameManager::GetInstance()->RenderLoading();
	GameManager::GetInstance()->RenderLoading();
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
	m_pEffectShader->BuildObjects(&view_info);
	view_info.positions.clear();
	view_info.f_uvY.clear();

	view_info.textureName = "Resources/Images/T_Speed.dds";
	view_info.meshSize = XMFLOAT2(0.07f, 0.1f);
	view_info.positions.emplace_back(XMFLOAT3(+0.93f, -0.92f, 0.0f));
	view_info.maxUv = XMFLOAT2(1.0f, 1.0f);
	view_info.minUv = XMFLOAT2(0.0f, 0.0f);
	view_info.f_uvY.emplace_back(0.0f);
	uiShader = new ImageShader;
	uiShader->BuildObjects(&view_info);
	instancingImageUiShaders.emplace_back(uiShader);
	view_info.positions.clear();
	view_info.f_uvY.clear();

	m_pCountDownShader = new CountDownShader;
	m_pCountDownShader->BuildObjects(NULL);
#ifdef isDebug
	bShader = new BillBoardShader;
	model_info.modelName = "Resources/Images/B_Tree.dds";
	model_info.dataFileName = "Resources/ObjectData/BillBoardData";
	model_info.size = 50;
	bShader->BuildObjects(&model_info);
	instancingBillBoardShaders.emplace_back(bShader);

	shader = new BillBoardShader;
	model_info.modelName = "Resources/Images/B_Stone.dds";
	model_info.dataFileName = "Resources/ObjectData/BillBoardStoneData";
	model_info.size = 50;
	shader->BuildObjects(&model_info);
	instancingBillBoardShaders.emplace_back(shader);

	shader = new BillBoardShader;
	model_info.modelName = "Resources/Images/B_Bush.dds";
	model_info.dataFileName = "Resources/ObjectData/BillBoardBushData";
	model_info.size = 50;
	shader->BuildObjects(&model_info);
	instancingBillBoardShaders.emplace_back(shader);
#endif

	shader = new ItemShader;
	model_info.modelName = "Resources/Models/M_Itembox.bin";
	model_info.dataFileName = "Resources/ObjectData/MeatData";
	model_info.useBillBoard = false;
	shader->BuildObjects(&model_info);
	instancingModelShaders.emplace_back(shader);
	shader->AddRef();
	UpdatedShaders.emplace_back(shader);

	shader = new BananaShader;
	model_info.modelName = "Resources/Models/M_Banana.bin";
	model_info.dataFileName = NULL;
	model_info.useBillBoard = true;
	shader->BuildObjects(&model_info);
	instancingModelShaders.emplace_back(shader);
	shader->AddRef();
	UpdatedShaders.emplace_back(shader);

	shader = new MudShader;
	model_info.modelName = "Resources/Models/M_Mud.bin";
	model_info.dataFileName = NULL;
	model_info.useBillBoard = false;
	shader->BuildObjects(&model_info);
	instancingModelShaders.emplace_back(shader);
	shader->AddRef();
	UpdatedShaders.emplace_back(shader);
	GameManager::GetInstance()->RenderLoading();
	shader = new StoneShader;
	model_info.modelName = "Resources/Models/M_Stone.bin";
	model_info.dataFileName = NULL;
	model_info.useBillBoard = true;
	shader->BuildObjects(&model_info);
	instancingModelShaders.emplace_back(shader);
	shader->AddRef();
	UpdatedShaders.emplace_back(shader);

	shader = new MeteoriteShader;
	model_info.modelName = "Resources/Models/M_Meteorite.bin";
	model_info.dataFileName = NULL;
	model_info.useBillBoard = false;
	shader->BuildObjects(&model_info);
	instancingModelShaders.emplace_back(shader);
	shader->AddRef();
	UpdatedShaders.emplace_back(shader);

	shader = new FogBoxShader;
	model_info.modelName = "Resources/Models/M_Fog.bin";
	model_info.dataFileName = NULL;
	shader->BuildObjects(&model_info);
	instancingModelShaders.emplace_back(shader);
	shader->AddRef();
	UpdatedShaders.emplace_back(shader);

	shader = new MoundShader;
	model_info.modelName = "Resources/Models/M_RockRIP.bin";
	model_info.dataFileName = NULL;
	shader->BuildObjects(&model_info);
	instancingModelShaders.emplace_back(shader);
	shader->AddRef();

	UpdatedShaders.emplace_back(shader);
	shader = new MeteoriteSpotShader;
	model_info.modelName = "Resources/Models/M_Shadow_Rip.bin";
	model_info.dataFileName = NULL;
	shader->BuildObjects(&model_info);
	instancingModelShaders.emplace_back(shader);
	shader->AddRef();
	UpdatedShaders.emplace_back(shader);

	

	GameManager::GetInstance()->RenderLoading();

#ifdef isDebug
	shader = new TreeShader;
	model_info.modelName = "Resources/Models/M_Tree.bin";
	model_info.dataFileName = "Resources/ObjectData/TreeData";
	model_info.useBillBoard = true;
	shader->BuildObjects(&model_info);
	instancingModelShaders.emplace_back(shader);

	shader = new TreeShader;
	model_info.modelName = "Resources/Models/M_Stone.bin";
	model_info.dataFileName = "Resources/ObjectData/StoneData";
	shader->BuildObjects(&model_info);
	instancingModelShaders.emplace_back(shader);
	GameManager::GetInstance()->RenderLoading();
	shader = new TreeShader;
	model_info.modelName = "Resources/Models/M_Bush.bin";
	model_info.dataFileName = "Resources/ObjectData/WeedData";
	shader->BuildObjects(&model_info);
	instancingModelShaders.emplace_back(shader);

	shader = new TreeShader;
	model_info.modelName = "Resources/Models/M_Tree2.bin";
	model_info.dataFileName = "Resources/ObjectData/Tree2Data";
	shader->BuildObjects(&model_info);
	instancingModelShaders.emplace_back(shader);

	shader = new TreeShader;
	model_info.modelName = "Resources/Models/M_Tree3.bin";
	model_info.dataFileName = "Resources/ObjectData/Tree3Data";
	shader->BuildObjects(&model_info);
	instancingModelShaders.emplace_back(shader);

	shader = new TreeShader;
	model_info.modelName = "Resources/Models/M_Tree4.bin";
	model_info.dataFileName = "Resources/ObjectData/Tree4Data";
	shader->BuildObjects(&model_info);
	instancingModelShaders.emplace_back(shader);

	//--
	shader = new TreeShader;
	model_info.modelName = "Resources/Models/M_Stone1.bin";
	model_info.dataFileName = "Resources/ObjectData/Stone1Data";
	shader->BuildObjects(&model_info);
	instancingModelShaders.emplace_back(shader);
	//
	shader = new TreeShader;
	model_info.modelName = "Resources/Models/M_Grass.bin";
	model_info.dataFileName = "Resources/ObjectData/Grass1Data";
	shader->BuildObjects(&model_info);
	instancingModelShaders.emplace_back(shader);
	//
	shader = new TreeShader;
	model_info.modelName = "Resources/Models/M_Stone3.bin";
	model_info.dataFileName = "Resources/ObjectData/Stone3Data";
	shader->BuildObjects(&model_info);
	instancingModelShaders.emplace_back(shader);

	model_info.useBillBoard = false;
	shader = new TreeShader;
	model_info.modelName = "Resources/Models/M_Sub_Dino.bin";
	model_info.dataFileName = "Resources/ObjectData/DinoData";
	shader->BuildObjects(&model_info);
	instancingModelShaders.emplace_back(shader);

	shader = new TreeShader;
	model_info.modelName = "Resources/Models/M_Sub_Dino2.bin";
	model_info.dataFileName = "Resources/ObjectData/Dino2Data";
	shader->BuildObjects(&model_info);
	instancingModelShaders.emplace_back(shader);
#endif

	shader = new FenceShader;
	model_info.modelName = "Resources/Models/M_Block.bin";
	model_info.dataFileName = "Resources/ObjectData/RectData(Fence)";
	model_info.useBillBoard = false;
	shader->BuildObjects(&model_info);
	instancingModelShaders.emplace_back(shader);
	shader->AddRef();
	UpdatedShaders.emplace_back(shader);

	m_pCheckPointShader = new BlockShader;
	model_info.modelName = "Resources/Models/M_Block.bin";
	model_info.dataFileName = "Resources/ObjectData/RectData(LineBox)";
	m_pCheckPointShader->BuildObjects(&model_info);

	shader = new ModelShader;
	model_info.modelName = "Resources/Models/M_Cavern.bin";
	model_info.dataFileName = "Resources/ObjectData/CaveData";
	shader->BuildObjects(&model_info);
	instancingModelShaders.emplace_back(shader);

	uiShader = new TimeCountShader;
	uiShader->BuildObjects(NULL);
	instancingNumberUiShaders.emplace_back(uiShader);

	uiShader = new TrackCountShader;
	uiShader->BuildObjects(NULL);
	instancingNumberUiShaders.emplace_back(uiShader);

	uiShader = new RankCountShader;
	uiShader->BuildObjects(NULL);
	instancingNumberUiShaders.emplace_back(uiShader);

	uiShader = new VelocityCountShader;
	uiShader->BuildObjects(m_pTerrain);
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
	uiShader->BuildObjects(&ItemUi_info);
	instancingImageUiShaders.emplace_back(uiShader);

	//보유 아이템 사진
	ItemUi_info.textureName = "Resources/Images/T_Itemsprite.dds";
	ItemUi_info.maxUv = XMFLOAT2(0.125f, 1.0f);

	uiShader = new ImageShader;
	uiShader->BuildObjects(&ItemUi_info);
	uiShader->getUvXs()[0] = 0.0f;  //스프라이트의 간격은 0.125f
	instancingImageUiShaders.emplace_back(uiShader);

	animatedShader = new PlayerShader;
	model_info.modelName = "Resources/Models/M_DinoTest.bin";
	model_info.dataFileName = NULL;
	animatedShader->BuildObjects(&model_info);
	instancingAnimatedModelShaders.emplace_back(animatedShader);
	animatedShader->AddRef();
	UpdatedShaders.emplace_back(animatedShader);
	 
	m_pMinimapShader = new MinimapShader();
	m_pMinimapShader->BuildObjects("Resources/Images/MiniMap.dds", NULL);

	string name = "Resources/Images/T_Faceicon.dds";
	m_pIconShader = new IconShader();
	m_pIconShader->BuildObjects(&name);

	motionBlurShader = new MotionBlurShader();

	XMFLOAT3 startPosition = m_pCheckPointShader->getList()[0]->GetPosition();
	particleSystems.emplace_back(new ParticleSystem(SPAWN, NULL, XMFLOAT3(startPosition.x, m_pTerrain->GetHeight(startPosition.x, startPosition.z), startPosition.z)));
	particleSystems.emplace_back(new ParticleSystem( SPAWN, NULL, XMFLOAT3(startPosition.x - 50, m_pTerrain->GetHeight(startPosition.x, startPosition.z), startPosition.z)));
	particleSystems.emplace_back(new ParticleSystem( SPAWN, NULL, XMFLOAT3(startPosition.x + 50, m_pTerrain->GetHeight(startPosition.x, startPosition.z), startPosition.z)));

	BuildLights();

	BuildSubCameras();

	gameTexts.emplace_back(GameText(XMFLOAT2(0.05f, 0.19f))); //플레이어 명단
	gameTexts.emplace_back(GameText(XMFLOAT2(0.05f, 0.25f)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.05f, 0.31f)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.05f, 0.37f)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.05f, 0.43f)));

	CreateShaderVariables();
	GameManager::GetInstance()->RenderLoading();

	SoundManager::GetInstance()->Play("InGame_BGM", 0.1f);
	GameManager::GetInstance()->RenderLoading();

#ifdef isConnectedToServer
	NetWorkManager::GetInstance()->SendInGameReady();
#else
	isAllConnected = true;
#endif
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
			break;
		case VK_RIGHT:
			break;
		case VK_UP:
			break;
		case VK_DOWN:
			break;
		case VK_ESCAPE:
#ifdef isConnectedToServer
			NetWorkManager::GetInstance()->SendInGameFinish();
#else
			EventHandler::GetInstance()->m_iMinute = ((TimeCountShader*)TIME_COUNT_SHADER)->GetMinute();
			EventHandler::GetInstance()->m_fSecond = ((TimeCountShader*)TIME_COUNT_SHADER)->GetSecond();
			EventHandler::GetInstance()->m_sWinner = NetWorkManager::GetInstance()->GetPlayerName();

			sceneType = End_Scene;  //멀티 플레이시 이 구간에서 서버로부터 골인한 플레이어를 확인후 씬 전환
#endif
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
#ifndef isConnectedToServer
				EventHandler::GetInstance()->RegisterEvent(message);
#else
				NetWorkManager::GetInstance()->SendEvent(message);
#endif
				break;
			case IconMeat:
				SoundManager::GetInstance()->Play("MeatEat", 0.5f);
				m_pPlayer->SetMaxVelocityXZ(50);
				m_fBoostTimer = 10;
				isBoost = true;
				break;
			case IconMeteorite:
				int checkCount = (m_pPlayer->GetCheckPoint() + 9) % 181;
				message.shaderName = _METEORITE_SHADER;
				CGameObject* checkPoint = m_pCheckPointShader->getList()[checkCount];
				XMFLOAT4X4 mat = checkPoint->m_xmf4x4World;
				mat._42 += 500.0f;
				message.departMat = mat;
				message.msgName = _ADD_OBJECT;
#ifndef isConnectedToServer
				EventHandler::GetInstance()->RegisterEvent(message);
#else
				NetWorkManager::GetInstance()->SendEvent(message);
#endif
				//---
				message.shaderName = _METEORITESPOT_SHADER;
				mat = checkPoint->m_xmf4x4World;
				mat._42 += 1.0f;
				message.departMat = mat;
				message.msgName = _ADD_OBJECT;
#ifndef isConnectedToServer
				EventHandler::GetInstance()->RegisterEvent(message);
#else
				NetWorkManager::GetInstance()->SendEvent(message);
#endif
				break;
			}
			m_eCurrentItem = IconDefault;
			instancingImageUiShaders[ITEM_UI]->getUvXs()[0] = 0.125f * m_eCurrentItem;
			break;
		case VK_F1:
			SoundManager::GetInstance()->Play("MeatEat", 0.5f);
			m_pPlayer->SetMaxVelocityXZ(50);
			m_fBoostTimer = 10;
			isBoost = true;
			break;
		case VK_F2:
		case VK_F3:
			if (m_pPlayer) m_pCamera = m_pPlayer->ChangeCamera((wParam - VK_F1 + 1),
				deltaTime);
			break;
		case VK_F4:
			message.shaderName = _METEORITE_SHADER;
			XMFLOAT4X4 mat = m_pPlayer->m_xmf4x4World;
			mat._42 += 500.0f;
			message.departMat = mat;
			message.msgName = _ADD_OBJECT;
#ifndef isConnectedToServer
			EventHandler::GetInstance()->RegisterEvent(message);
#else
			NetWorkManager::GetInstance()->SendEvent(message);
#endif
			//---
			message.shaderName = _METEORITESPOT_SHADER;
			mat = m_pPlayer->m_xmf4x4World;
			mat._42 += -6.0f;
			message.departMat = mat;
			message.msgName = _ADD_OBJECT;
#ifndef isConnectedToServer
			EventHandler::GetInstance()->RegisterEvent(message);
#else
			NetWorkManager::GetInstance()->SendEvent(message);
#endif
			break;
		case VK_F5:
			XMFLOAT4X4 matrix1 = m_pPlayer->m_xmf4x4ToParent;
			XMFLOAT3 pos1 = m_pPlayer->GetLook();
			pos1 = Vector3::ScalarProduct(pos1, 40, false);

			matrix1._41 -= pos1.x;
			matrix1._43 -= pos1.z;
			matrix1._42 = m_pTerrain->GetHeight(matrix1._41, 256 * TerrainScaleZ - matrix1._43) + 1.0f;
			XMFLOAT3 up1 = m_pTerrain->GetNormal(matrix1._41, matrix1._43);


			matrix1._21 = up1.x;
			matrix1._22 = up1.y;
			matrix1._23 = up1.z;
			XMFLOAT3 look1 = Vector3::CrossProduct(m_pPlayer->GetRight(), up1, true);

			matrix1._31 = look1.x;
			matrix1._32 = look1.y;
			matrix1._33 = look1.z;

			//이부분에도 바로 추가하지않고 신호를 보냄. 업데이트에서 신호를 받아서 추가하도록 한다.
			message.shaderName = _BANANA_SHADER;
			message.departMat = matrix1;
			message.msgName = _ADD_OBJECT;
#ifndef isConnectedToServer
			EventHandler::GetInstance()->RegisterEvent(message);
#else
			NetWorkManager::GetInstance()->SendEvent(message);
#endif
			break;
		case VK_F6:
			XMFLOAT4X4 matrix2 = m_pPlayer->m_xmf4x4ToParent;
			XMFLOAT3 pos2 = m_pPlayer->GetLook();
			pos2 = Vector3::ScalarProduct(pos2, 40, false);

			matrix2._41 -= pos2.x;
			matrix2._43 -= pos2.z;
			matrix2._42 = m_pTerrain->GetHeight(matrix2._41, 256 * TerrainScaleZ - matrix2._43) + 1.0f;
			XMFLOAT3 up2 = m_pTerrain->GetNormal(matrix2._41, matrix2._43);


			matrix2._21 = up2.x;
			matrix2._22 = up2.y;
			matrix2._23 = up2.z;
			XMFLOAT3 look2 = Vector3::CrossProduct(m_pPlayer->GetRight(), up2, true);

			matrix2._31 = look2.x;
			matrix2._32 = look2.y;
			matrix2._33 = look2.z;

			//이부분에도 바로 추가하지않고 신호를 보냄. 업데이트에서 신호를 받아서 추가하도록 한다.
			message.shaderName = _MUD_SHADER;
			message.departMat = matrix2;
			message.msgName = _ADD_OBJECT;
#ifndef isConnectedToServer
			EventHandler::GetInstance()->RegisterEvent(message);
#else
			NetWorkManager::GetInstance()->SendEvent(message);
#endif
			break;
		case VK_F7:
			XMFLOAT4X4 matrix3 = m_pPlayer->m_xmf4x4ToParent;
			XMFLOAT3 pos3 = m_pPlayer->GetLook();
			pos3 = Vector3::ScalarProduct(pos3, 40, false);

			matrix3._41 -= pos3.x;
			matrix3._43 -= pos3.z;
			matrix3._42 = m_pTerrain->GetHeight(matrix3._41, 256 * TerrainScaleZ - matrix3._43) + 1.0f;
			XMFLOAT3 up3 = m_pTerrain->GetNormal(matrix3._41, matrix3._43);


			matrix3._21 = up3.x;
			matrix3._22 = up3.y;
			matrix3._23 = up3.z;
			XMFLOAT3 look3 = Vector3::CrossProduct(m_pPlayer->GetRight(), up3, true);

			matrix3._31 = look3.x;
			matrix3._32 = look3.y;
			matrix3._33 = look3.z;

			//이부분에도 바로 추가하지않고 신호를 보냄. 업데이트에서 신호를 받아서 추가하도록 한다.
			message.shaderName = _STONE_SHADER;
			message.departMat = matrix3;
			message.msgName = _ADD_OBJECT;
#ifndef isConnectedToServer
			EventHandler::GetInstance()->RegisterEvent(message);
#else
			NetWorkManager::GetInstance()->SendEvent(message);
#endif
			break;

		case VK_F8:
			XMFLOAT4X4 matrix4 = m_pPlayer->m_xmf4x4ToParent;
			XMFLOAT3 pos4 = m_pPlayer->GetLook();
			pos4 = Vector3::ScalarProduct(pos4, 40, false);

			matrix4._41 -= pos4.x;
			matrix4._43 -= pos4.z;
			matrix4._42 = m_pTerrain->GetHeight(matrix4._41, 256 * TerrainScaleZ - matrix4._43) + 1.0f;
			XMFLOAT3 up4 = m_pTerrain->GetNormal(matrix4._41, matrix4._43);


			matrix4._21 = up4.x;
			matrix4._22 = up4.y;
			matrix4._23 = up4.z;
			XMFLOAT3 look4 = Vector3::CrossProduct(m_pPlayer->GetRight(), up4, true);

			matrix4._31 = look4.x;
			matrix4._32 = look4.y;
			matrix4._33 = look4.z;

			//이부분에도 바로 추가하지않고 신호를 보냄. 업데이트에서 신호를 받아서 추가하도록 한다.
			message.shaderName = _FOGBOX_SHADER;
			message.departMat = matrix4;
			message.msgName = _ADD_OBJECT;
#ifndef isConnectedToServer
			EventHandler::GetInstance()->RegisterEvent(message);
#else
			NetWorkManager::GetInstance()->SendEvent(message);
#endif
			break;
		case VK_LEFT:
			break;
		case VK_RIGHT:
			break;
			break;
		case VK_UP:
			break;
		case VK_DOWN:
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
	
	POINT ptCursorPos;
	if (GetCapture() == hwnd)
	{
		SetCursor(NULL);
		GetCursorPos(&ptCursorPos);
		cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
		SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
	}
	//마우스 또는 키 입력이 있으면 플레이어를 이동하거나(dwDirection) 회전한다(cxDelta 또는 cyDelta).
	if ((cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		if ((cxDelta || cyDelta) && m_pCamera->GetMode() == SPACESHIP_CAMERA)
		{
			if (pKeyBuffer[VK_RBUTTON] & 0xF0)
				m_pPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
			else
				m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
		}
	}

	if (dwDirection == 0)
		m_pPlayer->m_fForce = 0;

	m_pPlayer->Move(dwDirection, 800.0f*deltaTime, deltaTime, true);
}

void ItemGameScene::Render()
{
	BaseScene::Render();

	if (m_pCamera->m_fFogStart < 5.0f)
	{
		m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_CUBE_MAP]);
		if (m_pSkyBox)
			m_pSkyBox->Render(m_pd3dCommandList, m_pCamera);
	}
	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_TERRAIN]);
	if (m_pTerrain) m_pTerrain->Render(m_pd3dCommandList, m_pCamera);

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

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_SKIN_MESH_INSTANCING]);
	for (CSkinedObInstancingShader* shader : instancingAnimatedModelShaders)
		if (shader) {
			shader->Render(m_pd3dCommandList, m_pCamera);
		}


	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_PARTICLE]);

	if (Vector3::Length(m_pPlayer->m_xmf3Velocity) > 0)
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
	m_pPlayer->Render(m_pd3dCommandList, m_pCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_SHADOW_MODEL_INSTANCING]);
	for (CObInstancingShader* shader : instancingModelShaders)
		if (shader) shader->Render(m_pd3dCommandList, m_pCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_SHADOW_BILLBOARD]);
	for (CObInstancingShader* shader : instancingBillBoardShaders)
		if (shader) shader->Render(m_pd3dCommandList, m_pCamera);
	for (CObInstancingShader* shader : instancingModelShaders)
	{
		if (shader)
			shader->BillBoardRender(m_pd3dCommandList, m_pCamera);
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
}

void ItemGameScene::RenderPostProcess(ComPtr<ID3D12Resource> curBuffer, ComPtr<ID3D12Resource> velocityMap)
{
	static float deltaUvX = 0.0f;
	XMFLOAT3 vel = m_pPlayer->GetVelocity();
	float length = sqrtf(vel.x * vel.x + vel.z * vel.z);
	if (length > 30)
	{
		if (!SoundManager::GetInstance()->Playing("Boost"))
			SoundManager::GetInstance()->Play("Boost", 0.5f);
		
		m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_MOTION_BLUR]);
		motionBlurShader->Dispatch(m_pd3dCommandList, curBuffer.Get(), velocityMap.Get(), 10);
		m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_EFFECT]);
		m_pEffectShader->Render(m_pd3dCommandList, m_pCamera);
		m_pEffectShader->getUvXs()[0] = deltaUvX;
		deltaUvX += 0.125f;
		if (deltaUvX >= 1.0)
			deltaUvX = 0.0f;
	}
	else
		if (SoundManager::GetInstance()->Playing("Boost"))
			SoundManager::GetInstance()->Stop("Boost");
	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_UI_NUMBER]);
	for (CUiShader* shader : instancingNumberUiShaders)
		if (shader) shader->Render(m_pd3dCommandList, m_pCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_UI]);
	for (CUiShader* shader : instancingImageUiShaders)
		if (shader) shader->Render(m_pd3dCommandList, m_pCamera);
	if (m_pCountDownShader)
		m_pCountDownShader->Render(m_pd3dCommandList, m_pCamera);

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_FONT]);
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


void ItemGameScene::FixedUpdate(float fTimeElapsed)
{
	
	//물리
	{
		m_pPlayer->FixedUpdate(fTimeElapsed);

		for (CObjectsShader* shader : UpdatedShaders)
		{
			shader->FixedUpdate(fTimeElapsed);  //물리 적용할 것
		}
	}

	if (isAllConnected)
	{
		if (!isStart)
		{
			if (m_fCountDownTime > 0.9f)
			{
				m_fCountDownTime -= fTimeElapsed;
			}
			else
			{
				m_fCountDownTime = 0.0f;
				isStart = true;
			}
			
		}
		m_pCountDownShader->Update(fTimeElapsed, &m_fCountDownTime);
	}
}


SceneType ItemGameScene::Update(float fTimeElapsed)
{
	m_pCamera->m_fFogStart = 0.0f;

	if (sceneType != SceneType::ItemGame_Scene)
	{
		SoundManager::GetInstance()->AllStop();
		return sceneType;
	}

	
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

#ifdef isConnectedToServer
			NetWorkManager::GetInstance()->SendInGameFinish();

#else
			SoundManager::GetInstance()->AllStop();
			EventHandler::GetInstance()->m_iMinute = ((TimeCountShader*)TIME_COUNT_SHADER)->GetMinute();
			EventHandler::GetInstance()->m_fSecond = ((TimeCountShader*)TIME_COUNT_SHADER)->GetSecond();
			EventHandler::GetInstance()->m_sWinner = NetWorkManager::GetInstance()->GetPlayerName();

			sceneType = End_Scene;  //멀티 플레이시 이 구간에서 서버로부터 골인한 플레이어를 확인후 씬 전환
#endif
		}
		else
		{
			int rank = 1;
			 vector<CGameObject*> list = PLAYER_SHADER->getList();
			

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
#ifdef isConnectedToServer

#else
			if (((CPlayer*)list[0])->GetCheckPoint() == CHECKPOINT_GOAL)
			{
				EventHandler::GetInstance()->m_iMinute = ((TimeCountShader*)TIME_COUNT_SHADER)->GetMinute();
				EventHandler::GetInstance()->m_fSecond = ((TimeCountShader*)TIME_COUNT_SHADER)->GetSecond();
				EventHandler::GetInstance()->m_sWinner = ((CPlayer*)list[0])->GetName();

				sceneType = End_Scene;

			}
#endif
		}

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
			SoundManager::GetInstance()->AllStop();
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
						}
					}
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
	}

#ifdef isConnectedToServer
	NetWorkManager::GetInstance()->SendPlayerInfo(m_pPlayer->GetCheckPoint(), dwDirection, m_pPlayer->m_xmf4x4ToParent);
#endif
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
	m_pLights->m_pLights[0].m_xmf3Direction = XMFLOAT3(1.0f, -0.7f, 0.0f);
	
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
void ItemGameScene::BuildSubCameras()
{
	m_pMinimapCamera = new CMinimapCamera;
	m_pMinimapCamera->SetPosition(XMFLOAT3(0, 300, 0));
	m_pMinimapCamera->SetLookAt(XMFLOAT3(0, 0, 0));
	m_pMinimapCamera->GenerateOrthoProjectionMatrix(1000, 1000, 10, 1000.0f);
	float minimapWidth = GameManager::GetInstance()->GetWindowWidth() / 5;
	float minimapHeight = GameManager::GetInstance()->GetWindowHeight() / 4;
	m_pMinimapCamera->SetViewport(GameManager::GetInstance()->GetWindowWidth() - minimapWidth, GameManager::GetInstance()->GetWindowHeight() / 2 - (minimapHeight / 2), minimapWidth, minimapHeight, 0.0f, 1.0f);
	m_pMinimapCamera->SetScissorRect(0, 0, GameManager::GetInstance()->GetWindowWidth(), GameManager::GetInstance()->GetWindowHeight());

	m_pMinimapCamera->GenerateViewMatrix(m_pMinimapCamera->GetPosition(), XMFLOAT3(128 * TerrainScaleX, 0, 128 * TerrainScaleZ), XMFLOAT3(0, 0, 1));
	m_pMinimapCamera->CreateShaderVariables(GameManager::GetInstance()->GetDevice().Get(), GameManager::GetInstance()->GetCommandList().Get());

	m_pShadowCamera = new CMinimapCamera;
	m_pShadowCamera->SetPosition(XMFLOAT3(0, 300, 0));
	m_pShadowCamera->SetLookAt(XMFLOAT3(0, 0, 0));
	m_pShadowCamera->SetViewport(0, 0, GameManager::GetInstance()->GetWindowWidth(), GameManager::GetInstance()->GetWindowHeight(), 0.0f, 1.0f);
	m_pShadowCamera->SetScissorRect(0, 0, GameManager::GetInstance()->GetWindowWidth(), GameManager::GetInstance()->GetWindowHeight());
}
void ItemGameScene:: ReBuildSubCameras()
{
	float minimapWidth = GameManager::GetInstance()->GetWindowWidth() / 5;
	float minimapHeight = GameManager::GetInstance()->GetWindowHeight() / 4;
	m_pMinimapCamera->SetViewport(GameManager::GetInstance()->GetWindowWidth() - minimapWidth, GameManager::GetInstance()->GetWindowHeight() / 2 - (minimapHeight / 2), minimapWidth, minimapHeight, 0.0f, 1.0f);
	m_pMinimapCamera->SetScissorRect(0, 0, GameManager::GetInstance()->GetWindowWidth(), GameManager::GetInstance()->GetWindowHeight());

	m_pShadowCamera->SetViewport(0, 0, GameManager::GetInstance()->GetWindowWidth(), GameManager::GetInstance()->GetWindowHeight(), 0.0f, 1.0f);
	m_pShadowCamera->SetScissorRect(0, 0, GameManager::GetInstance()->GetWindowWidth(), GameManager::GetInstance()->GetWindowHeight());
}
void ItemGameScene::UpdateShadow()
{
	XMFLOAT3 centerPosition(m_pPlayer->GetPosition());  //지형의 한 가운데
	float rad = 600;   // 지형을 담는 구의 반지름(ex World의 반지름)

	XMVECTOR lightDir = XMLoadFloat3(&m_pLights->m_pLights[0].m_xmf3Direction);
	lightDir = XMVector3Normalize(lightDir);

	XMVECTOR shadowCameraPosition = XMLoadFloat3(&centerPosition) - 1.0f*rad*lightDir;
	XMVECTOR targetPosition = XMLoadFloat3(&centerPosition);
	XMVECTOR shadowUp = XMVectorSet(1.0f, 1.0f, 0.0f, 0.0f);

	XMFLOAT3 xmf3CameraPosition;
	XMFLOAT3 xmf3CameraTarget;
	XMStoreFloat3(&xmf3CameraPosition, shadowCameraPosition);
	XMStoreFloat3(&xmf3CameraTarget, targetPosition);

	m_pShadowCamera->GenerateViewMatrix(xmf3CameraPosition, xmf3CameraTarget, XMFLOAT3(0, 1, 0));
	m_pShadowCamera->GenerateFrustum();

#ifdef _WITH_LEFT_HAND_COORDINATES
	XMMATRIX shadowView = XMMatrixLookAtLH(shadowCameraPosition, targetPosition, shadowUp);
#else
	XMMATRIX shadowView = XMMatrixLookAtRH(shadowCameraPosition, targetPosition, shadowUp);
#endif

	XMStoreFloat3(&m_pcbMappedShadow->m_xmf3ShadowCameraPosition, shadowCameraPosition);

	XMFLOAT3 xmf3TargetPos;
	XMStoreFloat3(&xmf3TargetPos, XMVector3TransformCoord(targetPosition, shadowView));

	float l = xmf3TargetPos.x - rad;
	float b = xmf3TargetPos.y - rad;
	float n = xmf3TargetPos.z - rad;
	float r = xmf3TargetPos.x + rad;
	float t = xmf3TargetPos.y + rad;
	float f = xmf3TargetPos.z + rad;

#ifdef _WITH_LEFT_HAND_COORDINATES
	XMMATRIX ShadowProj = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);
#else
	XMMATRIX ShadowProj = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);
#endif
	//아래는 역행렬을 구하기 위한 행렬
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
void ItemGameScene::CreateShaderVariables()
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = ::CreateBufferResource(GameManager::GetInstance()->GetDevice().Get(), m_pd3dCommandList, NULL,
		ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbLights->Map(0, NULL, (void **)&m_pcbMappedLights);

	ncbElementBytes = ((sizeof(CB_GAME_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbShadow = ::CreateBufferResource(GameManager::GetInstance()->GetDevice().Get(), m_pd3dCommandList, NULL,
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

void ItemGameScene::ResetShadowBuffer()
{
	m_pTerrain->resetShadowTexture();
}

void ItemGameScene::ProcessEvent(const MessageStruct& msg)
{
	if (msg.msgName == _ADD_OBJECT)
	{
		if (instancingModelShaders[msg.shaderName])
			instancingModelShaders[msg.shaderName]->addObject(msg.departMat);
	}
	else if (msg.msgName == _DELETE_OBJECT)
	{
		if (instancingModelShaders[msg.shaderName])
			instancingModelShaders[msg.shaderName]->DeleteObject(msg.objectSerialNum);
	}
	else if (msg.msgName == _ADD_PARTICLE)
	{
		XMFLOAT3 pos = XMFLOAT3(msg.departMat._41, msg.departMat._42, msg.departMat._43);
		particleSystems.emplace_back(new ParticleSystem(msg.shaderName, NULL, pos));
	}
	else if (msg.msgName == _DISENABLE_OBJECT)
	{
		if (instancingModelShaders[msg.shaderName])
			instancingModelShaders[msg.shaderName]->DisEnableObject(msg.objectSerialNum);
	}
}

void ItemGameScene::ReSize()
{
	BaseScene::ReSize();
	ResetShadowBuffer();
	ReBuildSubCameras();
	motionBlurShader->OnResize(GameManager::GetInstance()->GetWindowWidth(), GameManager::GetInstance()->GetWindowHeight());
}

void ItemGameScene::ProcessPacket(char* packet, float fTimeElapsed)
{
	SC_PACKET_PLAYER_ANIMATION t;

	switch (packet[1])
	{
	case SC_PLAYER_INFO:
		UpdatePlayerInfo(packet, fTimeElapsed);//플레이어 정보 처리
		break;
	case SC_EVENT:
		UpdateEventInfo(packet, fTimeElapsed); //이벤트처리
		break;
	
	case SC_INGAME_READY: // 플레이어의 모든 연결이 끝났다고 서버로부터 받는 패킷처리 
		//이 패킷을 받으면 바로 게임 카운트다운 시작
		UpdateStartInfo(packet, fTimeElapsed);
		break;
	case SC_INGAME_FINISH:
		UpdateFinishInfo(packet, fTimeElapsed);
		break;
	case SC_SLIDING_ANI:
		UpdatePlayerSliding(packet, fTimeElapsed);
		break;
	case SC_COLLISION_ANI:
		UpdatePlayerCollision(packet, fTimeElapsed);
		break;
	default:
		break;
	}
}

void ItemGameScene::UpdatePlayerInfo(char* packet, float fTimeElapsed)
{
	SC_PACKET_PLAYER_INFO* playerInfo = reinterpret_cast<SC_PACKET_PLAYER_INFO*>(packet);

	if (NetWorkManager::GetInstance()->GetMyID() == playerInfo->id)
		return;

	vector<CGameObject*> obList = PLAYER_SHADER->getList();
	auto obj = find_if(obList.begin(), obList.end(), [&](CGameObject* a) {
		return a->GetId() == playerInfo->id; });
	if (obj != obList.end())
	{
		(*obj)->m_xmf4x4ToParent = playerInfo->xmf4x4Parents;
		((CPlayer*)(*obj))->SetPosition(XMFLOAT3((*obj)->m_xmf4x4ToParent._41,
			(*obj)->m_xmf4x4ToParent._42, (*obj)->m_xmf4x4ToParent._43));
		((CPlayer*)(*obj))->SetRight(XMFLOAT3((*obj)->m_xmf4x4ToParent._11,
			(*obj)->m_xmf4x4ToParent._12, (*obj)->m_xmf4x4ToParent._13));
		((CPlayer*)(*obj))->SetUp(XMFLOAT3((*obj)->m_xmf4x4ToParent._21,
			(*obj)->m_xmf4x4ToParent._22, (*obj)->m_xmf4x4ToParent._23));
		((CPlayer*)(*obj))->SetLook(XMFLOAT3((*obj)->m_xmf4x4ToParent._31,
			(*obj)->m_xmf4x4ToParent._32, (*obj)->m_xmf4x4ToParent._33));

		((CPlayer*)(*obj))->Move(playerInfo->keyState, 20.0f, fTimeElapsed, true);
		((CPlayer*)(*obj))->SetCheckPoint(playerInfo->checkPoints);
	}
	else
	{
		auto findId = find_if(obList.begin(), obList.end(), [&](CGameObject* a) {
			return a->GetId() == -1; });
		if (findId != obList.end())
		{
			(*findId)->SetId(playerInfo->id);
			(*findId)->SetName(playerInfo->playerNames); // 없을경우 이름지정
			(*findId)->m_xmf4x4ToParent = playerInfo->xmf4x4Parents; //변환행렬 처리
			((CPlayer*)(*findId))->SetPosition(XMFLOAT3((*findId)->m_xmf4x4ToParent._41,
				(*findId)->m_xmf4x4ToParent._42, (*findId)->m_xmf4x4ToParent._43));
			((CPlayer*)(*findId))->SetRight(XMFLOAT3((*findId)->m_xmf4x4ToParent._11,
				(*findId)->m_xmf4x4ToParent._12, (*findId)->m_xmf4x4ToParent._13));
			((CPlayer*)(*findId))->SetUp(XMFLOAT3((*findId)->m_xmf4x4ToParent._21,
				(*findId)->m_xmf4x4ToParent._22, (*findId)->m_xmf4x4ToParent._23));
			((CPlayer*)(*findId))->SetLook(XMFLOAT3((*findId)->m_xmf4x4ToParent._31,
				(*findId)->m_xmf4x4ToParent._32, (*findId)->m_xmf4x4ToParent._33));

			((CPlayer*)(*findId))->Move(playerInfo->keyState, 20.0f, fTimeElapsed, true);  //애니메이션처리
			((CPlayer*)(*findId))->SetCheckPoint(playerInfo->checkPoints);
		}
	}
}
void ItemGameScene::UpdateEventInfo(char* packet, float fTimeElapsed)
{
	SC_PACKET_EVENT* playerInfo = reinterpret_cast<SC_PACKET_EVENT*>(packet);
	MessageStruct msg = playerInfo->msg;

	EventHandler::GetInstance()->RegisterEvent(msg);
}

void ItemGameScene::UpdateInitInfo(char* packet, float fTimeElapsed)
{

}
void ItemGameScene::UpdateStartInfo(char* packet, float fTimeElapsed)
{
	isAllConnected = true;
}
void ItemGameScene::UpdateFinishInfo(char* packet, float fTimeElapsed)
{
	SoundManager::GetInstance()->AllStop();
	SC_PACKET_INGAME_FINISH_INFO* finishInfo = reinterpret_cast<SC_PACKET_INGAME_FINISH_INFO*>(packet);
	EventHandler::GetInstance()->m_iMinute = ((TimeCountShader*)TIME_COUNT_SHADER)->GetMinute();
	EventHandler::GetInstance()->m_fSecond = ((TimeCountShader*)TIME_COUNT_SHADER)->GetSecond();
	EventHandler::GetInstance()->m_sWinner = finishInfo->name;
	sceneType = End_Scene;  //멀티 플레이시 이 구간에서 서버로부터 골인한 플레이어를 확인후 씬 전환
}

void ItemGameScene::UpdatePlayerSliding(char* packet, float fTimeElapsed)
{
	SC_PACKET_PLAYER_ANI* playerInfo = reinterpret_cast<SC_PACKET_PLAYER_ANI*>(packet);

	vector<CGameObject*> obList = PLAYER_SHADER->getList();
	auto obj = find_if(obList.begin(), obList.end(), [&](CGameObject* a) {
		return a->GetId() == playerInfo->id; });
	if (obj != obList.end())
	{
		((CPlayer*)(*obj))->OnSliding();
	}
}
void ItemGameScene::UpdatePlayerCollision(char* packet, float fTimeElapsed)
{
	SC_PACKET_PLAYER_ANI* playerInfo = reinterpret_cast<SC_PACKET_PLAYER_ANI*>(packet);

	vector<CGameObject*> obList = PLAYER_SHADER->getList();
	auto obj = find_if(obList.begin(), obList.end(), [&](CGameObject* a) {
		return a->GetId() == playerInfo->id; });
	if (obj != obList.end())
	{
		((CPlayer*)(*obj))->OnCollisionAni();
	}
}