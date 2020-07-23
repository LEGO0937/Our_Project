#include "ItemObject.h"
#include "TerrainObject.h"
#include "EventHandler/EventHandler.h"

ItemMeteorite::ItemMeteorite(int nMeshes) :CGameObject(nMeshes)
{
	m_fMass = 400;
	isKinematic = true;
	m_ModelType = ModelType::Item_Meteorite;
	m_fLifeCount = 2;
	m_xmf3Gravity = XMFLOAT3(0.0f, -9.8f, 0.0f);
}
ItemMeteorite::~ItemMeteorite()
{

}

bool ItemMeteorite::Update(float fTimeElapsed, CGameObject* target)
{
	//m_fLifeCount -= fTimeElapsed;
	if (m_fLifeCount < 0)
	{
		CHeightMapTerrain* t = (CHeightMapTerrain*)m_pUpdatedContext;
		MessageStruct message;
		message.msgName = "Add_Model";
		message.shaderName = "MoundShader";
		message.departMat = m_xmf4x4World;
		message.departMat._42 = t->GetHeight(m_xmf4x4World._41, m_xmf4x4World._43);
		//EventHandler::GetInstance()->CallBack(message);
		EventHandler::GetInstance()->RegisterEvent(message);

		message.msgName = "Add_Particle";
		message.shaderName = "StoneParticle";
		EventHandler::GetInstance()->RegisterEvent(message);

		return true;
	}
	return false;
}

void ItemMeteorite::OnUpdateCallback(float fTimeElapsed)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)m_pUpdatedContext;
	XMFLOAT3 xmf3Scale = pTerrain->GetScale();
	XMFLOAT3 xmf3Position = GetPosition();
	int z = (int)(xmf3Position.z / xmf3Scale.z);
	bool bReverseQuad = ((z % 2) != 0);
#ifndef _WITH_LEFT_HAND_COORDINATES
	float fHeight = pTerrain->GetHeight(xmf3Position.x, 256 * xmf3Scale.z - xmf3Position.z);
#else
	float fHeight = pTerrain->GetHeight(xmf3Position.x, 256 * xmf3Scale.z - xmf3Position.z);
#endif

	if (xmf3Position.y < fHeight)
	{
		m_fLifeCount = -1;
	}
}