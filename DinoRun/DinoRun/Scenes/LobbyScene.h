#pragma once
#include "BaseScene.h"

class CObInstancingShader;
class CUiShader;
class CObjectsShader;

struct Room
{
	char m_iRoomNumber;
	char m_iUserNumber;
	bool m_bIsGaming;
	bool m_bMode;
	char m_iMaxUserNumber = 5;
	
	Room(char roomNum,char userNum, bool isGameing, bool mode):m_iRoomNumber(roomNum), 
		m_iUserNumber(userNum), m_bIsGaming(isGameing),m_bMode(mode)  //isGameing 0: �����, 1: ������
	{}
};

class LobbyScene : public BaseScene
{
public:
	LobbyScene();
	~LobbyScene();

	void ProcessInput(HWND hWnd, float deltaTime);
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam, float deltaTime);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam, float deltaTime);

	void BuildObjects(shared_ptr<CreateManager> pCreateManager);

	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();

	virtual void CreateShaderVariables(CreateManager* pCreateManager);
	virtual void UpdateShaderVariables();
	virtual void ReleaseShaderVariables();

	virtual void AnimateObjects(float fTimeElapsed);  //�ִϸ��̼� ������Ʈ
	virtual SceneType Update(CreateManager* pCreateManager, float fTimeElapsed);  // �浹ó�� �� ����

	void Render();

	virtual void setPlayer(CPlayer* player);
	virtual void setCamera(CCamera* camera);

	virtual void ProcessPacket(char* packet);
private:
	vector<CObInstancingShader*> instacingBillBoardShaders;
	vector<CUiShader*> instacingUiShaders;

	bool isClickedLeftRoom = false;
	bool isClickedRightRoom = false;
	bool isClickedLeftUser = false;
	bool isClickedRightUser = false;

	bool isClicked = false;
	float m_fClickedTime = 0.f;

	char m_iRoomPageNum = 0;
	char m_iUserPageNum = 0;
	bool m_bMode = 0.0f;
	int m_iResultNum = 0; 
	vector<Room> m_vRooms;
	vector<string> m_vUsers;
};
