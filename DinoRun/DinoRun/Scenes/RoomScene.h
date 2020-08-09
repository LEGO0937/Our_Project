#pragma once

#include "BaseScene.h"

class CUiShader;

struct User
{
	string m_sName;
	bool m_bButtonState;
	char m_id;

	User(string name = "", bool state = 0,char id = 0) :m_sName(name), m_bButtonState(state),m_id(id) {}
};
class RoomScene : public BaseScene
{
public:
	RoomScene();
	~RoomScene();

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

	virtual void AnimateObjects(float fTimeElapsed);  //애니메이션 업데이트
	virtual SceneType Update(CreateManager* pCreateManager, float fTimeElapsed);  // 충돌처리 및 물리

	void Render();

	virtual void setPlayer(CPlayer* player);
	virtual void setCamera(CCamera* camera);

	virtual void ProcessPacket(char* packet, float fTimeElapsed);
	void UpdateUnreadyState(char* packet, float fTimeElapsed);
	void UpdateReadyState(char* packet, float fTimeElapsed);
	void UpdateAddUser(char* packet, float fTimeElapsed);
	void UpdateDeleteUser(char* packet, float fTimeElapsed);
	void UpdateLogOut(char* packet, float fTimeElapsed);
	void UpdateNextScene(char* packet, float fTimeElapsed);
private:
	vector<CUiShader*> instacingUiShaders;
	bool isClickedButton = false;
	vector<User> m_vUsers;
};
