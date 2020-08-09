#include "Camera.h"

CCamera::CCamera()
{
	m_xmf4x4View = Matrix4x4::Identity();
	m_xmf4x4Projection = Matrix4x4::Identity();
	m_d3dViewport = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT, 0.0f, 1.0f };
	m_d3dScissorRect = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT };
	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;
	m_xmf3Offset = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fTimeLag = 0.0f;
	m_xmf3LookAtWorld = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_nMode = 0x00;
	m_pPlayer = NULL;
}

CCamera::CCamera(CCamera *pCamera)
{
	if (pCamera)
	{
		//ī�޶� �̹� ������ ���� ī�޶��� ������ ���ο� ī�޶� �����Ѵ�. 
		*this = *pCamera;
	}
	else
	{
		//ī�޶� ������ �⺻ ������ �����Ѵ�. 
		m_xmf4x4View = Matrix4x4::Identity();
		m_xmf4x4Projection = Matrix4x4::Identity();
		m_d3dViewport = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT, 0.0f, 1.0f };
		m_d3dScissorRect = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT };
		m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
		m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
		m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
		m_fPitch = 0.0f;
		m_fRoll = 0.0f;
		m_fYaw = 0.0f;
		m_xmf3Offset = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_fTimeLag = 0.0f;
		m_xmf3LookAtWorld = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_nMode = 0x00;
		m_pPlayer = NULL;
	}
}

CCamera::~CCamera()
{
}

void CCamera::SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float
	fMinZ, float fMaxZ)
{
	m_d3dViewport.TopLeftX = float(xTopLeft);
	m_d3dViewport.TopLeftY = float(yTopLeft);
	m_d3dViewport.Width = float(nWidth);
	m_d3dViewport.Height = float(nHeight);
	m_d3dViewport.MinDepth = fMinZ;
	m_d3dViewport.MaxDepth = fMaxZ;
}
void CCamera::SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom)
{
	m_d3dScissorRect.left = xLeft;
	m_d3dScissorRect.top = yTop;
	m_d3dScissorRect.right = xRight;
	m_d3dScissorRect.bottom = yBottom;
}

void CCamera::GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance,
	float fAspectRatio, float fFOVAngle)
{
#ifdef _WITH_LEFT_HAND_COORDINATES
	m_xmf4x4Projection = Matrix4x4::PerspectiveFovLH(XMConvertToRadians(fFOVAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
#else
	m_xmf4x4Projection = Matrix4x4::PerspectiveFovRH(XMConvertToRadians(fFOVAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
#endif
	m_fFarDistance = fFarPlaneDistance;
	m_fNearDistance = fNearPlaneDistance;
	m_fFovAngle = fFOVAngle;
}
void CCamera::ReGenerateProjectionMatrix(float fAspectRatio)
{
	GenerateProjectionMatrix(m_fNearDistance, m_fFarDistance, fAspectRatio, m_fFovAngle);
}
void CCamera::GenerateOrthoProjectionMatrix(float ViewWidth, float ViewHeight,
	float fNear, float fFar)
{
#ifdef _WITH_LEFT_HAND_COORDINATES
	m_xmf4x4Projection = Matrix4x4::OrthoLH(ViewWidth, ViewHeight, fNear, fFar);
#else
	m_xmf4x4Projection = Matrix4x4::OrthoRH(ViewWidth, ViewHeight, fNear, fFar);
#endif
}

void CCamera::GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3
	xmf3Up)
{
	m_xmf3Position = xmf3Position;
	m_xmf3LookAtWorld = xmf3LookAt;
	m_xmf3Up = xmf3Up;
	GenerateViewMatrix();
}

void CCamera::GenerateViewMatrix()
{
#ifdef _WITH_LEFT_HAND_COORDINATES
	m_xmf4x4View = Matrix4x4::LookAtLH(m_xmf3Position, m_xmf3LookAtWorld, m_xmf3Up);
#else
	m_xmf4x4View = Matrix4x4::LookAtRH(m_xmf3Position, m_xmf3LookAtWorld, m_xmf3Up);
#endif
	m_xmf4x4PrevView = m_xmf4x4View;
}
void CCamera::RegenerateViewMatrix()
{
	//ī�޶��� z-���� �������� ī�޶��� ��ǥ����� �����ϵ��� ī�޶� ��ȯ ����� �����Ѵ�. 
	//ī�޶��� z-�� ���͸� ����ȭ�Ѵ�. 

	m_xmf4x4PrevView = m_xmf4x4View;
	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	//ī�޶��� z-��� y-�࿡ ������ ���͸� x-������ �����Ѵ�.
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	//ī�޶��� z-��� x-�࿡ ������ ���͸� y-������ �����Ѵ�.
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
	m_xmf4x4View._11 = m_xmf3Right.x; m_xmf4x4View._12 = m_xmf3Up.x; m_xmf4x4View._13 =
		m_xmf3Look.x;
	m_xmf4x4View._21 = m_xmf3Right.y; m_xmf4x4View._22 = m_xmf3Up.y; m_xmf4x4View._23 =
		m_xmf3Look.y;
	m_xmf4x4View._31 = m_xmf3Right.z; m_xmf4x4View._32 = m_xmf3Up.z; m_xmf4x4View._33 =
		m_xmf3Look.z;
	m_xmf4x4View._41 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Right);
	m_xmf4x4View._42 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Up);
	m_xmf4x4View._43 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Look);
	GenerateFrustum();
}
bool CCamera::IsInFrustum(BoundingOrientedBox& xmBoundingBox)
{
	return(m_xmFrustum.Intersects(xmBoundingBox));
}

void CCamera::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList)
{
	UINT ncbGameObjectBytes = ((sizeof(VS_CB_CAMERA_INFO) + 255) & ~255); //256�� ���
	m_pd3dcbCamera = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL,
		ncbGameObjectBytes, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbCamera->Map(0, NULL, (void **)&m_pcbMappedCamera);
}

void CCamera::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4View;
	XMStoreFloat4x4(&xmf4x4View, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4View)));
	//��Ʈ �Ķ���� �ε��� 1��

	XMFLOAT4X4 xmf4x4Projection;
	XMStoreFloat4x4(&xmf4x4Projection,
		XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4Projection)));
	::memcpy(&m_pcbMappedCamera->m_xmf4x4View, &xmf4x4View, sizeof(XMFLOAT4X4));
	::memcpy(&m_pcbMappedCamera->m_xmf4x4Projection, &xmf4x4Projection, sizeof(XMFLOAT4X4));

	XMFLOAT4X4 xmf4x4PrevView;
	XMStoreFloat4x4(&xmf4x4PrevView, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4PrevView)));
	::memcpy(&m_pcbMappedCamera->m_xmf4x4PrevView, &xmf4x4PrevView, sizeof(XMFLOAT4X4));


	m_pcbMappedCamera->m_cameraPosition = GetPosition();


	D3D12_GPU_VIRTUAL_ADDRESS d3dcbGameObjectGpuVirtualAddress =
		m_pd3dcbCamera->GetGPUVirtualAddress();

	pd3dCommandList->SetGraphicsRootConstantBufferView(0,
		d3dcbGameObjectGpuVirtualAddress);


}

void CCamera::ReleaseShaderVariables()
{
	if (m_pd3dcbCamera)
	{
		m_pd3dcbCamera->Unmap(0, NULL);
		m_pd3dcbCamera->Release();
		m_pd3dcbCamera = NULL;
	}
}

void CCamera::SetViewportsAndScissorRects(ID3D12GraphicsCommandList *pd3dCommandList)
{
	pd3dCommandList->RSSetViewports(1, &m_d3dViewport);
	pd3dCommandList->RSSetScissorRects(1, &m_d3dScissorRect);
}

void CCamera::GenerateFrustum()
{
	//���� ���� ��ȯ ��Ŀ��� ����ü�� �����Ѵ�(����ü�� ī�޶� ��ǥ��� ǥ���ȴ�).
	m_xmFrustum.CreateFromMatrix(m_xmFrustum, XMLoadFloat4x4(&m_xmf4x4Projection));
	//ī�޶� ��ȯ ����� ������� ���Ѵ�.
	XMMATRIX xmmtxInversView = XMMatrixInverse(NULL, XMLoadFloat4x4(&m_xmf4x4View));
	//����ü�� ī�޶� ��ȯ ����� ����ķ� ��ȯ�Ѵ�(���� ����ü�� ���� ��ǥ��� ǥ���ȴ�).
	m_xmFrustum.Transform(m_xmFrustum, xmmtxInversView);
}


CSpaceShipCamera::CSpaceShipCamera(CCamera *pCamera) : CCamera(pCamera)
{
	m_nMode = SPACESHIP_CAMERA;
}

//�����̽�-�� ī�޶� �÷��̾��� ���� x-��(Right), y-��(Up), z-��(Look)�� �������� ȸ���ϴ� �Լ��̴�. 
void CSpaceShipCamera::Rotate(float x, float y, float z)
{
	if (m_pPlayer && (x != 0.0f))
	{
		//�÷��̾��� ���� x-�࿡ ���� x ������ ȸ�� ����� ����Ѵ�. 
		XMFLOAT3 xmf3Right = m_pPlayer->GetRight();
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Right),
			XMConvertToRadians(x));
		//ī�޶��� ���� x-��, y-��, z-���� ȸ���Ѵ�. 
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);

		m_xmf3Position = Vector3::Subtract(m_xmf3Position, m_pPlayer->GetPosition());
		//�÷��̾��� ��ġ�� �߽����� ī�޶��� ��ġ ����(�÷��̾ �������� ��)�� ȸ���Ѵ�. 
		m_xmf3Position = Vector3::TransformCoord(m_xmf3Position, xmmtxRotate);
		//ȸ����Ų ī�޶��� ��ġ ���Ϳ� �÷��̾��� ��ġ�� ���Ͽ� ī�޶��� ��ġ ���͸� ���Ѵ�. 
		m_xmf3Position = Vector3::Add(m_xmf3Position, m_pPlayer->GetPosition());
	}

	if (m_pPlayer && (y != 0.0f))
	{
		XMFLOAT3 xmf3Up = m_pPlayer->GetUp();
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up),
			XMConvertToRadians(y));
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Position = Vector3::Subtract(m_xmf3Position, m_pPlayer->GetPosition());
		m_xmf3Position = Vector3::TransformCoord(m_xmf3Position, xmmtxRotate);
		m_xmf3Position = Vector3::Add(m_xmf3Position, m_pPlayer->GetPosition());
	}
	if (m_pPlayer && (z != 0.0f))
	{
		XMFLOAT3 xmf3Look = m_pPlayer->GetLook();
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Look),
			XMConvertToRadians(z));
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Position = Vector3::Subtract(m_xmf3Position, m_pPlayer->GetPosition());
		m_xmf3Position = Vector3::TransformCoord(m_xmf3Position, xmmtxRotate);
		m_xmf3Position = Vector3::Add(m_xmf3Position, m_pPlayer->GetPosition());
	}
}

CFirstPersonCamera::CFirstPersonCamera(CCamera *pCamera) : CCamera(pCamera)
{
	m_nMode = FIRST_PERSON_CAMERA;
	if (pCamera)
	{

		if (pCamera->GetMode() == SPACESHIP_CAMERA)
		{
			m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
			m_xmf3Right.y = 0.0f;
			m_xmf3Look.y = 0.0f;
			m_xmf3Right = Vector3::Normalize(m_xmf3Right);
			m_xmf3Look = Vector3::Normalize(m_xmf3Look);
		}
	}
}

void CFirstPersonCamera::Rotate(float x, float y, float z)
{
	if (x != 0.0f)
	{
		//ī�޶��� ���� x-���� �������� ȸ���ϴ� ����� �����Ѵ�. ����� ��� ���� �����̴� �����̴�.
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right),
			XMConvertToRadians(x));
		//ī�޶��� ���� x-��, y-��, z-���� ȸ�� ����� ����Ͽ� ȸ���Ѵ�.
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
	}
	if (m_pPlayer && (y != 0.0f))
	{
		//�÷��̾��� ���� y-���� �������� ȸ���ϴ� ����� �����Ѵ�. 
		XMFLOAT3 xmf3Up = m_pPlayer->GetUp();
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up),
			XMConvertToRadians(y));
		//ī�޶��� ���� x-��, y-��, z-���� ȸ�� ����� ����Ͽ� ȸ���Ѵ�.
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
	}
	if (m_pPlayer && (z != 0.0f))
	{
		//�÷��̾��� ���� z-���� �������� ȸ���ϴ� ����� �����Ѵ�. 
		XMFLOAT3 xmf3Look = m_pPlayer->GetLook();
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Look),
			XMConvertToRadians(z));
		//ī�޶��� ��ġ ���͸� �÷��̾� ��ǥ��� ǥ���Ѵ�(������ ����).
		m_xmf3Position = Vector3::Subtract(m_xmf3Position, m_pPlayer->GetPosition());
		//������ ���� ���͸� ȸ���Ѵ�.
		m_xmf3Position = Vector3::TransformCoord(m_xmf3Position, xmmtxRotate);
		//ȸ���� ī�޶��� ��ġ�� ���� ��ǥ��� ǥ���Ѵ�. 
		m_xmf3Position = Vector3::Add(m_xmf3Position, m_pPlayer->GetPosition());
		//ī�޶��� ���� x-��, y-��, z-���� ȸ���Ѵ�.
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
	}
}

CThirdPersonCamera::CThirdPersonCamera(CCamera *pCamera) : CCamera(pCamera)
{
	m_fMass = 4.0f; //������ 1�̾���

	m_nMode = THIRD_PERSON_CAMERA;
	if (pCamera)
	{

		if (pCamera->GetMode() == SPACESHIP_CAMERA)
		{
			m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
			m_xmf3Right.y = 0.0f;
			m_xmf3Look.y = 0.0f;
			m_xmf3Right = Vector3::Normalize(m_xmf3Right);
			m_xmf3Look = Vector3::Normalize(m_xmf3Look);
		}
	}
}

void CThirdPersonCamera::Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed)
{
	//�÷��̾ ������ �÷��̾��� ȸ���� ���� 3��Ī ī�޶� ȸ���ؾ� �Ѵ�. 
	if (m_pPlayer)
	{
		XMFLOAT4X4 xmf4x4Rotate = Matrix4x4::Identity();

		XMFLOAT3 xmf3Right1 = m_pPlayer->GetRight();
		XMFLOAT3 xmf3Up1 = m_pPlayer->GetUp();
		XMFLOAT3 xmf3Look1 = m_pPlayer->GetLook();

		XMFLOAT3 xmf3Right = m_pPlayer->GetRight();
		XMFLOAT3 xmf3Up = m_pPlayer->GetUp();
		XMFLOAT3 xmf3Look = m_pPlayer->GetLook();

		//�÷��̾��� ���� x-��, y-��, z-�� ���ͷκ��� ȸ�� ���(�÷��̾�� ���� ������ ��Ÿ���� ���)�� �����Ѵ�. 
		xmf4x4Rotate._11 = xmf3Right.x; xmf4x4Rotate._21 = xmf3Up.x; xmf4x4Rotate._31 =
			xmf3Look.x;
		xmf4x4Rotate._12 = xmf3Right.y; xmf4x4Rotate._22 = xmf3Up.y; xmf4x4Rotate._32 =
			xmf3Look.y;
		xmf4x4Rotate._13 = xmf3Right.z; xmf4x4Rotate._23 = xmf3Up.z; xmf4x4Rotate._33 =
			xmf3Look.z;
		//ī�޶� ������ ���͸� ȸ�� ��ķ� ��ȯ(ȸ��)�Ѵ�. 
		XMFLOAT3 xmf3Offset = Vector3::TransformCoord(m_xmf3Offset, xmf4x4Rotate);
		//ȸ���� ī�޶��� ��ġ�� �÷��̾��� ��ġ�� ȸ���� ī�޶� ������ ���͸� ���� ���̴�.
		XMFLOAT3 xmf3Position = Vector3::Add(m_pPlayer->GetPosition(), xmf3Offset);
		//������ ī�޶��� ��ġ���� ȸ���� ī�޶��� ��ġ������ ����� �Ÿ��� ��Ÿ���� �����̴�. 
		XMFLOAT3 xmf3Direction = Vector3::Subtract(xmf3Position, m_xmf3Position);

		//--------------------------------
		float c = 3.0f, k = 20.0f;
		float cXZ = 1.0f, kXZ = 17.5f;
		{
			//k = 0.04  ,  c = 0.2
			//f= - cv -k*x   -cv -kx�ƴѰ�?..
			//c^2 = 4mk  ����  c=1.5   k = -30
			//c^3 > 4mk -> overdamping
			//c^2 = 4mk -> critical damping
			//c^2 < 4mk ->under damping

			//x,y,z�� ���� ���� ���� ���� ���� ��.

			//�۾����� ���� ������
			//xmf3Position-> ���ο� ī�޶� ��ġ
			XMFLOAT3 vel = m_pPlayer->GetVelocity();
			float length = (vel.x * vel.x + vel.z * vel.z);
			if (length > 900)
				m_xmf3Offset = XMFLOAT3(0.0f, 35.0f, -80.0f);
			else
				m_xmf3Offset = XMFLOAT3(0.0f, 30.0f, -60.0f);

			XMFLOAT4X4 viewM = Matrix4x4::LookAtLH(xmf3Position, m_pPlayer->GetPosition(), xmf3Up1);

			xmf3Right = XMFLOAT3(viewM._11, viewM._21, viewM._31);
			xmf3Look = XMFLOAT3(viewM._13, viewM._23, viewM._33);
			xmf3Up = XMFLOAT3(viewM._12, viewM._22, viewM._32);

			xmf3Look = Vector3::Normalize(xmf3Look);

			xmf3Right = Vector3::CrossProduct(xmf3Up, xmf3Look, true);
			//ī�޶��� z-��� x-�࿡ ������ ���͸� y-������ �����Ѵ�.
			xmf3Up = Vector3::CrossProduct(xmf3Look, xmf3Right, true);
			viewM._11 = xmf3Right.x; viewM._12 = xmf3Up.x; viewM._13 =
				xmf3Look.x;
			viewM._21 = xmf3Right.y; viewM._22 = xmf3Up.y; viewM._23 =
				xmf3Look.y;
			viewM._31 = xmf3Right.z; viewM._32 = xmf3Up.z; viewM._33 =
				xmf3Look.z;
			viewM._41 = -Vector3::DotProduct(xmf3Position, xmf3Right);
			viewM._42 = -Vector3::DotProduct(xmf3Position, xmf3Up);
			viewM._43 = -Vector3::DotProduct(xmf3Position, xmf3Look);


			XMFLOAT4X4 inverseViewM;
			inverseViewM._11 = xmf3Right.x; inverseViewM._12 = xmf3Right.y; inverseViewM._13 =
				xmf3Right.z; inverseViewM._14 = 0;
			inverseViewM._21 = xmf3Up.x; inverseViewM._22 = xmf3Up.y; inverseViewM._23 =
				xmf3Up.z; inverseViewM._24 = 0;
			inverseViewM._31 = xmf3Look.x; inverseViewM._32 = xmf3Look.y; inverseViewM._33 =
				xmf3Look.z; inverseViewM._34 = 0;
			inverseViewM._41 = xmf3Position.x;
			inverseViewM._42 = xmf3Position.y;
			inverseViewM._43 = xmf3Position.z;
			inverseViewM._44 = 1;
			XMFLOAT3 xmf3ResultVec;
			XMFLOAT3 xmf3ResultVel;

			XMFLOAT4 xmf4Position(m_xmf3Position.x, m_xmf3Position.y, m_xmf3Position.z, 1);
			XMFLOAT4 xmf4Velocity(m_xmf3Velocity.x, m_xmf3Velocity.y, m_xmf3Velocity.z, 0);
			XMVECTOR xmvResultVec = XMVector3TransformCoord(XMLoadFloat4(&xmf4Position), XMLoadFloat4x4(&viewM));

			XMVECTOR xmVResultVel = XMVector4Transform(XMLoadFloat4(&xmf4Velocity), XMLoadFloat4x4(&viewM));

			XMStoreFloat3(&xmf3ResultVec, xmvResultVec);
			XMStoreFloat3(&xmf3ResultVel, xmVResultVel);

			float forceX = (-cXZ * xmf3ResultVel.x) + (-kXZ * xmf3ResultVec.x);
			float forceY = (-c * xmf3ResultVel.y) + (-k * xmf3ResultVec.y);
			float forceZ = (-cXZ * xmf3ResultVel.z) + (-kXZ * xmf3ResultVec.z);

			XMFLOAT4 xmf4Force = XMFLOAT4(forceX, forceY, forceZ, 0);

			//xmf4Force = Vector3::DivProduct
			XMFLOAT3 accelerationForce;

			XMStoreFloat3(&accelerationForce, XMVector4Transform(XMLoadFloat4(&xmf4Force), XMLoadFloat4x4(&inverseViewM)));

			m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, accelerationForce, fTimeElapsed);

			m_xmf3Position = Vector3::Add(m_xmf3Position, m_xmf3Velocity, fTimeElapsed);

		}
		//---------------------------------
		/*
		xmf3Position = Vector3::Subtract(xmf3Position, m_pPlayer->GetPosition());
		m_xmf3Position = Vector3::Subtract(m_xmf3Position, m_pPlayer->GetPosition());
		XMFLOAT3 p1, p2;

		p1 = m_xmf3Position;
		p2 = xmf3Position;

		p1 = Vector3::Normalize(p1);
		p2 = Vector3::Normalize(p2);
		XMFLOAT3 crossProduct = Vector3::CrossProduct(p1, p2,false);
		float angle = Vector3::DotProduct(p1, p2);
		angle = (angle > 0.0f) ? XMConvertToDegrees(acosf(angle)) : 90.0f;
		angle *= (crossProduct.y > 0.0f) ? 1.0f : -1.0f;

		if (isnan(angle))
		{
			angle = 0;
		}
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up),
			XMConvertToRadians(angle));

		//m_xmf3Position = Vector3::Subtract(m_xmf3Position, m_pPlayer->GetPosition());
		//�÷��̾��� ��ġ�� �߽����� ī�޶��� ��ġ ����(�÷��̾ �������� ��)�� ȸ���Ѵ�.
		m_xmf3Position = Vector3::TransformCoord(m_xmf3Position, xmmtxRotate);
		//ȸ����Ų ī�޶��� ��ġ ���Ϳ� �÷��̾��� ��ġ�� ���Ͽ� ī�޶��� ��ġ ���͸� ���Ѵ�.
		m_xmf3Position = Vector3::Add(m_xmf3Position, m_pPlayer->GetPosition());
	*/
		SetLookAt(xmf3LookAt);
	}
}

void CThirdPersonCamera::SetLookAt(const XMFLOAT3& xmf3LookAt)
{
	//���� ī�޶��� ��ġ���� �÷��̾ �ٶ󺸱� ���� ī�޶� ��ȯ ����� �����Ѵ�. 
	XMFLOAT3 xmf3PlayerUp = m_pPlayer->GetUp();
	//XMFLOAT4X4 mtxLookAt = Matrix4x4::LookAtRH(m_xmf3Position, xmf3LookAt, m_pPlayer->GetUpVector());
#ifdef _WITH_LEFT_HAND_COORDINATES
	XMFLOAT4X4 mtxLookAt = Matrix4x4::LookAtLH(m_xmf3Position, xmf3LookAt, xmf3PlayerUp);
#else
	XMFLOAT4X4 mtxLookAt = Matrix4x4::LookAtRH(m_xmf3Position, xmf3LookAt, xmf3PlayerUp);
#endif
	//ī�޶� ��ȯ ��Ŀ��� ī�޶��� x-��, y-��, z-���� ���Ѵ�. 
	m_xmf3Right = XMFLOAT3(mtxLookAt._11, mtxLookAt._21, mtxLookAt._31);
	m_xmf3Up = XMFLOAT3(mtxLookAt._12, mtxLookAt._22, mtxLookAt._32);
	m_xmf3Look = XMFLOAT3(mtxLookAt._13, mtxLookAt._23, mtxLookAt._33);
}



CMinimapCamera::CMinimapCamera() : CCamera()
{
	m_xmf3Up = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, -1.0f, 0.0f);
	m_nMode = MiniMap_CAMERA;
}

void CMinimapCamera::Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed)
{
}

void CMinimapCamera::SetLookAt(const XMFLOAT3& xmf3LookAt)
{
	//XMFLOAT4X4 mtxLookAt = Matrix4x4::LookAtRH(m_xmf3Position, xmf3LookAt, m_pPlayer->GetUpVector());
#ifdef _WITH_LEFT_HAND_COORDINATES
	XMFLOAT4X4 mtxLookAt = Matrix4x4::LookAtLH(m_xmf3Position, xmf3LookAt, m_xmf3Up);
#else
	XMFLOAT4X4 mtxLookAt = Matrix4x4::LookAtRH(m_xmf3Position, xmf3LookAt, m_xmf3Up);
#endif
	//ī�޶� ��ȯ ��Ŀ��� ī�޶��� x-��, y-��, z-���� ���Ѵ�. 
	m_xmf3Right = XMFLOAT3(mtxLookAt._11, mtxLookAt._21, mtxLookAt._31);
	m_xmf3Up = XMFLOAT3(mtxLookAt._12, mtxLookAt._22, mtxLookAt._32);
	m_xmf3Look = XMFLOAT3(mtxLookAt._13, mtxLookAt._23, mtxLookAt._33);
}
