#include "LobbyScene.h"
#include "../Common/FrameWork/CreateManager.h"
#include "../Common/FrameWork/NetWorkManager.h"
#include "../Common/FrameWork/SoundManager.h"

#include "../Objects/PlayerObject.h"

#include "../CShaders/BillBoardShader/BillBoardShader.h"
#include "../CShaders/UiShader/UiShader.h"

#include "../Common/Camera/Camera.h"

#define IS_GAMING_IMAGE 1
#define MODE_IMAGE 2
#define ARROW_BUTTON 3

#define ROOM_LEFT 0
#define ROOM_RIGHT 1
#define USER_LEFT 2
#define USER_RIGHT 3

LobbyScene::LobbyScene() :BaseScene()
{
	sceneType = SceneType::Lobby_Scene;
}
LobbyScene::~LobbyScene()
{
	//*서버*
	//로비씬을 나가는 부분 다시 로그인 창으로 돌아가기때문에
	//이 곳에서 서버에게 자신의 닉네임을 알려주고 나감.
	//서버는 닉네임을 받고 접속중인 유저 리스트에서 이 닉네임을 제거한다.
	m_pSoundManager->Stop("Start_BGM");
}
void LobbyScene::ReleaseUploadBuffers()
{
	BaseScene::ReleaseUploadBuffers();
	
	for (CObInstancingShader* shader : instacingBillBoardShaders)
		if (shader) shader->ReleaseUploadBuffers();
	
	for (CUiShader* shader : instacingUiShaders)
		if (shader) { shader->ReleaseUploadBuffers(); }
}
void LobbyScene::ReleaseObjects()
{
	BaseScene::ReleaseObjects();
	
	m_vRooms.clear();
	m_vUsers.clear();

	for (CObInstancingShader* shader : instacingBillBoardShaders)
		if (shader) { shader->ReleaseShaderVariables(); shader->ReleaseObjects();  shader->Release(); }
	
	for (CUiShader* shader : instacingUiShaders)
		if (shader) { shader->ReleaseShaderVariables(); shader->ReleaseObjects();  shader->Release(); }
	
	instacingBillBoardShaders.clear();

}
void LobbyScene::BuildObjects(shared_ptr<CreateManager> pCreateManager)
{
	m_pCreateManager = pCreateManager;
	m_pNetWorkManager = pCreateManager->GetNetWorkMgr();
	m_pSoundManager = pCreateManager->GetSoundMgr();

	m_pd3dCommandList = pCreateManager->GetCommandList().Get();

	m_pSoundManager->Play("Start_BGM", 0.2f);
	CUiShader* uiShader;

	uiShader = new BackGroundShader;
	string name = "Resources/Images/T_LobbyBackground.dds";
	uiShader->BuildObjects(pCreateManager.get(), &name);
	instacingUiShaders.emplace_back(uiShader);
	/*
	m_vRooms 초기화
	m_vUsers 초기화
	*/
	//*서버*
	//로비씬에 들어오기를 성공하면 서버에게 자신의 닉네임을 전송해줌. 접속중 유저 리스트에 자신을 추가하기 위해서임
	//로비씬에 들어오면 서버로부터 방 정보와 유저 이름 정보를 받아서
	//vector형식의 방,유저목록에 저장함, 방정보는 Room클래스, 유저 이름 정보 string 사용
	//방 정보에는 방 번호, 현재 인원수, 게임중 or 대기중 상태의 변수를 가짐

	//==================================임시로 작성한 부분
	m_vRooms.emplace_back(Room(1, 5, false, true));
	m_vRooms.emplace_back(Room(2, 2, false,false));
	m_vRooms.emplace_back(Room(3, 1, true,false));
	m_vRooms.emplace_back(Room(4, 0, true,true));  //0.5 = true*0.5f
	m_vRooms.emplace_back(Room(5, 3, false,true));
	m_vRooms.emplace_back(Room(6, 2, false,false));
	m_vRooms.emplace_back(Room(7, 0, true, false));
	m_vRooms.emplace_back(Room(8, 3, false,false));
	m_vRooms.emplace_back(Room(9, 2, false,false));


	m_vUsers.emplace_back("das");
	m_vUsers.emplace_back("das1");
	m_vUsers.emplace_back("das2");
	m_vUsers.emplace_back("das3");
	m_vUsers.emplace_back("das4");
	m_vUsers.emplace_back("das5");
	m_vUsers.emplace_back("das6");
	m_vUsers.emplace_back("das7");
	m_vUsers.emplace_back("das8");
	m_vUsers.emplace_back("das9");
	m_vUsers.emplace_back("das10");
	m_vUsers.emplace_back("das11");
	m_vUsers.emplace_back("das12");
	m_vUsers.emplace_back("das13");
	m_vUsers.emplace_back("das14");
	m_vUsers.emplace_back("das15");
	m_vUsers.emplace_back("das16");
	m_vUsers.emplace_back("das17");
	m_vUsers.emplace_back("das18");
	m_vUsers.emplace_back("das19");
	m_vUsers.emplace_back("das20");
	m_vUsers.emplace_back("das21");
	//------------------UserList----------------------
		
	gameTexts.emplace_back(GameText(XMFLOAT2(0.71f, 0.28f),XMFLOAT2(1.05f,1.05f)));  //유저 목록 8줄 0~7 idx
	gameTexts.emplace_back(GameText(XMFLOAT2(0.71f, 0.35f),XMFLOAT2(1.05f,1.05f)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.71f, 0.42f),XMFLOAT2(1.05f,1.05f)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.71f, 0.49f),XMFLOAT2(1.05f,1.05f)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.71f, 0.56f),XMFLOAT2(1.05f,1.05f)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.71f, 0.63f),XMFLOAT2(1.05f,1.05f)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.71f, 0.70f),XMFLOAT2(1.05f,1.05f)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.71f, 0.77f),XMFLOAT2(1.05f,1.05f)));
	//------------------RoomList----------------------
	gameTexts.emplace_back(GameText(XMFLOAT2(0.09f, 0.24f), XMFLOAT2(0.8f, 0.8f)));  //방 인원 수 8~11 idx
	gameTexts.emplace_back(GameText(XMFLOAT2(0.09f, 0.43f), XMFLOAT2(0.8f, 0.8f)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.40f, 0.24f), XMFLOAT2(0.8f, 0.8f)));
	gameTexts.emplace_back(GameText(XMFLOAT2(0.40f, 0.43f), XMFLOAT2(0.8f, 0.8f)));

	//------------------isGmaing? Image----  1'st
	UI_INFO view_info;    
	view_info.textureName = "Resources/Images/T_Button.dds";
	view_info.meshSize = XMFLOAT2(0.087f, 0.05f);
	view_info.positions.emplace_back(XMFLOAT3(-0.45f, 0.475f, 0.0f));
	view_info.f_uvY.emplace_back(0.5f);

	view_info.positions.emplace_back(XMFLOAT3(-0.45f, 0.087f, 0.0f));
	view_info.f_uvY.emplace_back(0.5f);

	view_info.positions.emplace_back(XMFLOAT3(0.175f, 0.475f, 0.0f));
	view_info.f_uvY.emplace_back(0.5f);

	view_info.positions.emplace_back(XMFLOAT3(0.175f, 0.087f, 0.0f));
	view_info.f_uvY.emplace_back(0.5f);
	view_info.maxUv = XMFLOAT2(0.5f, 0.25f);
	view_info.minUv = XMFLOAT2(0.0f, 0.0f);
	uiShader = new ImageShader;
	uiShader->BuildObjects(pCreateManager.get(), &view_info);
	instacingUiShaders.emplace_back(uiShader);

	//------------ Item or Speed Mode Image------------  2'st
	view_info.positions.clear();
	view_info.f_uvY.clear();

	view_info.positions.emplace_back(XMFLOAT3(-0.45f, 0.275f, 0.0f));
	view_info.f_uvY.emplace_back(0.75f);

	view_info.positions.emplace_back(XMFLOAT3(-0.45f, -0.113f, 0.0f));
	view_info.f_uvY.emplace_back(0.75f);

	view_info.positions.emplace_back(XMFLOAT3(0.175f, 0.275f, 0.0f));
	view_info.f_uvY.emplace_back(0.75f);

	view_info.positions.emplace_back(XMFLOAT3(0.175f, -0.113f, 0.0f));
	view_info.f_uvY.emplace_back(0.75f);
	uiShader = new ImageShader;
	uiShader->BuildObjects(pCreateManager.get(), &view_info);
	instacingUiShaders.emplace_back(uiShader);

	//----------------ArrowButton Image-  3'st
	view_info.textureName = "Resources/Images/T_ArrowButton.dds";  
	view_info.meshSize = XMFLOAT2(0.08f, 0.06f);
	view_info.positions.clear();
	view_info.f_uvY.clear();
	view_info.positions.emplace_back(XMFLOAT3(-0.40f, -0.3f, 0.0f));
	view_info.f_uvY.emplace_back(0.25f);
	view_info.positions.emplace_back(XMFLOAT3(-0.18f, -0.3f, 0.0f));
	view_info.f_uvY.emplace_back(0.0f);

	view_info.positions.emplace_back(XMFLOAT3(0.55f, -0.75f, 0.0f));
	view_info.f_uvY.emplace_back(0.25f);
	view_info.positions.emplace_back(XMFLOAT3(0.77f, -0.75f, 0.0f));
	view_info.f_uvY.emplace_back(0.0f);

	uiShader = new ImageShader;
	uiShader->BuildObjects(pCreateManager.get(), &view_info);
	instacingUiShaders.emplace_back(uiShader);


	//fontShader = new FontShader;
	//fontShader->BuildObjects(pCreateManager, NULL);

	CreateShaderVariables(pCreateManager.get());
}

void LobbyScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
	lParam, float deltaTime)
{
	Point2D point;
	::SetCapture(hWnd);
	::GetCursorPos(&m_ptOldCursorPos);
	ScreenToClient(hWnd, &m_ptOldCursorPos);
	point = ScreenToProj(m_nWndClientWidth, m_nWndClientHeight, m_ptOldCursorPos);
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		m_pSoundManager->Play("Mouse_Down", 0.2f);
		if (point.x > -0.48f && point.x < -0.32f && point.y > -0.36f && point.y < -0.24f) //방목록 왼쪽화살표 충돌체크
		{
			instacingUiShaders[ARROW_BUTTON]->getUvXs()[ROOM_LEFT] = 0.5;
			isClickedLeftRoom = true;
		}
		else if (point.x > -0.26f && point.x < -0.1f && point.y > -0.36f && point.y < -0.24f) //방목록 오른쪽 화살표 충돌체크
		{
			instacingUiShaders[ARROW_BUTTON]->getUvXs()[ROOM_RIGHT] = 0.5f;
			isClickedRightRoom = true;
		}
		else if (point.x > 0.47f && point.x < 0.63f && point.y > -0.81f && point.y < -0.69f) //유저목록 왼쪽화살표 충돌체크
		{
			instacingUiShaders[ARROW_BUTTON]->getUvXs()[USER_LEFT] = 0.5f;
			isClickedLeftUser = true;
		}
		else if (point.x > 0.69f && point.x < 0.85f && point.y > -0.81f && point.y < -0.69f) //유저목록 오른쪽화살표 충돌체크
		{
			instacingUiShaders[ARROW_BUTTON]->getUvXs()[USER_RIGHT] = 0.5f;
			isClickedRightUser = true;
		}
		else
		{
			if (!isClicked)
			{
				isClicked = true;
				m_fClickedTime = 0;
			}
			else
			{
				int clickNum = m_iRoomPageNum * 4;
				//더블 클릭 구간 
				//입장 시 m_vRooms의 m_iRoomPageNum * 4 + a 째방 번호를 네트워크 구조체에 값을 넣어주도록한다.
				// 다음 룸씬에서 동일한 방의 유저들과 정보를 나눠주기 위해.
				if (m_vRooms.size())
				{
					if (point.x > -0.87f && point.x < -0.34f && point.y > 0.22f && point.y < 0.51f) //1번 방 충돌체크
					{
						if (clickNum <= m_vRooms.size() - 1)
						{
							//*서버*
							//방 클릭 시 해당 방의 번호를 서버에게 보냄 서버는 방에 여유인원이 있고, 대기중 상태라면
							//들어오라는 신호를 보낸다.
							//연결 성공 시 네트워크 클래스에 m_vRooms[clickNum].m_iRoomNumber를 받아서 
							//접속할 방의 번호 저장할것.
							if (!m_vRooms[clickNum].m_bIsGaming && m_vRooms[clickNum].m_iUserNumber < m_vRooms[clickNum].m_iMaxUserNumber)
							{
								m_iResultNum = m_vRooms[clickNum].m_iRoomNumber;
								m_bMode = m_vRooms[clickNum].m_bMode;
								sceneType = SceneType::Room_Scene;
							}
						}
					}
					else if (point.x > -0.87f && point.x < -0.34f && point.y > -0.17f && point.y < 0.14f) //2번 방 충돌체크
					{
						if (clickNum + 1 <= m_vRooms.size() - 1)
						{
							if (!m_vRooms[clickNum + 1].m_bIsGaming && m_vRooms[clickNum + 1].m_iUserNumber < m_vRooms[clickNum + 1].m_iMaxUserNumber)
							{
								m_iResultNum = m_vRooms[clickNum + 1].m_iRoomNumber;
								m_bMode = m_vRooms[clickNum + 1].m_bMode;
								sceneType = SceneType::Room_Scene;
							}
						}
					}
					else if (point.x > -0.24f && point.x < 0.28f && point.y > 0.22f && point.y < 0.51f) //3번 방 충돌체크
					{
						if (clickNum + 2 <= m_vRooms.size() - 1)
						{
							if (!m_vRooms[clickNum + 2].m_bIsGaming && m_vRooms[clickNum + 2].m_iUserNumber < m_vRooms[clickNum + 2].m_iMaxUserNumber)
							{
								m_iResultNum = m_vRooms[clickNum + 2].m_iRoomNumber;
								m_bMode = m_vRooms[clickNum + 2].m_bMode;
								sceneType = SceneType::Room_Scene;
							}
						}
					}
					else if (point.x > -0.24f && point.x < 0.28f && point.y > -0.17f && point.y < 0.14f) //4번 방 충돌체크
					{
						if (clickNum + 3 <= m_vRooms.size() - 1)
						{
							if (!m_vRooms[clickNum + 3].m_bIsGaming && m_vRooms[clickNum + 3].m_iUserNumber < m_vRooms[clickNum + 3].m_iMaxUserNumber)
							{
								m_iResultNum = m_vRooms[clickNum + 3].m_iRoomNumber;
								m_bMode = m_vRooms[clickNum + 3].m_bMode;
								sceneType = SceneType::Room_Scene;
							}
						}
					}
				}
			}
		}
		break;
	case WM_RBUTTONDOWN:
		//마우스 캡쳐를 하고 현재 마우스 위치를 가져온다. 
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
		m_pSoundManager->Play("Mouse_Up", 0.2f);
		if (point.x > -0.48f && point.x < -0.32f && point.y > -0.36f && point.y < -0.24f) //방목록 왼쪽화살표 충돌체크
		{
			if (isClickedLeftRoom)
			{
				//방목록 변경
				m_iRoomPageNum -= 1;
				if (m_iRoomPageNum < 0)
					m_iRoomPageNum = 0;
			}
					
		}
		else if (point.x > -0.26f && point.x < -0.1f && point.y > -0.36f && point.y < -0.24f) //방목록 오른쪽 화살표 충돌체크
		{
			if (isClickedRightRoom)
			{
				//방목록 변경
				m_iRoomPageNum += 1;
				if (m_iRoomPageNum > ((m_vRooms.size()*0.25)))
					m_iRoomPageNum = ((m_vRooms.size()*0.25));

			}
		}
		else if (point.x > 0.47f && point.x < 0.63f && point.y > -0.81f && point.y < -0.69f) //유저목록 왼쪽화살표 충돌체크
		{
			if (isClickedLeftUser)
			{
				//유저목록 변경
				m_iUserPageNum -= 1;
				if (m_iUserPageNum < 0)
					m_iUserPageNum = 0;
			}
		}
		else if (point.x > 0.69f && point.x < 0.85f && point.y > -0.81f && point.y < -0.69f) //유저목록 오른쪽화살표 충돌체크
		{
			if (isClickedRightUser)
			{
				//유저목록 변경
				m_iUserPageNum += 1;
				if (m_iUserPageNum > ((m_vUsers.size()*0.125)))
					m_iUserPageNum = ((m_vUsers.size()*0.125));

			}
		}
		else
		{

		}
		isClickedLeftRoom = false;
		isClickedRightRoom = false;

		isClickedLeftUser = false;
		isClickedRightUser = false;
		for(int i = 0; i<4; ++i)
			instacingUiShaders[ARROW_BUTTON]->getUvXs()[i] = 0;
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
void LobbyScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
	lParam, float deltaTime)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			sceneType = SceneType::Start_Scene;
			break;
		default:
			break;
		}
		break;
	case WM_KEYDOWN:
		break;
	default:
		break;
	}
}
void LobbyScene::ProcessInput(HWND hwnd, float deltaTime)
{
}

void LobbyScene::Render()
{
	BaseScene::Render();

	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_UI]);
	for (CUiShader* shader : instacingUiShaders)
	{
		if(shader)
			shader->Render(m_pd3dCommandList, m_pCamera);
	}
	
	m_pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[PSO_PONT]);
	if (fontShader)
		fontShader->Render(m_pd3dCommandList, m_pCamera, gameTexts);

#ifdef _WITH_BOUND_BOX

#endif
}


void LobbyScene::AnimateObjects(float fTimeElapsed)
{
	//말 그대로 애니메이션 update

}

SceneType LobbyScene::Update(CreateManager* pCreateManager, float fTimeElapsed)
{
	if (sceneType != SceneType::Lobby_Scene)
	{
		//네트워크 클래스에 방의 모드 저장할 것
		m_pNetWorkManager->SetRoomNum(m_iResultNum);
		m_pNetWorkManager->SetGameMode(m_bMode);
		return sceneType;
	}

	//----------------- 매번 서버로부터 방들의 정보를 갱신 
	//m_iPageNum 
	//m_vRooms.clear();
	/*
	for () 5~6초 지날때마다 갱신
	{
		m_vRooms.emplace_back(Room(1, 1, 0));

	}
	*/
	if (m_vUsers.size())
	{
		for (int i = m_iUserPageNum * 8, n = 0; i < (m_iUserPageNum * 8) + 8; ++i, ++n)
		{
			if (i <= m_vUsers.size() - 1)
			{
				gameTexts[n].text = m_vUsers[i];
			}
			else
			{
				gameTexts[n].text = "";
			}
		}
	}
	if (m_vRooms.size())
	{
		for (int i = m_iRoomPageNum * 4, n = 0; i < (m_iRoomPageNum * 4) + 4; ++i, ++n)
		{
			if (i <= m_vRooms.size() - 1)
			{
				gameTexts[n + 8].text = to_string(m_vRooms[i].m_iUserNumber) + " / " + to_string(m_vRooms[i].m_iMaxUserNumber);
				instacingUiShaders[IS_GAMING_IMAGE]->getUvXs()[n] = 0.5f * m_vRooms[i].m_bIsGaming;
				instacingUiShaders[MODE_IMAGE]->getUvXs()[n] = 0.5f * m_vRooms[i].m_bMode;
			}
			else
			{
				gameTexts[n + 8].text = "None";
				instacingUiShaders[IS_GAMING_IMAGE]->getUvXs()[n] = 0;
				instacingUiShaders[MODE_IMAGE]->getUvXs()[n] = 0.0f;
			}
		}
	}
	
	//물리 및 충돌을 위한 update
	if (isClicked)
		m_fClickedTime += fTimeElapsed;
	if (m_fClickedTime > 0.3f)
	{
		isClicked = false;
		m_fClickedTime = 0.f;
	}
	for (CUiShader* shader : instacingUiShaders)
		shader->Update(fTimeElapsed, NULL);
	return Lobby_Scene;
}


void LobbyScene::CreateShaderVariables(CreateManager* pCreateManager)
{
}


void LobbyScene::UpdateShaderVariables()
{
}

void LobbyScene::ReleaseShaderVariables()
{

}


void LobbyScene::setPlayer(CPlayer* player)
{
	BaseScene::setPlayer(player);
}

void LobbyScene::setCamera(CCamera* camera)
{
	BaseScene::setCamera(camera);
}