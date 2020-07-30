//-----------------------------------------------------------------------------
// File: CPlayer.cpp
//-----------------------------------------------------------------------------

#include "PlayerObject.h"
#include "TerrainObject.h"
#include "../Common/FrameWork/CreateManager.h"
#include "../CShaders/Shader.h"
#include "../Common/Camera/Camera.h"

#include "../Common/ParticleSystem/ParticleSystem.h"
#include "../../DinoRun/Common/Animation/Animation.h"

#include "EventHandler/EventHandler.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CPlayer


CPlayer::CPlayer() : CGameObject()
{
	m_pCamera = NULL;

	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fMaxVelocityXZ = 0.0f;
	m_fMaxVelocityY = 0.0f;
	m_fFriction = 0.0f;

	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;

	m_pUpdatedContext = NULL;
	m_pCameraUpdatedContext = NULL;
}

CPlayer::~CPlayer()
{
	ReleaseShaderVariables();

	if (m_pCamera) delete m_pCamera;
}

void CPlayer::CreateShaderVariables(CreateManager* pCreateManager)
{
	if (m_pCamera) m_pCamera->CreateShaderVariables(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get());
}

void CPlayer::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
}

void CPlayer::ReleaseShaderVariables()
{
	if (m_pCamera) m_pCamera->ReleaseShaderVariables();
}

void CPlayer::Move(DWORD dwDirection, float fDistance, float fDeltaTime, bool bUpdateVelocity)
{
	if (isStun || m_xmf3Velocity.y != 0)
		return;

	//if (dwDirection)
	DWORD nCurrentCameraMode = m_pCamera->GetMode();
	if (nCurrentCameraMode == THIRD_PERSON_CAMERA)
	{
		XMFLOAT3 vel = GetVelocity();
		float length = (vel.x * vel.x + vel.z * vel.z);

		m_xmf3Forces = XMFLOAT3(0, 0, 0);
#ifdef _WITH_LEFT_HAND_COORDINATES
		if (dwDirection & DIR_FORWARD)
		{
			KeyDownUp(); 
			if((dwDirection & DIR_RIGHT) || (dwDirection & DIR_LEFT))
				m_fForce += fDistance * 0.5f;
			else
				m_fForce += fDistance;
			if (!SoundManager::GetInstance()->Playing("Running"))
				SoundManager::GetInstance()->Play("Running");
		}
		else
		{
			if (m_fForce > 0)
				m_fForce = 0;
			if (isUp)
				KeyUpUp();
		}

		if (dwDirection & DIR_BACKWARD)
		{
			KeyDownDown();
			m_fForce -= fDistance * 0.5f;
			if (!SoundManager::GetInstance()->Playing("Running"))
				SoundManager::GetInstance()->Play("Running");
		}
		else
		{
			if (m_fForce < 0)
				m_fForce = 0;
			if (isDown)
				KeyUpDown();
		}
#else
		if (dwDirection & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -fDistance);
		if (dwDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, +fDistance);
#endif
#ifdef _WITH_LEFT_HAND_COORDINATES
		if (dwDirection & DIR_RIGHT)
		{
			KeyDownRight();
			/*
			if (!isShift)
			{
				if (length < 30)
					m_fWheelDegree += 15 * fDeltaTime;
				else
					m_fWheelDegree += 10 * fDeltaTime;
			}
			else
				m_fWheelDegree += 30 * fDeltaTime;
				*/
		}
		else
		{
			if (isRight)
				KeyUpRight();
		}
		//	xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, +fDistance);
		if (dwDirection & DIR_LEFT)
		{
			KeyDownLeft();
			/*
			if (!isShift)
			{
				if (length < 30)
					m_fWheelDegree -= 15 * fDeltaTime;
				else
					m_fWheelDegree -= 10 * fDeltaTime;
			}
			else
				m_fWheelDegree -= 30 * fDeltaTime;
				*/
		}
		else
		{
			if (isLeft)
				KeyUpLeft();
		}
			
		//	xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -fDistance);
#else
		if (dwDirection & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -fDistance);
		if (dwDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, +fDistance);
#endif
		//if (dwDirection & DIR_UP) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, +fDistance);
		//if (dwDirection & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, -fDistance);

		//Move(xmf3Shift, bUpdateVelocity);
		/*
		if (!isShift)
		{
			if (length > 30)
			{
				if (m_fWheelDegree > 10)
					m_fWheelDegree = 10;
				else if (m_fWheelDegree < -10)
					m_fWheelDegree = -10;
			}
			else
			{
				if (m_fWheelDegree > 10)
					m_fWheelDegree = 10;
				else if (m_fWheelDegree < -10)
					m_fWheelDegree = -10;
			}
		}
		else
		{
			if (m_fWheelDegree > 20)
				m_fWheelDegree = 20;
			else if (m_fWheelDegree < -20)
				m_fWheelDegree = -20;
		}
		*/
	}
	else
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		if (dwDirection & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, fDistance);
		if (dwDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -fDistance);
		if (dwDirection & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, fDistance);
		if (dwDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -fDistance);
		if (dwDirection & DIR_UP) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, fDistance);
		if (dwDirection & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, -fDistance);

		Move(xmf3Shift, false);
	}
	if (m_fForce > m_fMaxForce)
		m_fForce = m_fMaxForce;
	if (m_fForce < -m_fMaxForce)
		m_fForce = -m_fMaxForce;


}

void CPlayer::Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity)
{
	if (bUpdateVelocity)
	{
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	}
	else
	{
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
		m_pCamera->Move(xmf3Shift);
		m_pCamera->RegenerateViewMatrix();
	}
}

void CPlayer::Rotate(float x, float y, float z)
{
#ifndef _WITH_LEFT_HAND_COORDINATES
	x = -x; y = -y; z = -z;
#endif
	DWORD nCurrentCameraMode = m_pCamera->GetMode();
	if ((nCurrentCameraMode == FIRST_PERSON_CAMERA) || (nCurrentCameraMode == THIRD_PERSON_CAMERA))
	{
		if (x != 0.0f)
		{
			m_fPitch += x;
			if (m_fPitch > +89.0f) { x -= (m_fPitch - 89.0f); m_fPitch = +89.0f; }
			if (m_fPitch < -89.0f) { x -= (m_fPitch + 89.0f); m_fPitch = -89.0f; }
		}
		if (y != 0.0f)
		{
			m_fYaw += y;
			if (m_fYaw > 360.0f) m_fYaw -= 360.0f;
			if (m_fYaw < 0.0f) m_fYaw += 360.0f;
		}
		if (z != 0.0f)
		{
			m_fRoll += z;
			if (m_fRoll > +20.0f) { z -= (m_fRoll - 20.0f); m_fRoll = +20.0f; }
			if (m_fRoll < -20.0f) { z -= (m_fRoll + 20.0f); m_fRoll = -20.0f; }
		}
		m_pCamera->Rotate(x, y, z);
		if (y != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);

			if (!isShift)
			{
				//if (Vector3::Length(m_xmf3Velocity) < 35)
				m_xmf3Velocity = Vector3::TransformCoord(m_xmf3Velocity, xmmtxRotate);
			}
			else
			{
				if (!SoundManager::GetInstance()->Playing("Curve"))
				{
					if (SoundManager::GetInstance()->Playing("Running"))
						SoundManager::GetInstance()->Stop("Running");
					SoundManager::GetInstance()->Play("Curve");
				}
				//m_xmf3Velocity = Vector3::TransformCoord(m_xmf3Velocity, xmmtxRotate);
			}
		}
	}
	else if (nCurrentCameraMode == SPACESHIP_CAMERA)
	{
		m_pCamera->Rotate(x, y, z);
		if (x != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(x));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		}
		if (y != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
		if (z != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), XMConvertToRadians(z));
			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}

	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
}


bool CPlayer::Update(float fTimeElapsed, CGameObject* target)
{
	//�浹 ó��
	float fVx, fVy, fVz;
	MessageStruct message;

	if (target->m_fMass)
	{
		if (Vector3::Length(m_xmf3Velocity) > 20)
		{
			message.shaderName = HEAT_EFFECT;
			message.departMat = m_xmf4x4World;
			message.msgName = _ADD_PARTICLE;
#ifndef isConnectedToServer
			EventHandler::GetInstance()->RegisterEvent(message);
#else
			NetWorkManager::GetInstance()->SendEventPacket(message);  //�������� ���϶� register��� �̰� ���� ��.
#endif
			OnCollisionAni();
		}

		fVx = ((m_fMass - target->m_fMass) / (m_fMass + target->m_fMass))* m_xmf3Velocity.x
			+ ((2.f*target->m_fMass) / (m_fMass + target->m_fMass))*target->m_xmf3Velocity.x;

		fVy = ((m_fMass - target->m_fMass) / (m_fMass + target->m_fMass))* m_xmf3Velocity.y
			+ ((2.f*target->m_fMass) / (m_fMass + target->m_fMass))*target->m_xmf3Velocity.y;

		fVz = ((m_fMass - target->m_fMass) / (m_fMass + target->m_fMass))* m_xmf3Velocity.z
			+ ((2.f*target->m_fMass) / (m_fMass + target->m_fMass))*target->m_xmf3Velocity.z;
		SetVelocity(XMFLOAT3(fVx, fVy, fVz));
		SetPosition(XMFLOAT3(m_xmf4x4PrevWorld._41, m_xmf4x4PrevWorld._42, m_xmf4x4PrevWorld._43));

		UpdateDistance(fTimeElapsed, target);
		m_fForce = 0;

		
	}
	float fLength;
	switch (target->GetModelType())   //�浹ó���� ������ ���𰡸� ���� or �����Ϸ��� return true �� ��
	{
	case ModelType::CheckPoint:
		++m_uCheckpointCount;
		break;
	case ModelType::Item_Stone:
	case ModelType::Fence:
	case ModelType::Player:
		SoundManager::GetInstance()->Play("Heat", 0.2f);
		return true;
	case ModelType::Item_Box:
		//������ ����
		message.objectSerialNum = target->GetId();
		message.shaderName = _ITEM_SHADER;
		message.msgName = _DISENABLE_OBJECT;
#ifndef isConnectedToServer
		EventHandler::GetInstance()->RegisterEvent(message);
#else
		NetWorkManager::GetInstance()->SendEventPacket(message);  //�������� ���϶� register��� �̰� ���� ��.
#endif
		message.shaderName = BOX_PARTICLE;
		message.departMat = target->m_xmf4x4World;
		message.msgName = _ADD_PARTICLE;
#ifndef isConnectedToServer
		EventHandler::GetInstance()->RegisterEvent(message);
#else
		NetWorkManager::GetInstance()->SendEventPacket(message);  //�������� ���϶� register��� �̰� ���� ��.
#endif
		SoundManager::GetInstance()->Play("ItemBox", 0.2f);
		return true;
	case ModelType::Item_Meat:
		m_fMaxVelocityXZ += 15;
		if (m_fMaxVelocityXZ > MAX_VELOCITY)
			m_fMaxVelocityXZ = MAX_VELOCITY;

		message.objectSerialNum = target->GetId();
		message.shaderName = _MEAT_SHADER;
		message.msgName = _DISENABLE_OBJECT;
#ifndef isConnectedToServer
		EventHandler::GetInstance()->RegisterEvent(message);
#else
		NetWorkManager::GetInstance()->SendEventPacket(message);  //�������� ���϶� register��� �̰� ���� ��.
#endif

		message.shaderName = MEAT_PARTICLE;
		message.departMat = target->m_xmf4x4World;
		message.msgName = _ADD_PARTICLE;
#ifndef isConnectedToServer
		EventHandler::GetInstance()->RegisterEvent(message);
#else
		NetWorkManager::GetInstance()->SendEventPacket(message);  //�������� ���϶� register��� �̰� ���� ��.
#endif
		SoundManager::GetInstance()->Play("MeatEat", 0.5f);
		//target->SetEnableState(false);  //���� ��Ȱ��ȭ ��ȣ ������ �����ְ� ���̴����� ó���� ��.
		return true;
	case ModelType::Item_Banana:
		//if (m_fWheelDegree > 0)
		//	m_fWheelDegree = 30;
		//else if (m_fWheelDegree < 0)
		//	m_fWheelDegree = -30;
		OnSliding();
		message.objectSerialNum = target->GetId();
		message.shaderName = _BANANA_SHADER;
		message.msgName = _DELETE_OBJECT;
#ifndef isConnectedToServer
		EventHandler::GetInstance()->RegisterEvent(message);
#else
		NetWorkManager::GetInstance()->SendEventPacket(message);  //�������� ���϶� register��� �̰� ���� ��.
#endif
		break;		
	case ModelType::Item_Mud:
		fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
		if (fLength > 4)  //�ӵ� ũ�Ⱑ xz�ִ뷮�� ������ �ִ뷮���� ���ϰ� ��. �ӵ������� ������ m/s
		{
			m_xmf3Velocity.x *= (4 / fLength);
			m_xmf3Velocity.z *= (4 / fLength);
		}
		break;
	case ModelType::Item_Mound:
		if (!target->GetKinematicState())
		{
			XMFLOAT3 dir = Vector3::Subtract(m_xmf3Position, target->GetPosition());
			dir = Vector3::Normalize(dir);

			SetVelocity(XMFLOAT3(20*dir.x, 17 , 20*dir.z));

			OnCollisionAni();
		}
		break;
	default:
		break;
	}
	return false;
}
void CPlayer::ProcessRotate(float fTimeElapsed)
{
	if (!isStun)
	{

		XMFLOAT3 vel = GetVelocity();
		float length = (vel.x * vel.x + vel.z * vel.z);

		UINT curTrack = m_pSkinnedAnimationController->m_CurrentTrack;
		if (curTrack >= RUN)
			curTrack -= ANIMATIONGAP;
		switch (curTrack)
		{
		case IDLE:
			m_fWheelDegree = 0;
			break;
		case IDLE_LEFT_TURN:
		case IDLE_RIGHT_RETURN:
		case IDLE_LEFT_TURNING:
			if (!isShift)
			{
				m_fWheelDegree -= 20 * fTimeElapsed;
			}
			else
				m_fWheelDegree -= 30 * fTimeElapsed;
			break;
		case IDLE_RIGHT_TURN:
		case IDLE_LEFT_RETURN:
		case IDLE_RIGHT_TURNING:
			if (!isShift)
			{
				m_fWheelDegree += 20 * fTimeElapsed;
			}
			else
				m_fWheelDegree += 30 * fTimeElapsed;
			break;
		}

		if (!isShift)
		{
			//if (length > 20)
			{
				if (m_fWheelDegree > 7)
					m_fWheelDegree = 7;
				else if (m_fWheelDegree < -7)
					m_fWheelDegree = -7;
			}
			//else
			//{
			//	if (m_fWheelDegree > 15)
			//		m_fWheelDegree = 15;
			//	else if (m_fWheelDegree < -15)
			//		m_fWheelDegree = -15;
			//}
		}
		else
		{
			if (m_fWheelDegree > 20)
				m_fWheelDegree = 20;
			else if (m_fWheelDegree < -20)
				m_fWheelDegree = -20;
		}
	}
}
void CPlayer::FixedUpdate(float fTimeElapsed)
{
	DWORD nCurrentCameraMode = m_pCamera->GetMode();
	if (!(nCurrentCameraMode == THIRD_PERSON_CAMERA))
		return;
	/*
	if (!isLeft && !isRight)
	{
		if (m_fWheelDegree > 0)
		{
			if (!isShift)
				m_fWheelDegree -= 30 * fTimeElapsed;
			else
				m_fWheelDegree -= 30 * fTimeElapsed;
			if (m_fWheelDegree < 0.0f)
				m_fWheelDegree = 0.0f;
		}
		else if (m_fWheelDegree < 0)
		{
			if (!isShift)
				m_fWheelDegree += 30 * fTimeElapsed;
			else
				m_fWheelDegree += 30 * fTimeElapsed;

			if (m_fWheelDegree > 0.0f)
				m_fWheelDegree = 0.0f;
		}
	}
	*/
	if (isStun)
	{
		m_fTimeCount -= fTimeElapsed;
		if (m_fTimeCount < 0)
		{
			isStun = false;
			m_pSkinnedAnimationController->SetTrackEnable(SLIDING, false);
			m_pSkinnedAnimationController->SetTrackEnable(BIG_COLLISION, false);
		}
	}
	ProcessRotate(fTimeElapsed);

	XMFLOAT3 vel = m_xmf3Velocity;
	vel.y = 0;
	if (m_fWheelDegree != 0.0f && Vector3::Length(vel) > 0)   //������ ���Ⱒ�Է� & �õ��ɸ������϶��� 
	{
		float degree = m_fWheelDegree;  //���� �Ӹ��� ȸ�� ����
		
		//3.8f = ����ũ�� ���� �ķ������� ����
		//57.3f = ������ degree������ ��ȯ�ϱ����� ����
		//--------------------������
		float slipB = Vector3::DotProduct(m_xmf3Look, m_xmf3Velocity)
			/(Vector3::Length(m_xmf3Look)*Vector3::Length(m_xmf3Velocity));  //�̰� ���������� �ڻ��ΰ���.
			
		float result;

		if (Vector3::Length(m_xmf3Velocity) < 13.0f)
			result = Vector3::Length(vel) * tanf(XMConvertToRadians(degree)*slipB)*3.0f
				/ 3.8f;
		else if(!isShift)
			result = Vector3::Length(vel) * tanf(XMConvertToRadians(degree)*slipB)*0.7f
				/ 3.8f;
		else
			result = Vector3::Length(vel) * tanf(XMConvertToRadians(degree)*slipB)
				/ 3.8f;

		//--------------------------

		//w = Vector3::Length(m_xmf3Velocity) * cosf(XMConvertToRadians(slipB)) * tanf(XMConvertToRadians(degree)) / 3.8f;
		Rotate(0, result*70*fTimeElapsed, 0.0f); //degree�� �ٲ㼭 ȸ�� ���� 
		
	}
	//else
	//{
	//	//if (SoundManager::GetInstance()->Playing("Curve"))
	//	//	SoundManager::GetInstance()->Stop("Curve");
	//}
	float drag;
	//float cDrag = 0.3f;
	//������ Ⱦ�� ó�� �Ʒ��� ���� ��.
	if (isShift && m_fWheelDegree != 0)
	{
		drag = 0.5f* 1.0f* AIR * 3.8f;   //0.3f= ������� AIR = �������� , 2.2f = ���� ����
		//�帮��Ʈ�ÿ��� ������ �޴� ������ ������ �ƴ� �������̶����� ������ �� ŭ.
	}
	else
	{
		drag = 0.5f* 1.0f* AIR *2.2f;
	}
	float rR = drag * 29.7f;   // -> C_Rr
	
	
	

	float W = m_fMass * 9.8f;
	float L = 3.8f, c = 1.9f, b = 1.9f; //L�� ���� �ķ����� �Ÿ�. c = �߽ɿ��� �������� �Ÿ�, b�� ����
	float h = 1.4f; //cg�� ����
	float Wf;  //Wf���� ���� ����, Wr= �Ĺ� ���� ����
	float scaleVel = Vector3::Length(vel);
	//�ڳʸ� ���� 67830 or 67.8  or 678
	XMFLOAT3 acc = m_xmf3AcceleratingForce;
	acc.y = 0.0f;

	if (scaleVel)
	{
		Wf = 0.5f * W - (h / L)* m_fMass * Vector3::Length(acc);
		//0.5f = c/ L  c,b�� �� �����Ƿ� ����� 0.5f
	}
	else
	{
		Wf = 0.5f * W;
	}
	XMFLOAT3 xmf3Fdrag;
	xmf3Fdrag.x = -drag * vel.x * scaleVel;
	xmf3Fdrag.y = 0;
	xmf3Fdrag.z = -drag * vel.z * scaleVel;   //���� ����

	XMFLOAT3 xmf3Frr = Vector3::ScalarProduct(m_xmf3Velocity, -rR, false); //ȸ�� ����
	XMFLOAT3 xmf3Ftraction;

	xmf3Ftraction = Vector3::ScalarProduct(m_xmf3Look, m_fForce, false); //��Ű�� ���� ���� ���� ���� �� ���ϴ� ��
	if (isShift )
		xmf3Ftraction = Vector3::Add(xmf3Ftraction, Vector3::ScalarProduct(Vector3::Normalize(m_xmf3Velocity), -1000.0f, false));
	//	xmf3Ftraction = Vector3::ScalarProduct(m_xmf3Look, -400.0f, false); //�극��ũ ��

	xmf3Ftraction = Vector3::Add(xmf3Ftraction, xmf3Frr);
	xmf3Ftraction = Vector3::Add(xmf3Ftraction, xmf3Fdrag);  //����
	xmf3Ftraction = Vector3::Add(xmf3Ftraction, m_xmf3Forces);//���� �������� ���� > �̺κ��� �����Ǿ�� ��.
	//���� ������ ���ٰ� �ڷΰ��� ������ ����ؼ� ����Ǿ� ������ �ٷ� �ȹٲ�
	xmf3Ftraction.y -= 9.8f * m_fMass * 4;


	//===================
	//������

	XMFLOAT3 forceF = Vector3::ScalarProduct(vel,sinf(XMConvertToRadians(m_fWheelDegree)),true);
	forceF = Vector3::ScalarProduct(forceF,Wf,false);
	
	forceF = Vector3::ScalarProduct(forceF, cos(XMConvertToRadians(m_fWheelDegree)), false);
	xmf3Ftraction = Vector3::Add(xmf3Ftraction, forceF);

	//=================
	
	
	m_xmf3AcceleratingForce = Vector3::DivProduct(xmf3Ftraction, m_fMass, false); //���� ������ ������ ���ӷ��� ����
	Move(Vector3::ScalarProduct(m_xmf3AcceleratingForce, fTimeElapsed, false), true); //���ӷ¿� �ð���ȭ���� ���Ͽ� �ӷ¿� ����.

	float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
	float fMaxVelocityXZ = m_fMaxVelocityXZ;
	if (fLength > m_fMaxVelocityXZ)  //�ӵ� ũ�Ⱑ xz�ִ뷮�� ������ �ִ뷮���� ���ϰ� ��. �ӵ������� ������ m/s
	{
		m_xmf3Velocity.x *= (fMaxVelocityXZ / fLength);
		m_xmf3Velocity.z *= (fMaxVelocityXZ / fLength);
	}
	
	//XMVector3AngleBetweenVectors  ���ͻ��� ���� ���ϱ�
	XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, UNIT_PER_METER * fTimeElapsed, false); //10�� ���ִ� 10cm �̹Ƿ� 10�� ���ؾ���
	Move(xmf3Velocity, false); //�ӷ¸�ŭ ���� �̵�

	if (m_pUpdatedContext) OnUpdateCallback(fTimeElapsed);

//	DWORD nCurrentCameraMode = m_pCamera->GetMode();
	//if (nCurrentCameraMode == THIRD_PERSON_CAMERA) 
	m_pCamera->Update(m_xmf3Position, fTimeElapsed);
	if (m_pCameraUpdatedContext) OnCameraUpdateCallback(fTimeElapsed);
	//if (nCurrentCameraMode == THIRD_PERSON_CAMERA)
	((CThirdPersonCamera*)m_pCamera)->SetLookAt(m_xmf3Position);
	m_pCamera->RegenerateViewMatrix();

	//fLength = Vector3::Length(m_xmf3Velocity);
	//float fDeceleration = (m_fFriction * fTimeElapsed);
	//if (fDeceleration > fLength) fDeceleration = fLength;
	//m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));
	m_xmf3Forces = XMFLOAT3(0, 0, 0);

	if (isWalking)
	{
		if (::IsEqual(Vector3::Length(m_xmf3Velocity), 0.0f, 0.001f))   //�ӷ��� 0�� �ٻ��� ���
		{
			returnIdle();
		}
	}
}

void CPlayer::Animate(float fTimeElapsed)
{
	CGameObject::Animate(fTimeElapsed);
}

CCamera *CPlayer::OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode)
{
	CCamera *pNewCamera = NULL;
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		pNewCamera = new CFirstPersonCamera(m_pCamera);
		break;
	case THIRD_PERSON_CAMERA:
		pNewCamera = new CThirdPersonCamera(m_pCamera);
		break;
	case SPACESHIP_CAMERA:
		pNewCamera = new CSpaceShipCamera(m_pCamera);
		break;
	}
	if (nCurrentCameraMode == SPACESHIP_CAMERA)
	{
		m_xmf3Right = Vector3::Normalize(XMFLOAT3(m_xmf3Right.x, 0.0f, m_xmf3Right.z));
		m_xmf3Up = Vector3::Normalize(XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_xmf3Look = Vector3::Normalize(XMFLOAT3(m_xmf3Look.x, 0.0f, m_xmf3Look.z));

		m_fPitch = 0.0f;
		m_fRoll = 0.0f;
		m_fYaw = Vector3::Angle(XMFLOAT3(0.0f, 0.0f, 1.0f), m_xmf3Look);
		if (m_xmf3Look.x < 0.0f) m_fYaw = -m_fYaw;
	}
	else if ((nNewCameraMode == SPACESHIP_CAMERA) && m_pCamera)
	{
		m_xmf3Right = m_pCamera->GetRightVector();
		m_xmf3Up = m_pCamera->GetUpVector();
		m_xmf3Look = m_pCamera->GetLookVector();
	}

	if (pNewCamera)
	{
		pNewCamera->SetMode(nNewCameraMode);
		pNewCamera->SetPlayer(this);
	}

	if (m_pCamera) delete m_pCamera;

	return(pNewCamera);
}

void CPlayer::OnPrepareRender()
{
	m_xmf4x4ToParent._11 = m_xmf3Right.x; m_xmf4x4ToParent._12 = m_xmf3Right.y; m_xmf4x4ToParent._13 = m_xmf3Right.z;
	m_xmf4x4ToParent._21 = m_xmf3Up.x; m_xmf4x4ToParent._22 = m_xmf3Up.y; m_xmf4x4ToParent._23 = m_xmf3Up.z;
	m_xmf4x4ToParent._31 = m_xmf3Look.x; m_xmf4x4ToParent._32 = m_xmf3Look.y; m_xmf4x4ToParent._33 = m_xmf3Look.z;
	m_xmf4x4ToParent._41 = m_xmf3Position.x; m_xmf4x4ToParent._42 = m_xmf3Position.y; m_xmf4x4ToParent._43 = m_xmf3Position.z;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
#define _WITH_DEBUG_CALLBACK_DATA

void CSoundCallbackHandler::HandleCallback(void *pCallbackData)
{
	_TCHAR *pWavName = (_TCHAR *)pCallbackData;
#ifdef _WITH_DEBUG_CALLBACK_DATA
	TCHAR pstrDebug[256] = { 0 };
	_stprintf_s(pstrDebug, 256, _T("%s\n"), pWavName);
	OutputDebugString(pstrDebug);
#endif
#ifdef _WITH_SOUND_RESOURCE
	PlaySound(pWavName, ::ghAppInstance, SND_RESOURCE | SND_ASYNC);
#else
	PlaySound(pWavName, NULL, SND_FILENAME | SND_ASYNC);
#endif
}
void CFuncCallbackHandler::HandleCallback(void *pAnimationController, int nSet)
{

	CAnimationController* controller = (CAnimationController*)pAnimationController;
	controller->SetTrackEnable(nSet, true);
}

CDinoRunPlayer::CDinoRunPlayer(CreateManager* pCreateManager, string sModelName) : CPlayer()
{

	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);
	CLoadedModelInfo *pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pCreateManager, sModelName.c_str(), NULL);
	SetChild(pAngrybotModel->m_pModelRootObject->GetChild(), true);
	m_pSkinnedAnimationController = new CAnimationController(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), 16, pAngrybotModel);
	//���� �Ű������� �� 1�� �ִϸ��̼� Ʈ���� ���� ����� idle ���̴� 1�� �þ ���� ���� ���ϰ�.
	m_fMass = 130;

	m_fMaxForce = 2000;


	m_pSkinnedAnimationController->m_CurrentTrack = IDLE;
	m_pSkinnedAnimationController->SetTrackAnimationSet(IDLE, IDLE); //left_turn_start

	
	m_pSkinnedAnimationController->SetTrackAnimationSet(IDLE_LEFT_TURN, IDLE_LEFT_TURN);
	m_pSkinnedAnimationController->SetCallbackFuncKeys(IDLE_LEFT_TURN, 1);
	m_pSkinnedAnimationController->SetCallbackFuncKey(IDLE_LEFT_TURN, 0, 0.5f, IDLE_LEFT_TURN, IDLE_LEFT_TURNING);

	m_pSkinnedAnimationController->SetTrackAnimationSet(IDLE_RIGHT_TURN, IDLE_RIGHT_TURN);
	m_pSkinnedAnimationController->SetCallbackFuncKeys(IDLE_RIGHT_TURN, 1); //����Ʈ �Ͽ����� ����ִϸ��̼��� ������ 1
	m_pSkinnedAnimationController->SetCallbackFuncKey(IDLE_RIGHT_TURN, 0, 0.5f, IDLE_RIGHT_TURN, IDLE_RIGHT_TURNING);

	m_pSkinnedAnimationController->SetTrackAnimationSet(IDLE_LEFT_TURNING, IDLE_LEFT_TURNING);
	m_pSkinnedAnimationController->SetTrackAnimationSet(IDLE_RIGHT_TURNING, IDLE_RIGHT_TURNING);
	m_pSkinnedAnimationController->SetTrackAnimationSet(IDLE_LEFT_RETURN, IDLE_LEFT_RETURN);
	m_pSkinnedAnimationController->SetCallbackFuncKeys(IDLE_LEFT_RETURN, 1);
	m_pSkinnedAnimationController->SetTrackAnimationSet(IDLE_RIGHT_RETURN, IDLE_RIGHT_RETURN);
	m_pSkinnedAnimationController->SetCallbackFuncKeys(IDLE_RIGHT_RETURN, 1);

	m_pSkinnedAnimationController->SetTrackAnimationSet(RUN, RUN);
	m_pSkinnedAnimationController->SetTrackAnimationSet(RUN_LEFT_TURN, RUN_LEFT_TURN);
	m_pSkinnedAnimationController->SetCallbackFuncKeys(RUN_LEFT_TURN, 1);
	m_pSkinnedAnimationController->SetCallbackFuncKey(RUN_LEFT_TURN, 0, 0.5f, RUN_LEFT_TURN, RUN_LEFT_TURNING);

	m_pSkinnedAnimationController->SetTrackAnimationSet(RUN_RIGHT_TURN, RUN_RIGHT_TURN);
	m_pSkinnedAnimationController->SetCallbackFuncKeys(RUN_RIGHT_TURN, 1);
	m_pSkinnedAnimationController->SetCallbackFuncKey(RUN_RIGHT_TURN, 0, 0.5f, RUN_RIGHT_TURN, RUN_RIGHT_TURNING);

	m_pSkinnedAnimationController->SetTrackAnimationSet(RUN_LEFT_TURNING, RUN_LEFT_TURNING);
	m_pSkinnedAnimationController->SetTrackAnimationSet(RUN_RIGHT_TURNING, RUN_RIGHT_TURNING);
	m_pSkinnedAnimationController->SetTrackAnimationSet(RUN_LEFT_RETURN, RUN_LEFT_RETURN);
	m_pSkinnedAnimationController->SetCallbackFuncKeys(RUN_LEFT_RETURN, 1);
	m_pSkinnedAnimationController->SetTrackAnimationSet(RUN_RIGHT_RETURN, RUN_RIGHT_RETURN);
	m_pSkinnedAnimationController->SetCallbackFuncKeys(RUN_RIGHT_RETURN, 1);
	
	m_pSkinnedAnimationController->SetTrackEnable(IDLE, true);

	m_pSkinnedAnimationController->SetTrackAnimationSet(SLIDING, SLIDING); //left_turn_start
	m_pSkinnedAnimationController->SetTrackAnimationSet(BIG_COLLISION, BIG_COLLISION); //left_turn_start
	//m_pSkinnedAnimationController->SetTrackEnable(BIG_COLLISION, true);

	//m_pSkinnedAnimationController->SetTrackEnable(RUN_RIGHT_RETURN, true);

	/*
	m_pSkinnedAnimationController->SetCallbackKeys(1, 3);
#ifdef _WITH_SOUND_RESOURCE
	m_pSkinnedAnimationController->SetCallbackKey(1, 0, 0.1f, _T("Footstep01"));
	m_pSkinnedAnimationController->SetCallbackKey(1, 1, 0.5f, _T("Footstep02"));
	m_pSkinnedAnimationController->SetCallbackKey(1, 2, 0.9f, _T("Footstep03"));
#else
	m_pSkinnedAnimationController->SetCallbackKey(1, 0, 0.1f, _T("Sound/Footstep01.wav"));
	m_pSkinnedAnimationController->SetCallbackKey(1, 1, 0.5f, _T("Sound/Footstep02.wav"));
	m_pSkinnedAnimationController->SetCallbackKey(1, 2, 0.9f, _T("Sound/Footstep03.wav"));
#endif
	CAnimationCallbackHandler *pAnimationCallbackHandler = new CSoundCallbackHandler();
	m_pSkinnedAnimationController->SetAnimationCallbackHandler(1, pAnimationCallbackHandler);
*/
//SetUpdatedContext(pContext);
//SetCameraUpdatedContext(pContext);

	if (pAngrybotModel) delete pAngrybotModel;

	CreateShaderVariables(pCreateManager);

	SetPosition(XMFLOAT3(800.0f, 76.0f, 1150.0f)); //(XMFLOAT3(700.0f, 76.0f, 1150.0f));//800,76,900

	UpdateTransform(NULL);

	m_pParticleSystem = new ParticleSystem(pCreateManager,DUST_PARTICLE, this, XMFLOAT3(0.0f, 0, 18));
	//SetScale(XMFLOAT3(0.05f, 0.05f, 0.05f));

}

CDinoRunPlayer::~CDinoRunPlayer()
{
}

void CDinoRunPlayer::OnPrepareRender()
{
	CPlayer::OnPrepareRender();

	//m_xmf4x4ToParent = Matrix4x4::Multiply(XMMatrixScaling(m_xmf3Scale.x, m_xmf3Scale.y, m_xmf3Scale.z), m_xmf4x4ToParent);
	//m_xmf4x4ToParent = Matrix4x4::Multiply(XMMatrixRotationY(XMConvertToRadians(180.f)), m_xmf4x4ToParent);
}

CCamera *CDinoRunPlayer::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode) return(m_pCamera);
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		SetFriction(250.0f);
		SetGravity(XMFLOAT3(0.0f, -400.0f, 0.0f));
		SetMaxVelocityXZ(300.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(FIRST_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	case SPACESHIP_CAMERA:
		SetFriction(125.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(300.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(SPACESHIP_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 1800.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	case THIRD_PERSON_CAMERA:
		SetFriction(150.0f);
		SetGravity(XMFLOAT3(0.0f, -0.0f, 0.0f));
		SetMaxVelocityXZ(25.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.10f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 30.0f, -60.0f));
		m_pCamera->SetPosition(Vector3::Add(m_xmf3Position, m_pCamera->GetOffset()));
		m_pCamera->GenerateProjectionMatrix(1.01f, 1800.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		m_pCamera->SetLookAt(GetPosition());
		m_pCamera->RegenerateViewMatrix();
		break;
	default:
		break;
	}

	return(m_pCamera);
}

void CDinoRunPlayer::OnUpdateCallback(float fTimeElapsed)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)m_pUpdatedContext;
	XMFLOAT3 xmf3Scale = pTerrain->GetScale();
	XMFLOAT3 xmf3PlayerPosition = GetPosition();
	int z = (int)(xmf3PlayerPosition.z / xmf3Scale.z);
	bool bReverseQuad = ((z % 2) != 0);
#ifndef _WITH_LEFT_HAND_COORDINATES
	float fHeight = pTerrain->GetHeight(xmf3PlayerPosition.x, 256 * xmf3Scale.z - xmf3PlayerPosition.z) + 10.0f;
#else
	float fHeight = pTerrain->GetHeight(xmf3PlayerPosition.x, 256 * xmf3Scale.z - xmf3PlayerPosition.z) + 7.0f;
#endif

	if (xmf3PlayerPosition.y < fHeight)
	{
		XMFLOAT3 xmf3PlayerVelocity = GetVelocity();
		xmf3PlayerVelocity.y = 0.0f;
		SetVelocity(xmf3PlayerVelocity);
		xmf3PlayerPosition.y = fHeight;
		SetPosition(xmf3PlayerPosition);
	}
}

void CDinoRunPlayer::OnCameraUpdateCallback(float fTimeElapsed)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)m_pCameraUpdatedContext;
	XMFLOAT3 xmf3Scale = pTerrain->GetScale();
	XMFLOAT3 xmf3CameraPosition = m_pCamera->GetPosition();
	int z = (int)(xmf3CameraPosition.z / xmf3Scale.z);
	bool bReverseQuad = ((z % 2) != 0);
#ifndef _WITH_LEFT_HAND_COORDINATES
	float fHeight = pTerrain->GetHeight(xmf3CameraPosition.x, 256 * xmf3Scale.z - xmf3CameraPosition.z) + 10.0f;
#else
	float fHeight = pTerrain->GetHeight(xmf3CameraPosition.x, 256 * xmf3Scale.z - xmf3CameraPosition.z) + 10.0f;
#endif
	if (xmf3CameraPosition.y <= fHeight)
	{
		xmf3CameraPosition.y = fHeight;
		m_pCamera->SetPosition(xmf3CameraPosition);
		if (m_pCamera->GetMode() == THIRD_PERSON_CAMERA)
		{
			CThirdPersonCamera *p3rdPersonCamera = (CThirdPersonCamera *)m_pCamera;
			p3rdPersonCamera->SetLookAt(GetPosition());
		}
	}
}


void CDinoRunPlayer::KeyDownLeft()
{
	if (isLeft)
		return;

	isLeft = true;
	UINT prevCurrent = m_pSkinnedAnimationController->m_CurrentTrack;
	float positionTime = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[m_pSkinnedAnimationController->m_CurrentTrack]->m_fPosition;

	float length = 0.0f;

	if (isWalking)
	{
		if (isRight)
		{
			if (m_pSkinnedAnimationController->m_pAnimationTracks[RUN_RIGHT_TURNING].m_bEnable)
			{
				m_pSkinnedAnimationController->SetTrackEnable(RUN_RIGHT_RETURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(RUN_RIGHT_RETURN, 0, 0.48f, RUN_RIGHT_RETURN, RUN_LEFT_TURN);

				SetTrackAnimationPosition(RUN_RIGHT_RETURN, 0);
				SetTrackAnimationPosition(RUN_LEFT_TURN, 0);
				SetTrackAnimationPosition(RUN_LEFT_TURNING, 0);
				m_pSkinnedAnimationController->m_CurrentTrack = RUN_RIGHT_RETURN;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[RUN_RIGHT_TURN].m_bEnable)
			{
				length = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[RUN_RIGHT_RETURN]->m_fLength;
				m_pSkinnedAnimationController->SetTrackEnable(RUN_RIGHT_RETURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(RUN_RIGHT_RETURN, 0, 0.48f, RUN_RIGHT_RETURN, RUN_LEFT_TURN);
				SetTrackAnimationPosition(RUN_RIGHT_RETURN, length - positionTime);
				m_pSkinnedAnimationController->m_CurrentTrack = RUN_RIGHT_RETURN;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[RUN_LEFT_RETURN].m_bEnable)
			{
				length = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[RUN_LEFT_RETURN]->m_fLength;
				m_pSkinnedAnimationController->SetTrackEnable(RUN_LEFT_TURN, true);
				SetTrackAnimationPosition(RUN_LEFT_TURN, length - positionTime);
				m_pSkinnedAnimationController->m_CurrentTrack = RUN_LEFT_TURN;
			}
			else
				return;
		}
		else
		{
			if (m_pSkinnedAnimationController->m_pAnimationTracks[RUN_LEFT_RETURN].m_bEnable)
			{
				length = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[RUN_LEFT_RETURN]->m_fLength;
				m_pSkinnedAnimationController->SetTrackEnable(RUN_LEFT_TURN, true);
				SetTrackAnimationPosition(RUN_LEFT_TURN, length - positionTime);
				m_pSkinnedAnimationController->m_CurrentTrack = RUN_LEFT_TURN;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[RUN_RIGHT_RETURN].m_bEnable)
			{
				m_pSkinnedAnimationController->SetCallbackFuncKey(RUN_RIGHT_RETURN, 0, 0.48f, RUN_RIGHT_RETURN, RUN_LEFT_TURN);
				return;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[RUN].m_bEnable)
			{
				m_pSkinnedAnimationController->SetTrackEnable(RUN_LEFT_TURN, true);
				//
				SetTrackAnimationPosition(RUN_LEFT_TURN, 0);
				//
				m_pSkinnedAnimationController->m_CurrentTrack = RUN_LEFT_TURN;
			}
			else
				return;

		}
	}
	else
	{
		if (isRight)
		{
			if (m_pSkinnedAnimationController->m_pAnimationTracks[IDLE_RIGHT_TURNING].m_bEnable)
			{
				m_pSkinnedAnimationController->SetTrackEnable(IDLE_RIGHT_RETURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(IDLE_RIGHT_RETURN, 0, 0.48f, IDLE_RIGHT_RETURN, IDLE_LEFT_TURN);
				SetTrackAnimationPosition(IDLE_RIGHT_RETURN, 0);
				SetTrackAnimationPosition(IDLE_LEFT_TURN, 0);
				SetTrackAnimationPosition(IDLE_LEFT_TURNING, 0);
				m_pSkinnedAnimationController->m_CurrentTrack = IDLE_RIGHT_RETURN;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[IDLE_RIGHT_TURN].m_bEnable)
			{
				length = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[IDLE_RIGHT_RETURN]->m_fLength;
				m_pSkinnedAnimationController->SetTrackEnable(IDLE_RIGHT_RETURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(IDLE_RIGHT_RETURN, 0, 0.48f, IDLE_RIGHT_RETURN, IDLE_LEFT_TURN);
				SetTrackAnimationPosition(IDLE_RIGHT_RETURN, length - positionTime);
				m_pSkinnedAnimationController->m_CurrentTrack = IDLE_RIGHT_RETURN;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[IDLE_LEFT_RETURN].m_bEnable)
			{
				length = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[IDLE_LEFT_RETURN]->m_fLength;
				m_pSkinnedAnimationController->SetTrackEnable(IDLE_LEFT_TURN, true);
				SetTrackAnimationPosition(IDLE_LEFT_TURN, length - positionTime);
				m_pSkinnedAnimationController->m_CurrentTrack = IDLE_LEFT_TURN;
			}
			else
				return;
		}
		else
		{
			if (m_pSkinnedAnimationController->m_pAnimationTracks[IDLE_LEFT_RETURN].m_bEnable)
			{
				length = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[IDLE_LEFT_RETURN]->m_fLength;
				m_pSkinnedAnimationController->SetTrackEnable(IDLE_LEFT_TURN, true);
				SetTrackAnimationPosition(IDLE_LEFT_TURN, length - positionTime);
				m_pSkinnedAnimationController->m_CurrentTrack = IDLE_LEFT_TURN;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[IDLE_RIGHT_RETURN].m_bEnable)
			{
				m_pSkinnedAnimationController->SetCallbackFuncKey(IDLE_RIGHT_RETURN, 0, 0.48f, IDLE_RIGHT_RETURN, IDLE_LEFT_TURN);
				return;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[IDLE].m_bEnable)
			{
				m_pSkinnedAnimationController->SetTrackEnable(IDLE_LEFT_TURN, true);
				//
				SetTrackAnimationPosition(IDLE_LEFT_TURN, 0);
				//
				m_pSkinnedAnimationController->m_CurrentTrack = IDLE_LEFT_TURN;
			}
			else
				return;

		}
	}
	m_pSkinnedAnimationController->m_pAnimationTracks[prevCurrent].m_bEnable = false;
	SetTrackAnimationPosition(prevCurrent, 0.0f);
}
void CDinoRunPlayer::KeyDownRight()
{
	if (isRight)
		return;
	isRight = true;
	int prevCurrent = m_pSkinnedAnimationController->m_CurrentTrack;
	float positionTime = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[m_pSkinnedAnimationController->m_CurrentTrack]->m_fPosition;

	float length = 0.0f;
	if (isWalking)
	{
		if (isLeft)
		{
			if (m_pSkinnedAnimationController->m_pAnimationTracks[RUN_LEFT_TURNING].m_bEnable)
			{
				m_pSkinnedAnimationController->SetTrackEnable(RUN_LEFT_RETURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(RUN_LEFT_RETURN, 0, 0.48f, RUN_LEFT_RETURN, RUN_RIGHT_TURN);
				SetTrackAnimationPosition(RUN_LEFT_RETURN, 0);
				SetTrackAnimationPosition(RUN_RIGHT_TURN, 0);
				SetTrackAnimationPosition(RUN_RIGHT_TURNING, 0);
				m_pSkinnedAnimationController->m_CurrentTrack = RUN_LEFT_RETURN;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[RUN_LEFT_TURN].m_bEnable)
			{
				length = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[RUN_LEFT_RETURN]->m_fLength;
				m_pSkinnedAnimationController->SetTrackEnable(RUN_LEFT_RETURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(RUN_LEFT_RETURN, 0, 0.48f, RUN_LEFT_RETURN, RUN_RIGHT_TURN);
				SetTrackAnimationPosition(RUN_LEFT_RETURN, length - positionTime);
				m_pSkinnedAnimationController->m_CurrentTrack = RUN_LEFT_RETURN;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[RUN_RIGHT_RETURN].m_bEnable)
			{
				length = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[RUN_RIGHT_RETURN]->m_fLength;
				m_pSkinnedAnimationController->SetTrackEnable(RUN_RIGHT_TURN, true);
				SetTrackAnimationPosition(RUN_RIGHT_TURN, length - positionTime);
				m_pSkinnedAnimationController->m_CurrentTrack = RUN_RIGHT_TURN;
			}
			else
				return;
		}
		else
		{
			if (m_pSkinnedAnimationController->m_pAnimationTracks[RUN_RIGHT_RETURN].m_bEnable)
			{
				length = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[RUN_RIGHT_RETURN]->m_fLength;
				m_pSkinnedAnimationController->SetTrackEnable(RUN_RIGHT_TURN, true);
				SetTrackAnimationPosition(RUN_RIGHT_TURN, length - positionTime);
				m_pSkinnedAnimationController->m_CurrentTrack = RUN_RIGHT_TURN;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[RUN_LEFT_RETURN].m_bEnable)
			{
				m_pSkinnedAnimationController->SetCallbackFuncKey(RUN_LEFT_RETURN, 0, 0.48f, RUN_LEFT_RETURN, RUN_RIGHT_TURN);
				return;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[RUN].m_bEnable)
			{
				m_pSkinnedAnimationController->SetTrackEnable(RUN_RIGHT_TURN, true);
				//
				SetTrackAnimationPosition(RUN_RIGHT_TURN, 0);
				//
				m_pSkinnedAnimationController->m_CurrentTrack = RUN_RIGHT_TURN;
			}
			else
				return;
		}
	}
	else
	{
		if (isLeft)
		{
			if (m_pSkinnedAnimationController->m_pAnimationTracks[IDLE_LEFT_TURNING].m_bEnable)
			{
				m_pSkinnedAnimationController->SetTrackEnable(IDLE_LEFT_RETURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(IDLE_LEFT_RETURN, 0, 0.48f, IDLE_LEFT_RETURN, IDLE_RIGHT_TURN);
				SetTrackAnimationPosition(IDLE_LEFT_RETURN, 0);
				SetTrackAnimationPosition(IDLE_RIGHT_TURN, 0);
				SetTrackAnimationPosition(IDLE_RIGHT_TURNING, 0);
				m_pSkinnedAnimationController->m_CurrentTrack = IDLE_LEFT_RETURN;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[IDLE_LEFT_TURN].m_bEnable)
			{
				length = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[IDLE_LEFT_RETURN]->m_fLength;
				m_pSkinnedAnimationController->SetTrackEnable(IDLE_LEFT_RETURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(IDLE_LEFT_RETURN, 0, 0.48f, IDLE_LEFT_RETURN, IDLE_RIGHT_TURN);
				SetTrackAnimationPosition(IDLE_LEFT_RETURN, length - positionTime);
				m_pSkinnedAnimationController->m_CurrentTrack = IDLE_LEFT_RETURN;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[IDLE_RIGHT_RETURN].m_bEnable)
			{
				length = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[IDLE_RIGHT_RETURN]->m_fLength;
				m_pSkinnedAnimationController->SetTrackEnable(IDLE_RIGHT_TURN, true);
				SetTrackAnimationPosition(IDLE_RIGHT_TURN, length - positionTime);
				m_pSkinnedAnimationController->m_CurrentTrack = IDLE_RIGHT_TURN;
			}
			else
				return;
		}
		else
		{
			if (m_pSkinnedAnimationController->m_pAnimationTracks[IDLE_RIGHT_RETURN].m_bEnable)
			{
				length = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[IDLE_RIGHT_RETURN]->m_fLength;
				m_pSkinnedAnimationController->SetTrackEnable(IDLE_RIGHT_TURN, true);
				SetTrackAnimationPosition(IDLE_RIGHT_TURN, length - positionTime);
				m_pSkinnedAnimationController->m_CurrentTrack = IDLE_RIGHT_TURN;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[IDLE_LEFT_RETURN].m_bEnable)
			{
				m_pSkinnedAnimationController->SetCallbackFuncKey(IDLE_LEFT_RETURN, 0, 0.48f, IDLE_LEFT_RETURN, IDLE_RIGHT_TURN);
				return;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[IDLE].m_bEnable)
			{
				m_pSkinnedAnimationController->SetTrackEnable(IDLE_RIGHT_TURN, true);
				//
				SetTrackAnimationPosition(IDLE_RIGHT_TURN, 0);
				//
				m_pSkinnedAnimationController->m_CurrentTrack = IDLE_RIGHT_TURN;
			}
			else
				return;
		}
	}
	m_pSkinnedAnimationController->m_pAnimationTracks[prevCurrent].m_bEnable = false;
	SetTrackAnimationPosition(prevCurrent, 0.0f);
}
void CDinoRunPlayer::KeyDownUp()
{
	if (isUp)
		return;
	isUp = true;

	if (isDown || isWalking)
		return;
	UINT curTrack = m_pSkinnedAnimationController->m_CurrentTrack;
	UINT nextTrack = curTrack + ANIMATIONGAP;

	float positionTime = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[curTrack]->m_fPosition;

	m_pSkinnedAnimationController->m_pAnimationTracks[curTrack].m_bEnable = false;
	SetTrackAnimationPosition(m_pSkinnedAnimationController->m_CurrentTrack, 0.0f);

	m_pSkinnedAnimationController->m_CurrentTrack = nextTrack;
	SetTrackAnimationPosition(nextTrack, positionTime);

	isWalking = true;
	UINT track;
	m_pSkinnedAnimationController->SetTrackEnable(nextTrack, true);
	switch (nextTrack)
	{
	case RUN_LEFT_TURN:
	case RUN_RIGHT_TURN:  //+2
		break;
	case RUN_LEFT_RETURN:
		track = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[curTrack]->m_pCallbackFuncKeys[0].m_pCallbackData + ANIMATIONGAP;
		m_pSkinnedAnimationController->SetCallbackFuncKey(RUN_LEFT_RETURN, 0, 0.48f, RUN_LEFT_RETURN, track);
		break;
	case RUN_RIGHT_RETURN:
		track = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[curTrack]->m_pCallbackFuncKeys[0].m_pCallbackData + ANIMATIONGAP;
		m_pSkinnedAnimationController->SetCallbackFuncKey(RUN_RIGHT_RETURN, 0, 0.48f, RUN_RIGHT_RETURN, track);
		break;
	}
}
void CDinoRunPlayer::KeyDownDown()
{
	if (isDown)
		return;
	isDown = true;

	if (isUp || isWalking)
		return;
	UINT curTrack = m_pSkinnedAnimationController->m_CurrentTrack;
	UINT nextTrack = curTrack + ANIMATIONGAP;

	float positionTime = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[curTrack]->m_fPosition;

	m_pSkinnedAnimationController->m_pAnimationTracks[curTrack].m_bEnable = false;
	SetTrackAnimationPosition(m_pSkinnedAnimationController->m_CurrentTrack, 0.0f);

	m_pSkinnedAnimationController->m_CurrentTrack = nextTrack;
	SetTrackAnimationPosition(nextTrack, positionTime);

	isWalking = true;
	UINT track;

	m_pSkinnedAnimationController->SetTrackEnable(nextTrack, true);
	switch (nextTrack)
	{
	case RUN_LEFT_TURN:
	case RUN_RIGHT_TURN:  //+2
		break;
	case RUN_LEFT_RETURN:
		track = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[curTrack]->m_pCallbackFuncKeys[0].m_pCallbackData + ANIMATIONGAP;
		m_pSkinnedAnimationController->SetCallbackFuncKey(RUN_LEFT_RETURN, 0, 0.48f, RUN_LEFT_RETURN, track);
		break;
	case RUN_RIGHT_RETURN:
		track = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[curTrack]->m_pCallbackFuncKeys[0].m_pCallbackData + ANIMATIONGAP;
		m_pSkinnedAnimationController->SetCallbackFuncKey(RUN_RIGHT_RETURN, 0, 0.48f, RUN_RIGHT_RETURN, track);
		break;
	}
}
void CDinoRunPlayer::KeyUpLeft()
{
	if (!isLeft)
		return;
	isLeft = false;
	int prevCurrent = m_pSkinnedAnimationController->m_CurrentTrack;
	float positionTime = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[m_pSkinnedAnimationController->m_CurrentTrack]->m_fPosition;
	//��> Ʈ������ �ִ°� �߸�����. set�� ������Ÿ���� �ʿ�
	float length = 0.0f;

	if (isWalking)
	{
		if (isRight)
		{
			if (m_pSkinnedAnimationController->m_pAnimationTracks[RUN_LEFT_TURN].m_bEnable)
			{
				length = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[RUN_LEFT_TURN]->m_fLength;
				m_pSkinnedAnimationController->SetTrackEnable(RUN_LEFT_RETURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(RUN_LEFT_RETURN, 0, 0.48f, RUN_LEFT_RETURN, RUN_RIGHT_TURN);
				SetTrackAnimationPosition(RUN_LEFT_RETURN, length - positionTime);
				m_pSkinnedAnimationController->m_CurrentTrack = RUN_LEFT_RETURN;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[RUN_RIGHT_RETURN].m_bEnable)
			{
				length = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[RUN_RIGHT_RETURN]->m_fLength;
				m_pSkinnedAnimationController->SetTrackEnable(RUN_RIGHT_TURN, true);
				SetTrackAnimationPosition(RUN_RIGHT_TURN, length - positionTime);
				m_pSkinnedAnimationController->m_CurrentTrack = RUN_RIGHT_TURN;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[RUN_LEFT_TURNING].m_bEnable)
			{
				m_pSkinnedAnimationController->SetTrackEnable(RUN_LEFT_RETURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(RUN_LEFT_RETURN, 0, 0.48f, RUN_LEFT_RETURN, RUN_RIGHT_TURN);
				//
				SetTrackAnimationPosition(RUN_LEFT_RETURN, 0);
				//
				m_pSkinnedAnimationController->m_CurrentTrack = RUN_LEFT_RETURN;

			}
			else
				return;
		}
		else
		{
			if (m_pSkinnedAnimationController->m_pAnimationTracks[RUN_LEFT_TURN].m_bEnable)
			{
				length = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[RUN_LEFT_TURN]->m_fLength;
				m_pSkinnedAnimationController->SetTrackEnable(RUN_LEFT_RETURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(RUN_LEFT_RETURN, 0, 0.48f, RUN_LEFT_RETURN, RUN);
				SetTrackAnimationPosition(RUN_LEFT_RETURN, length - positionTime);
				m_pSkinnedAnimationController->m_CurrentTrack = RUN_LEFT_RETURN;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[RUN_LEFT_TURNING].m_bEnable)
			{
				m_pSkinnedAnimationController->SetTrackEnable(RUN_LEFT_RETURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(RUN_LEFT_RETURN, 0, 0.48f, RUN_LEFT_RETURN, RUN);
				//
				SetTrackAnimationPosition(RUN_LEFT_RETURN, 0);
				//
				m_pSkinnedAnimationController->m_CurrentTrack = RUN_LEFT_RETURN;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[RUN_RIGHT_RETURN].m_bEnable)
			{
				m_pSkinnedAnimationController->SetCallbackFuncKey(RUN_RIGHT_RETURN, 0, 0.48f, RUN_RIGHT_RETURN, RUN);
				return;
			}
			else
				return;
		}
	}
	else
	{
		if (isRight)
		{
			if (m_pSkinnedAnimationController->m_pAnimationTracks[IDLE_LEFT_TURN].m_bEnable)
			{
				length = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[IDLE_LEFT_TURN]->m_fLength;
				m_pSkinnedAnimationController->SetTrackEnable(IDLE_LEFT_RETURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(IDLE_LEFT_RETURN, 0, 0.48f, IDLE_LEFT_RETURN, IDLE_RIGHT_TURN);
				SetTrackAnimationPosition(IDLE_LEFT_RETURN, length - positionTime);
				m_pSkinnedAnimationController->m_CurrentTrack = IDLE_LEFT_RETURN;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[IDLE_RIGHT_RETURN].m_bEnable)
			{
				length = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[IDLE_RIGHT_RETURN]->m_fLength;
				m_pSkinnedAnimationController->SetTrackEnable(IDLE_RIGHT_TURN, true);
				SetTrackAnimationPosition(IDLE_RIGHT_TURN, length - positionTime);
				m_pSkinnedAnimationController->m_CurrentTrack = IDLE_RIGHT_TURN;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[IDLE_LEFT_TURNING].m_bEnable)
			{
				m_pSkinnedAnimationController->SetTrackEnable(IDLE_LEFT_RETURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(IDLE_LEFT_RETURN, 0, 0.48f, IDLE_LEFT_RETURN, IDLE_RIGHT_TURN);
				//
				SetTrackAnimationPosition(IDLE_LEFT_RETURN, 0);
				//
				m_pSkinnedAnimationController->m_CurrentTrack = IDLE_LEFT_RETURN;

			}
			else
				return;

		}
		else
		{
			if (m_pSkinnedAnimationController->m_pAnimationTracks[IDLE_LEFT_TURN].m_bEnable)
			{
				length = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[IDLE_LEFT_TURN]->m_fLength;
				m_pSkinnedAnimationController->SetTrackEnable(IDLE_LEFT_RETURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(IDLE_LEFT_RETURN, 0, 0.48f, IDLE_LEFT_RETURN, IDLE);
				SetTrackAnimationPosition(IDLE_LEFT_RETURN, length - positionTime);
				m_pSkinnedAnimationController->m_CurrentTrack = IDLE_LEFT_RETURN;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[IDLE_LEFT_TURNING].m_bEnable)
			{
				m_pSkinnedAnimationController->SetTrackEnable(IDLE_LEFT_RETURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(IDLE_LEFT_RETURN, 0, 0.48f, IDLE_LEFT_RETURN, IDLE);
				//
				SetTrackAnimationPosition(IDLE_LEFT_RETURN, 0);
				//
				m_pSkinnedAnimationController->m_CurrentTrack = IDLE_LEFT_RETURN;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[IDLE_RIGHT_RETURN].m_bEnable)
			{
				m_pSkinnedAnimationController->SetCallbackFuncKey(IDLE_RIGHT_RETURN, 0, 0.48f, IDLE_RIGHT_RETURN, IDLE);
				return;
			}
			else
				return;
		}
	}
	m_pSkinnedAnimationController->m_pAnimationTracks[prevCurrent].m_bEnable = false;
	SetTrackAnimationPosition(prevCurrent, 0.0f);
}
void CDinoRunPlayer::KeyUpRight()
{
	if (!isRight)
		return;
	isRight = false;
	int prevCurrent = m_pSkinnedAnimationController->m_CurrentTrack;
	float positionTime = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[m_pSkinnedAnimationController->m_CurrentTrack]->m_fPosition;

	float length = 0.0f;
	if (isWalking)
	{
		if (isLeft)
		{
			if (m_pSkinnedAnimationController->m_pAnimationTracks[RUN_RIGHT_TURN].m_bEnable)
			{
				length = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[RUN_RIGHT_TURN]->m_fLength;
				m_pSkinnedAnimationController->SetTrackEnable(RUN_RIGHT_RETURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(RUN_RIGHT_RETURN, 0, 0.48f, RUN_RIGHT_RETURN, RUN_LEFT_TURN);
				SetTrackAnimationPosition(RUN_RIGHT_RETURN, length - positionTime);
				m_pSkinnedAnimationController->m_CurrentTrack = RUN_RIGHT_RETURN;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[RUN_LEFT_RETURN].m_bEnable)
			{
				length = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[RUN_LEFT_RETURN]->m_fLength;
				m_pSkinnedAnimationController->SetTrackEnable(RUN_LEFT_TURN, true);
				SetTrackAnimationPosition(RUN_LEFT_TURN, length - positionTime);
				m_pSkinnedAnimationController->m_CurrentTrack = RUN_LEFT_TURN;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[RUN_RIGHT_TURNING].m_bEnable)
			{
				m_pSkinnedAnimationController->SetTrackEnable(RUN_RIGHT_RETURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(RUN_RIGHT_RETURN, 0, 0.48f, RUN_RIGHT_RETURN, RUN_LEFT_TURN);
				//
				SetTrackAnimationPosition(RUN_RIGHT_RETURN, 0);
				//
				m_pSkinnedAnimationController->m_CurrentTrack = RUN_RIGHT_RETURN;
			}
			else
				return;
		}
		else
		{
			if (m_pSkinnedAnimationController->m_pAnimationTracks[RUN_RIGHT_TURN].m_bEnable)
			{
				length = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[RUN_RIGHT_TURN]->m_fLength;
				m_pSkinnedAnimationController->SetTrackEnable(RUN_RIGHT_RETURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(RUN_RIGHT_RETURN, 0, 0.48f, RUN_RIGHT_RETURN, RUN);
				SetTrackAnimationPosition(RUN_RIGHT_RETURN, length - positionTime);
				m_pSkinnedAnimationController->m_CurrentTrack = RUN_RIGHT_RETURN;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[RUN_RIGHT_TURNING].m_bEnable)
			{
				m_pSkinnedAnimationController->SetTrackEnable(RUN_RIGHT_RETURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(RUN_RIGHT_RETURN, 0, 0.48f, RUN_RIGHT_RETURN, RUN);
				//
				SetTrackAnimationPosition(RUN_RIGHT_RETURN, 0);
				//
				m_pSkinnedAnimationController->m_CurrentTrack = RUN_RIGHT_RETURN;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[RUN_LEFT_RETURN].m_bEnable)
			{
				m_pSkinnedAnimationController->SetCallbackFuncKey(RUN_LEFT_RETURN, 0, 0.48f, RUN_LEFT_RETURN, RUN);
				return;
			}
			else
				return;
		}
	}
	else
	{
		if (isLeft)
		{
			if (m_pSkinnedAnimationController->m_pAnimationTracks[IDLE_RIGHT_TURN].m_bEnable)
			{
				length = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[IDLE_RIGHT_TURN]->m_fLength;
				m_pSkinnedAnimationController->SetTrackEnable(IDLE_RIGHT_RETURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(IDLE_RIGHT_RETURN, 0, 0.48f, IDLE_RIGHT_RETURN, IDLE_LEFT_TURN);
				SetTrackAnimationPosition(IDLE_RIGHT_RETURN, length - positionTime);
				m_pSkinnedAnimationController->m_CurrentTrack = IDLE_RIGHT_RETURN;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[IDLE_LEFT_RETURN].m_bEnable)
			{
				length = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[IDLE_LEFT_RETURN]->m_fLength;
				m_pSkinnedAnimationController->SetTrackEnable(IDLE_LEFT_TURN, true);
				SetTrackAnimationPosition(IDLE_LEFT_TURN, length - positionTime);
				m_pSkinnedAnimationController->m_CurrentTrack = IDLE_LEFT_TURN;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[IDLE_RIGHT_TURNING].m_bEnable)
			{
				m_pSkinnedAnimationController->SetTrackEnable(IDLE_RIGHT_RETURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(IDLE_RIGHT_RETURN, 0, 0.48f, IDLE_RIGHT_RETURN, IDLE_LEFT_TURN);
				//
				SetTrackAnimationPosition(IDLE_RIGHT_RETURN, 0);
				//
				m_pSkinnedAnimationController->m_CurrentTrack = IDLE_RIGHT_RETURN;
			}
			else
				return;
		}
		else
		{
			if (m_pSkinnedAnimationController->m_pAnimationTracks[IDLE_RIGHT_TURN].m_bEnable)
			{
				length = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[IDLE_RIGHT_TURN]->m_fLength;
				m_pSkinnedAnimationController->SetTrackEnable(IDLE_RIGHT_RETURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(IDLE_RIGHT_RETURN, 0, 0.48f, IDLE_RIGHT_RETURN, IDLE);
				SetTrackAnimationPosition(IDLE_RIGHT_RETURN, length - positionTime);
				m_pSkinnedAnimationController->m_CurrentTrack = IDLE_RIGHT_RETURN;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[IDLE_RIGHT_TURNING].m_bEnable)
			{
				m_pSkinnedAnimationController->SetTrackEnable(IDLE_RIGHT_RETURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(IDLE_RIGHT_RETURN, 0, 0.48f, IDLE_RIGHT_RETURN, IDLE);
				//
				SetTrackAnimationPosition(IDLE_RIGHT_RETURN, 0);
				//
				m_pSkinnedAnimationController->m_CurrentTrack = IDLE_RIGHT_RETURN;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[IDLE_LEFT_RETURN].m_bEnable)
			{
				m_pSkinnedAnimationController->SetCallbackFuncKey(IDLE_LEFT_RETURN, 0, 0.48f, IDLE_LEFT_RETURN, IDLE);
				return;
			}
			else
				return;
		}
	}

	m_pSkinnedAnimationController->m_pAnimationTracks[prevCurrent].m_bEnable = false;
	SetTrackAnimationPosition(prevCurrent, 0.0f);

}
void CDinoRunPlayer::KeyUpUp()
{
	isUp = false;
}
void CDinoRunPlayer::KeyUpDown()
{
	isDown = false;
}

void CDinoRunPlayer::returnIdle()
{
	UINT nextTrack = m_pSkinnedAnimationController->m_CurrentTrack - ANIMATIONGAP;

	float positionTime = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[m_pSkinnedAnimationController->m_CurrentTrack]->m_fPosition;

	m_pSkinnedAnimationController->m_pAnimationTracks[m_pSkinnedAnimationController->m_CurrentTrack].m_bEnable = false;
	SetTrackAnimationPosition(m_pSkinnedAnimationController->m_CurrentTrack, 0.0f);

	m_pSkinnedAnimationController->m_CurrentTrack = nextTrack;
	SetTrackAnimationPosition(nextTrack, positionTime);
	isWalking = false;

	m_pSkinnedAnimationController->SetTrackEnable(nextTrack, true);
	switch (nextTrack)
	{
	case IDLE_LEFT_TURN:
	case IDLE_RIGHT_TURN:  //+2
		//m_pSkinnedAnimationController->SetCallbackFuncKey(nextTrack, 0, 0.5f, nextTrack, nextTrack+2);
		break;
	case IDLE_LEFT_RETURN:
		m_pSkinnedAnimationController->SetCallbackFuncKey(IDLE_LEFT_RETURN, 0, 0.48f, IDLE_LEFT_RETURN, IDLE);
		break;
	case IDLE_RIGHT_RETURN:
		m_pSkinnedAnimationController->SetCallbackFuncKey(IDLE_RIGHT_RETURN, 0, 0.48f, IDLE_RIGHT_RETURN, IDLE);
		break;
	}

	if (SoundManager::GetInstance()->Playing("Running"))
	{
		SoundManager::GetInstance()->Stop("Running");
	}
	//m_pSkinnedAnimationController->SetCallbackFuncKey(IDLE_LEFT_RETURN, 0, 0.5f, IDLE_LEFT_RETURN, IDLE);
}

void CDinoRunPlayer::OnSliding()
{
	m_pSkinnedAnimationController->SetTrackEnable(SLIDING, true);
	isStun = true;
	m_fTimeCount = 1.0f;
}
void CDinoRunPlayer::OnCollisionAni()
{
	m_pSkinnedAnimationController->SetTrackEnable(BIG_COLLISION, true);
	isStun = true;
	m_fTimeCount = 1.0f;
}