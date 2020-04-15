//-----------------------------------------------------------------------------
// File: CPlayer.cpp
//-----------------------------------------------------------------------------

#include "PlayerObject.h"
#include "TerrainObject.h"
#include "../CShaders/Shader.h"
#include "../Common/Camera/Camera.h"
#include "../Global/Global.h"
#include "../Meshes/SkyBoxMesh.h"
#include "../Common/ImGui/imgui/imgui.h"
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CPlayer
extern float nScale;
extern int numMesh;
CPlayer::CPlayer()
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

	m_pPlayerUpdatedContext = NULL;
	m_pCameraUpdatedContext = NULL;
}

CPlayer::~CPlayer()
{
	ReleaseShaderVariables();

	if (m_pCamera) delete m_pCamera;
}

void CPlayer::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (m_pCamera) m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CPlayer::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
}

void CPlayer::ReleaseShaderVariables()
{
	if (m_pCamera) m_pCamera->ReleaseShaderVariables();
}

void CPlayer::Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity)
{
	if (dwDirection)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
#ifdef _WITH_LEFT_HAND_COORDINATES
		if (dwDirection & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, +fDistance);
		if (dwDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -fDistance);
#else
		if (dwDirection & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -fDistance);
		if (dwDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, +fDistance);
#endif
#ifdef _WITH_LEFT_HAND_COORDINATES
		if (dwDirection & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, +fDistance);
		if (dwDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -fDistance);
#else
		if (dwDirection & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -fDistance);
		if (dwDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, +fDistance);
#endif
		if (dwDirection & DIR_UP) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, +fDistance);
		if (dwDirection & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, -fDistance);

		Move(xmf3Shift, bUpdateVelocity);
	}
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

void CPlayer::Update(float fTimeElapsed)
{
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, m_xmf3Gravity);
	float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
	float fMaxVelocityXZ = m_fMaxVelocityXZ;
	if (fLength > m_fMaxVelocityXZ)
	{
		m_xmf3Velocity.x *= (fMaxVelocityXZ / fLength);
		m_xmf3Velocity.z *= (fMaxVelocityXZ / fLength);
	}
	float fMaxVelocityY = m_fMaxVelocityY;
	fLength = sqrtf(m_xmf3Velocity.y * m_xmf3Velocity.y);
	if (fLength > m_fMaxVelocityY) m_xmf3Velocity.y *= (fMaxVelocityY / fLength);

	XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed, false);
	Move(xmf3Velocity, false);

	if (m_pCamera->GetMode() == THIRD_PERSON_CAMERA)
	{
		if (m_pPlayerUpdatedContext) OnPlayerUpdateCallback(fTimeElapsed);
	}
	DWORD nCurrentCameraMode = m_pCamera->GetMode();
	if (nCurrentCameraMode == THIRD_PERSON_CAMERA) m_pCamera->Update(m_xmf3Position, fTimeElapsed);
	//if (m_pCameraUpdatedContext) OnCameraUpdateCallback(fTimeElapsed);
	if (nCurrentCameraMode == THIRD_PERSON_CAMERA) m_pCamera->SetLookAt(m_xmf3Position);
	m_pCamera->RegenerateViewMatrix();

	fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = (m_fFriction * fTimeElapsed);
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));
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

void CPlayer::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	DWORD nCameraMode = (pCamera) ? pCamera->GetMode() : 0x00;
	if (nCameraMode == THIRD_PERSON_CAMERA) CGameObject::Render(pd3dCommandList, pCamera);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 1.0f;
		static int counter = 0;
		static double transformValue=0;

		ImGui::Begin("Player Status");                          // Create a window called "Hello, world!" and append into it.
		ImGui::Text("Mesh_Number = %d", numMesh);
		ImGui::SameLine();
		if (ImGui::Button("<", ImVec2(20, 20)))
		{
			numMesh--;
			if (numMesh < 0)
				numMesh = 5;
		}
		ImGui::SameLine();
		if (ImGui::Button(">", ImVec2(20, 20)))
		{
			numMesh = (numMesh + 1) % 6;
		}
		
		ImGui::InputDouble("Transform Value",&transformValue,-100.0,100.0,"%.2f");

		if (ImGui::Button("x", ImVec2(20, 20)))
		{
			Move(DIR_RIGHT, transformValue, true);
		}
		ImGui::SameLine();
		if (ImGui::Button("y", ImVec2(20, 20)))
		{
			m_height += transformValue;
		}
		ImGui::SameLine();
		if (ImGui::Button("z", ImVec2(20, 20)))
		{
			Move(DIR_FORWARD, transformValue, true);
		}
		ImGui::SameLine();
		ImGui::Text("Translate");
	
		
		if (ImGui::Button("Rx", ImVec2(20, 20)))
		{
			Rotate(transformValue, 0, 0);
			Update(0);
		}
		ImGui::SameLine();
		if (ImGui::Button("Ry", ImVec2(20, 20)))
		{
			Rotate(0, transformValue, 0);
			Update(0);
		}
		ImGui::SameLine();
		if (ImGui::Button("Rz", ImVec2(20, 20)))
		{
			Rotate(0, 0, transformValue);
			Update(0);
		}
		ImGui::SameLine();
		ImGui::Text("Rotate");

		ImGui::InputFloat("Scale ValueX", &m_nScale.x, 1.0, 10.0, "%.2f");
		ImGui::InputFloat("Scale ValueY", &m_nScale.y, 1.0, 10.0, "%.2f");
		ImGui::InputFloat("Scale ValueZ", &m_nScale.z, 1.0, 10.0, "%.2f");
		ImGui::InputFloat("height Value", &m_height, -100.0, 100.0, "%.2f");

		ImGui::End();
	}
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

CTerrainPlayer::CTerrainPlayer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext)
{
	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);
	//CLoadedModelInfo *pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Resources/Models/box.bin", NULL);
	//SetChild(pAngrybotModel->m_pModelRootObject, true);
	//m_pSkinnedAnimationController = new CAnimationController(pd3dDevice, pd3dCommandList, 1, pAngrybotModel);
	//m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0); //left_turn_start
	//m_pSkinnedAnimationController->SetTrackEnable(0, true);

	/*
	CLoadedModelInfo *pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Resources/Models/DinoMain.bin", NULL);
	SetChild(pAngrybotModel->m_pModelRootObject, true);

	m_pSkinnedAnimationController = new CAnimationController(pd3dDevice, pd3dCommandList, 8, pAngrybotModel);
	m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0); //left_turn_start
	m_pSkinnedAnimationController->SetTrackEnable(0,false);
	m_pSkinnedAnimationController->SetCallbackFuncKeys(0, 1);

	m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1); //left_return
	m_pSkinnedAnimationController->SetTrackEnable(1, false);
	m_pSkinnedAnimationController->SetCallbackFuncKeys(1, 1);

	m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2); //left_turning
	m_pSkinnedAnimationController->SetTrackEnable(2, false);


	m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3); //right_turn_start
	m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_pSkinnedAnimationController->SetCallbackFuncKeys(3, 1);

	m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4); //right_return
	m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_pSkinnedAnimationController->SetCallbackFuncKeys(4, 1);

	m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5); //right_turning
	m_pSkinnedAnimationController->SetTrackEnable(5, false);


	m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6); //walk
	m_pSkinnedAnimationController->SetTrackEnable(6, false);


	m_pSkinnedAnimationController->SetTrackAnimationSet(7, 7); //run
	m_pSkinnedAnimationController->SetTrackEnable(7, false);

	m_pSkinnedAnimationController->m_CurrentTrack = 6;
	*/
	
	
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
	SetPlayerUpdatedContext(pContext);
	SetCameraUpdatedContext(pContext);

	//if (pAngrybotModel) delete pAngrybotModel;

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	//CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;
	SetPosition(XMFLOAT3(0.0f, 0, 0));

	SetScale(XMFLOAT3(0.01f, 0.01f, 0.01f));
}

CTerrainPlayer::~CTerrainPlayer()
{
}

void CTerrainPlayer::OnPrepareRender()
{
	CPlayer::OnPrepareRender();
	SetScale(m_nScale);
	m_xmf4x4ToParent = Matrix4x4::Multiply(XMMatrixScaling(m_xmf3Scale.x, m_xmf3Scale.y, m_xmf3Scale.z), m_xmf4x4ToParent);
	//m_xmf4x4ToParent = Matrix4x4::Multiply(XMMatrixRotationX(0.0f), m_xmf4x4ToParent);
}

CCamera *CTerrainPlayer::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
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
			m_pCamera->SetOffset(XMFLOAT3(0.0f, 0.0f, +1.0f));
			m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
			m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
			m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
			break;
		case THIRD_PERSON_CAMERA:
			SetFriction(250.0f);
			SetGravity(XMFLOAT3(0.0f, -0.0f, 0.0f));
			SetMaxVelocityXZ(300.0f);
			SetMaxVelocityY(400.0f);
			m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
			m_pCamera->SetTimeLag(0.25f);
			m_pCamera->SetOffset(XMFLOAT3(0.0f,200.0f, -250.0f));
			m_pCamera->SetPosition(Vector3::Add(m_xmf3Position, m_pCamera->GetOffset()));
			m_pCamera->GenerateProjectionMatrix(1.01f, 10000.0f, ASPECT_RATIO, 60.0f);
			m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
			m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
			break;
		default:
			break;
	}
	Update(fTimeElapsed);

	return(m_pCamera);
}

void CTerrainPlayer::OnPlayerUpdateCallback(float fTimeElapsed)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)m_pPlayerUpdatedContext;
	XMFLOAT3 xmf3Scale = pTerrain->GetScale();
	XMFLOAT3 xmf3PlayerPosition = GetPosition();
	int z = (int)(xmf3PlayerPosition.z / xmf3Scale.z);
	bool bReverseQuad = ((z % 2) != 0);
	float fHeight = pTerrain->GetHeight(xmf3PlayerPosition.x, 256 * xmf3Scale.z - xmf3PlayerPosition.z);
	

	//if (xmf3PlayerPosition.y < fHeight)
	{
		//XMFLOAT3 xmf3PlayerVelocity = GetVelocity();
		//xmf3PlayerVelocity.y = 0.0f;
		//SetVelocity(xmf3PlayerVelocity);
		xmf3PlayerPosition.y = fHeight + m_height;		
		SetPosition(xmf3PlayerPosition);
	}
}

void CTerrainPlayer::OnCameraUpdateCallback(float fTimeElapsed)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)m_pCameraUpdatedContext;
	XMFLOAT3 xmf3Scale = pTerrain->GetScale();
	XMFLOAT3 xmf3CameraPosition = m_pCamera->GetPosition();
	int z = (int)(xmf3CameraPosition.z / xmf3Scale.z);
	bool bReverseQuad = ((z % 2) != 0);
	float fHeight = 300;
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


void CTerrainPlayer::KeyDownLeft()
{
	if (!m_pSkinnedAnimationController)
		return;
	if (isLeft)
		return;

	isLeft = true;
	int prevCurrent = m_pSkinnedAnimationController->m_CurrentTrack;
	float positionTime = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[m_pSkinnedAnimationController->m_CurrentTrack]->m_fPosition;

	float length = 0.0f;
	if (isWalking)
	{
		if (isRight)
		{
			if (m_pSkinnedAnimationController->m_pAnimationTracks[RIGHT_TURNING].m_bEnable)
			{
				m_pSkinnedAnimationController->SetTrackEnable(RIGHT_RETURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(RIGHT_RETURN, 0, 0.82f, RIGHT_RETURN, WALKING);
				m_pSkinnedAnimationController->m_CurrentTrack = RIGHT_RETURN;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[RIGHT_TURN].m_bEnable)
			{
				length = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[RIGHT_RETURN]->m_fLength;
				m_pSkinnedAnimationController->SetTrackEnable(RIGHT_RETURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(RIGHT_RETURN, 0, 0.82f, RIGHT_RETURN, WALKING);
				SetTrackAnimationPosition(RIGHT_RETURN, length - positionTime);
				m_pSkinnedAnimationController->m_CurrentTrack = RIGHT_RETURN;
			}
		}
		else
		{
			if (m_pSkinnedAnimationController->m_pAnimationTracks[LEFT_RETURN].m_bEnable)
			{
				length = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[LEFT_RETURN]->m_fLength;
				m_pSkinnedAnimationController->SetTrackEnable(LEFT_TURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(LEFT_TURN, 0, 0.82f, LEFT_TURN, LEFT_TURNING);
				SetTrackAnimationPosition(LEFT_TURN, length - positionTime);
				m_pSkinnedAnimationController->m_CurrentTrack = LEFT_TURN;
			}
			else
			{
				m_pSkinnedAnimationController->SetTrackEnable(LEFT_TURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(LEFT_TURN, 0, 0.80f, LEFT_TURN, LEFT_TURNING);
				m_pSkinnedAnimationController->m_CurrentTrack = LEFT_TURN;
			}
		}
	}
	else
	{

	}
	m_pSkinnedAnimationController->m_pAnimationTracks[prevCurrent].m_bEnable = false;
	SetTrackAnimationPosition(prevCurrent, 0.0f);
}
void CTerrainPlayer::KeyDownRight()
{
	if (!m_pSkinnedAnimationController)
		return;
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
			if (m_pSkinnedAnimationController->m_pAnimationTracks[LEFT_TURNING].m_bEnable)
			{
				m_pSkinnedAnimationController->SetTrackEnable(LEFT_RETURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(LEFT_RETURN, 0, 0.82f, LEFT_RETURN, WALKING);
				m_pSkinnedAnimationController->m_CurrentTrack = LEFT_RETURN;
				SetTrackAnimationPosition(LEFT_RETURN, 0.0f);
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[LEFT_TURN].m_bEnable)
			{
				length = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[LEFT_RETURN]->m_fLength;
				m_pSkinnedAnimationController->SetTrackEnable(LEFT_RETURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(LEFT_RETURN, 0, 0.82f, LEFT_RETURN, WALKING);
				SetTrackAnimationPosition(LEFT_RETURN, length - positionTime);
				m_pSkinnedAnimationController->m_CurrentTrack = LEFT_RETURN;
			}
		}
		else
		{
			if (m_pSkinnedAnimationController->m_pAnimationTracks[RIGHT_RETURN].m_bEnable)
			{
				length = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[RIGHT_RETURN]->m_fLength;
				m_pSkinnedAnimationController->SetTrackEnable(RIGHT_TURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(RIGHT_TURN, 0, 0.82f, RIGHT_TURN, RIGHT_TURNING);
				SetTrackAnimationPosition(RIGHT_TURN, length - positionTime);
				m_pSkinnedAnimationController->m_CurrentTrack = RIGHT_TURN;
			}
			else
			{
				m_pSkinnedAnimationController->SetTrackEnable(RIGHT_TURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(RIGHT_TURN, 0, 0.8f, RIGHT_TURN, RIGHT_TURNING);
				m_pSkinnedAnimationController->m_CurrentTrack = RIGHT_TURN;
				SetTrackAnimationPosition(RIGHT_TURN, 0.0f);
			}
		}
	}
	else
	{

	}
	m_pSkinnedAnimationController->m_pAnimationTracks[prevCurrent].m_bEnable = false;
	SetTrackAnimationPosition(prevCurrent, 0.0f);
}
void CTerrainPlayer::KeyDownUp()
{

}
void CTerrainPlayer::KeyDownDonw()
{

}
void CTerrainPlayer::KeyUpLeft()
{
	if (!m_pSkinnedAnimationController)
		return;
	if (!isLeft)
		return;
	isLeft = false;
	int prevCurrent = m_pSkinnedAnimationController->m_CurrentTrack;
	float positionTime = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[m_pSkinnedAnimationController->m_CurrentTrack]->m_fPosition;
	//ㄴ> 트랙으로 주는게 잘못됐음. set의 포지션타임이 필요
	float length = 0.0f;
	if (isWalking)
	{
		if (isRight)
		{
			if (m_pSkinnedAnimationController->m_pAnimationTracks[WALKING].m_bEnable)
			{
				m_pSkinnedAnimationController->SetTrackEnable(RIGHT_TURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(RIGHT_TURN, 0, 0.82f, RIGHT_TURN, RIGHT_TURNING);
				m_pSkinnedAnimationController->m_CurrentTrack = RIGHT_TURN;
				SetTrackAnimationPosition(RIGHT_TURN, 0.0f);
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[RIGHT_RETURN].m_bEnable)
			{
				length = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[RIGHT_RETURN]->m_fLength;
				m_pSkinnedAnimationController->SetTrackEnable(RIGHT_TURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(RIGHT_TURN, 0, 0.82f, RIGHT_TURN, RIGHT_TURNING);
				SetTrackAnimationPosition(RIGHT_TURN, length - positionTime);
				m_pSkinnedAnimationController->m_CurrentTrack = RIGHT_TURN;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[LEFT_RETURN].m_bEnable)
			{
				m_pSkinnedAnimationController->SetCallbackFuncKey(LEFT_RETURN, 0, 0.82f, LEFT_RETURN, RIGHT_TURN);
				m_pSkinnedAnimationController->SetCallbackFuncKey(RIGHT_TURN, 0, 0.82f, RIGHT_TURN, RIGHT_TURNING);
				return;
			}
		}
		else
		{
			if (m_pSkinnedAnimationController->m_pAnimationTracks[LEFT_TURN].m_bEnable)
			{
				length = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[LEFT_TURN]->m_fLength;
				m_pSkinnedAnimationController->SetTrackEnable(LEFT_RETURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(LEFT_RETURN, 0, 0.82f, LEFT_RETURN, WALKING);
				SetTrackAnimationPosition(LEFT_RETURN, length - positionTime);
				m_pSkinnedAnimationController->m_CurrentTrack = LEFT_RETURN;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[LEFT_TURNING].m_bEnable)
			{
				m_pSkinnedAnimationController->SetTrackEnable(LEFT_RETURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(LEFT_RETURN, 0, 0.82f, LEFT_RETURN, WALKING);
				m_pSkinnedAnimationController->m_CurrentTrack = LEFT_RETURN;
				SetTrackAnimationPosition(LEFT_RETURN, 0.0f);
			}
			
		}
	}
	else
	{

	}
	m_pSkinnedAnimationController->m_pAnimationTracks[prevCurrent].m_bEnable = false;
	SetTrackAnimationPosition(prevCurrent, 0.0f);
}
void CTerrainPlayer::KeyUpRight()
{
	if (!m_pSkinnedAnimationController)
		return;
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
			if (m_pSkinnedAnimationController->m_pAnimationTracks[WALKING].m_bEnable)
			{
				m_pSkinnedAnimationController->SetTrackEnable(LEFT_TURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(LEFT_TURN, 0, 0.82f, LEFT_TURN, LEFT_TURNING);
				m_pSkinnedAnimationController->m_CurrentTrack = LEFT_TURN;
				SetTrackAnimationPosition(LEFT_TURN, 0.0f);
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[LEFT_RETURN].m_bEnable)
			{
				length = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[LEFT_RETURN]->m_fLength;
				m_pSkinnedAnimationController->SetTrackEnable(LEFT_TURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(LEFT_TURN, 0, 0.82f, LEFT_TURN, LEFT_TURNING);
				SetTrackAnimationPosition(LEFT_TURN, length - positionTime);
				m_pSkinnedAnimationController->m_CurrentTrack = LEFT_TURN;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[RIGHT_RETURN].m_bEnable)
			{
				m_pSkinnedAnimationController->SetCallbackFuncKey(RIGHT_RETURN, 0, 0.82f, RIGHT_RETURN, LEFT_TURN);
				m_pSkinnedAnimationController->SetCallbackFuncKey(LEFT_TURN, 0, 0.82f, LEFT_TURN, LEFT_TURNING);
				return;
			}
		}
		else
		{
			if (m_pSkinnedAnimationController->m_pAnimationTracks[RIGHT_TURN].m_bEnable)
			{
				length = m_pSkinnedAnimationController->m_pAnimationSets->m_ppAnimationSets[RIGHT_TURN]->m_fLength;
				m_pSkinnedAnimationController->SetTrackEnable(RIGHT_RETURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(RIGHT_RETURN, 0, 0.82f, RIGHT_RETURN, WALKING);
				SetTrackAnimationPosition(RIGHT_RETURN, length - positionTime);
				m_pSkinnedAnimationController->m_CurrentTrack = RIGHT_RETURN;
			}
			else if (m_pSkinnedAnimationController->m_pAnimationTracks[RIGHT_TURNING].m_bEnable)
			{
				m_pSkinnedAnimationController->SetTrackEnable(RIGHT_RETURN, true);
				m_pSkinnedAnimationController->SetCallbackFuncKey(RIGHT_RETURN, 0, 0.82f, RIGHT_RETURN, WALKING);
				m_pSkinnedAnimationController->m_CurrentTrack = RIGHT_RETURN;
				SetTrackAnimationPosition(RIGHT_RETURN, 0.0f);
			}
		}
	}
	else
	{

	}

	m_pSkinnedAnimationController->m_pAnimationTracks[prevCurrent].m_bEnable = false;
	SetTrackAnimationPosition(prevCurrent, 0.0f);

}
void CTerrainPlayer::KeyUpUp()
{

}
void CTerrainPlayer::KeyUpDown()
{

}