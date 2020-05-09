#include "stdafx.h"
#include "BillboardObject.h"


CBillboardObject::CBillboardObject()
{
}


CBillboardObject::~CBillboardObject()
{
}

void CBillboardObject::Animate(float fTimeElapsed, CCamera* pCamera,CPlayer *pPlayer)
{

	// 카메라 위치를 받아온다.
	XMFLOAT3 xmf3CameraPosition = pCamera->GetPosition();
	//SetLookAt 함수를 사용하면 인자로 넘겨준 위치를 바라보게 할 수 있다.
	SetLookAt(xmf3CameraPosition);
}

void CBillboardObject::SetLookAt(XMFLOAT3& xmfTarget)
{
	//각 나무 빌보드의 위치를 셋
	XMFLOAT3 xmf3Position(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);

	//각 나무 빌보드의 UP벡터는 항상 위를 바라보고 있어야한다. X,Y =1.0, Z순서임
	XMFLOAT3 xmf3Up(0.0f, 1.0f, 0.0f);

	//Look벡터를 구하는 부분이다. 
// Vector3::SubtractNormalize (A=타겟,B=빌보드 위치)	//Look벡터=A-B
// A-B => B가 A를 바라보는 벡터를 가진다. 
	XMFLOAT3 xmf3Look = Vector3::SubtractNormalize(xmfTarget, xmf3Position, true);
	// 룩벡터와 Up벡터를 외적하면 Right벡터를 얻을수 있다.
	// 왼손 좌표계임을 주의 바람
	XMFLOAT3 xmf3Right = Vector3::CrossProduct(xmf3Up, xmf3Look, true);



	// 각 구한 벡터를 행렬의 값으로 넣는다.
// 1행 : Right벡터
// 2행 : up 벡터
// 3행 : Look벡터
// 4행 : 위치 벡터 (여기서는 위치벡터는 셋해주지 않음)
	m_xmf4x4World._11 = xmf3Right.x;
	m_xmf4x4World._12 = xmf3Right.y;
	m_xmf4x4World._13 = xmf3Right.z;
	m_xmf4x4World._21 = xmf3Up.x;
	m_xmf4x4World._22 = xmf3Up.y;
	m_xmf4x4World._23 = xmf3Up.z;
	m_xmf4x4World._31 = xmf3Look.x;
	m_xmf4x4World._32 = xmf3Look.y;
	m_xmf4x4World._33 = xmf3Look.z;

}

void CBillboardObject::Render(ID3D12GraphicsCommandList *pd3dCommandList,int objNum ,CCamera *pCamera)
{
	CGameObject::Render(pd3dCommandList, pCamera,objNum);
}


//////////////////////////////////////////////////////
//CGrassBillboardObject

CGrassBillboardObject::CGrassBillboardObject()
{
	totalTime = 0.0f;
}

CGrassBillboardObject::~CGrassBillboardObject()
{

}

//풀 빌보드 Animate부분
void CGrassBillboardObject::Animate(float fTimeElapsed, CCamera* pCamera,bool isIntersect)
{

	// 카메라 위치를 받아온다.
	XMFLOAT3 xmf3CameraPosition = pCamera->GetPosition();
	
	//SetLookAt 함수를 사용하면 인자로 넘겨준 위치를 바라보게 할 수 있다.
	SetLookAt(xmf3CameraPosition);
	
	// 이 부분은 풀이 좌우로 흔들리게끔 해놓은 부분이다. 
	// 플레이어 반지름 100m 이내에 들어오면 풀은 아주 조금만 흔들린다.
	// 그 밖에 위치한 풀 빌보드는 많이 흔들린다. 


	totalTime += sinf(fTimeElapsed);
	float fAngle = sinf(0.5f*totalTime);

	XMFLOAT4X4 mtxRotate = Matrix4x4::Identity();


	// 
	if (isIntersect) {
		fAngle = 1.5f*cosf(3.0f*totalTime);
		//풀 오브젝트는  플레이어를 바라보고 있는 상태이므로 Roll회전을 해야함
		mtxRotate = Matrix4x4::RotationYawPitchRoll(0.0f, 0.0f, fAngle);
	}
	else {
		mtxRotate = Matrix4x4::RotationYawPitchRoll(0.0f, 0.0f, fAngle*60.0f);
	}

	//위에서 만든 회전행렬 mtxRotate를 기존 자신의 월드 변환 행령에 곱한다.
	// 여기서 주의할점이 있다.
	// T: 이동 행렬
	// R : 회전 행렬
	// 항상 이동행렬이랑 회전행렬이 곱해질때는 무조건
	// R(3) *R(2) *R(1) * T  이런식으로 
	// 이동행렬이 항상 마지막으로 곱해져야함 .
	// 그렇지 않고 중간에 T행렬이 있을 경우 자전이 아니고 공전이 되버림.
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);


}

void CGrassBillboardObject::SetLookAt(XMFLOAT3& xmfTarget)
{

	//각 풀 빌보드의 위치를 셋
	XMFLOAT3 xmf3Position(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);

	//각 풀 빌보드의 UP벡터는 항상 위를 바라보고 있어야한다. X,Y =1.0, Z순서임
	XMFLOAT3 xmf3Up(0.0f, 1.0f, 0.0f);
	//Look벡터를 구하는 부분이다. 
	// Vector3::SubtractNormalize (A=타겟,B=빌보드 위치)	//Look벡터=A-B
	// A-B => B가 A를 바라보는 벡터를 가진다. 
	XMFLOAT3 xmf3Look = Vector3::SubtractNormalize(xmfTarget, xmf3Position, true);
	
	// 룩벡터와 Up벡터를 외적하면 Right벡터를 얻을수 있다.
	// 왼손 좌표계임을 주의 바람
	XMFLOAT3 xmf3Right = Vector3::CrossProduct(xmf3Up, xmf3Look, true);



	// 각 구한 벡터를 행렬의 값으로 넣는다.
	// 1행 : Right벡터
	// 2행 : up 벡터
	// 3행 : Look벡터
	// 4행 : 위치 벡터 (여기서는 위치벡터는 셋해주지 않음)

	m_xmf4x4World._11 = xmf3Right.x;
	m_xmf4x4World._12 = xmf3Right.y;
	m_xmf4x4World._13 = xmf3Right.z;
	m_xmf4x4World._21 = xmf3Up.x;
	m_xmf4x4World._22 = xmf3Up.y;
	m_xmf4x4World._23 = xmf3Up.z;
	m_xmf4x4World._31 = xmf3Look.x;
	m_xmf4x4World._32 = xmf3Look.y;
	m_xmf4x4World._33 = xmf3Look.z;


}

void CGrassBillboardObject::Render(ID3D12GraphicsCommandList *pd3dCommandList,int objNum, CCamera *pCamera)
{
	CGameObject::Render(pd3dCommandList, pCamera,objNum);
}


