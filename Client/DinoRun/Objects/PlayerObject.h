#pragma once
#include "BaseObject.h"

class CCamera;
#define IDLE 0
#define IDLE_LEFT_TURN 1
#define IDLE_RIGHT_TURN 2
#define IDLE_LEFT_TURNING 3
#define IDLE_RIGHT_TURNING 4
#define IDLE_LEFT_RETURN 5
#define IDLE_RIGHT_RETURN 6

#define RUN 7
#define RUN_LEFT_TURN 8
#define RUN_RIGHT_TURN 9
#define RUN_LEFT_TURNING 10
#define RUN_RIGHT_TURNING 11
#define RUN_LEFT_RETURN 12
#define RUN_RIGHT_RETURN 13

#define ANIMATIONGAP RUN - IDLE

#define cPlayer 0.29f     //Cdrag
#define AIR 1.226f
//#define cRr cDrag*30   //Crr

#define Pi 3.1415f
#define WHEELROTATEPERSEC 30


class CPlayer : public CGameObject
{
protected:
	string						m_id = "Default";
	

	XMFLOAT3					m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3					m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	XMFLOAT3					m_xmf3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	float           			m_fPitch = 0.0f;
	float           			m_fYaw = 0.0f;
	float           			m_fRoll = 0.0f;

	XMFLOAT3     				m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float           			m_fMaxVelocityXZ = 0.0f;
	float           			m_fMaxVelocityY = 0.0f;
	float           			m_fFriction = 0.0f;

	LPVOID						m_pPlayerUpdatedContext = NULL;
	LPVOID						m_pCameraUpdatedContext = NULL;

	CCamera						*m_pCamera = NULL;

	//할 것임.
	UINT						m_uCheckpointCount = 0;
public:
	CPlayer();
	virtual ~CPlayer();
	string GetId() { return m_id; }
	XMFLOAT3 GetPosition() { return(m_xmf3Position); }
	XMFLOAT3 GetLookVector() { return(m_xmf3Look); }
	XMFLOAT3 GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3 GetRightVector() { return(m_xmf3Right); }

	UINT GetCheckPoint() { return m_uCheckpointCount; }
	void UpCheckPoint() { m_uCheckpointCount++; }

	void SetId(string id) { m_id = id; }
	void SetFriction(float fFriction) { m_fFriction = fFriction; }
	void SetGravity(const XMFLOAT3& xmf3Gravity) { m_xmf3Gravity = xmf3Gravity; }
	void SetMaxVelocityXZ(float fMaxVelocity) { m_fMaxVelocityXZ = fMaxVelocity; }
	void SetMaxVelocityY(float fMaxVelocity) { m_fMaxVelocityY = fMaxVelocity; }
	void SetVelocity(const XMFLOAT3& xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }
	void SetPosition(const XMFLOAT3& xmf3Position) { Move(XMFLOAT3(xmf3Position.x - m_xmf3Position.x, xmf3Position.y - m_xmf3Position.y, xmf3Position.z - m_xmf3Position.z), false); }
	
	void SetScale(const XMFLOAT3& xmf3Scale) { m_xmf3Scale = xmf3Scale; }

	float GetMaxVelocityXZ() { return m_fMaxVelocityXZ; }
	const XMFLOAT3& GetVelocity() const { return(m_xmf3Velocity); }
	float GetYaw() const { return(m_fYaw); }
	float GetPitch() const { return(m_fPitch); }
	float GetRoll() const { return(m_fRoll); }

	CCamera *GetCamera() { return(m_pCamera); }
	void SetCamera(CCamera *pCamera) { m_pCamera = pCamera; }

	void Move(DWORD nDirection, float fDistance, float fDeltaTime, bool bVelocity = false);
	virtual void Move(const XMFLOAT3& xmf3Shift, bool bVelocity = false);
	void Rotate(float x, float y, float z);

	void FixedUpdate(float fTimeElapsed); //물리
	bool Update(float fTimeElapsed, CGameObject* target);

	virtual void OnPlayerUpdateCallback(float fTimeElapsed) { }
	void SetPlayerUpdatedContext(LPVOID pContext) { m_pPlayerUpdatedContext = pContext; }

	virtual void OnCameraUpdateCallback(float fTimeElapsed) { }
	void SetCameraUpdatedContext(LPVOID pContext) { m_pCameraUpdatedContext = pContext; }

	virtual void CreateShaderVariables(CreateManager* pCreateManager);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);

	CCamera *OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode);

	virtual CCamera *ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed) { return NULL; }
	virtual void OnPrepareRender();

	virtual void Animate(float fTimeElapsed);

	virtual void KeyDownLeft() {};
	virtual void KeyDownRight() {};
	virtual void KeyDownUp() {};
	virtual void KeyDownDown() {};
	virtual void KeyUpLeft() {};
	virtual void KeyUpRight() {};
	virtual void KeyUpUp() {};
	virtual void KeyUpDown() {};
	virtual void returnIdle() {};

	void setShift(bool value) {isShift = value;}

	void SetWheelDegree(float fDegree) { m_fWheelDegree = fDegree; }
	float GetWheelDegree() { return m_fWheelDegree; }
protected:
	bool isWalking = false;
	bool isLeft = false;
	bool isDown = false;
	bool isUp = false;
	bool isRight = false;
	bool isShift = false;
	bool isStun = false;
	
	float m_fWheelDegree = 0.0f;
	float m_fTimeCount = 0.0f;
};


class CDinoRunPlayer : public CPlayer
{
public:
	CDinoRunPlayer(CreateManager* pCreateManager,string sModelName);
	virtual ~CDinoRunPlayer();
	
public:
	virtual void OnPrepareRender();
	virtual CCamera *ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);

	virtual void OnPlayerUpdateCallback(float fTimeElapsed);
	virtual void OnCameraUpdateCallback(float fTimeElapsed);
	
	virtual void KeyDownLeft();
	virtual void KeyDownRight();
	virtual void KeyDownUp();
	virtual void KeyDownDown();
	virtual void KeyUpLeft();
	virtual void KeyUpRight();
	virtual void KeyUpUp();
	virtual void KeyUpDown();
	virtual void returnIdle();
};

