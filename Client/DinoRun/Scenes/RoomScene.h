#pragma once

#include "BaseScene.h"

class CUiShader;

struct User
{
	string m_sName;
	float m_fButtonState;

	User(string name = "", float state = 0) :m_sName(name), m_fButtonState(state) {}
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

	virtual void AnimateObjects(float fTimeElapsed);  //�ִϸ��̼� ������Ʈ
	virtual SceneType Update(CreateManager* pCreateManager, float fTimeElapsed);  // �浹ó�� �� ����

	void Render(float fTimeElapsed);

	virtual void setPlayer(CPlayer* player);
	virtual void setCamera(CCamera* camera);

private:
	vector<CUiShader*> instacingUiShaders;
	bool isClickedButton = false;
	vector<User> m_vUsers;
};
