#include "ItemObject.h"
ItemMeteorite::ItemMeteorite(int nMeshes) :CGameObject(nMeshes)
{
	m_fMass = 10;
	//isKinematic = true;
	m_ModelType = ModelType::Item_Meteorite;
	m_fLifeCount = 2;
}
ItemMeteorite::~ItemMeteorite()
{

}

bool ItemMeteorite::Update(float fTimeElapsed, CGameObject* target)
{
	m_fLifeCount -= fTimeElapsed;
	if (m_fLifeCount < 0)
	{
		m_xmf3Gravity = XMFLOAT3(0.0f, 9.8f, 0.0f);
		return true;
	}
	return false;
}