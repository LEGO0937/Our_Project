#include "ItemObject.h"
ItemMound::ItemMound(int nMeshes) :CGameObject(nMeshes)
{
	m_fMass = 0;
	isKinematic = true;
	m_ModelType = ModelType::Item_Meteorite;
	m_fLifeCount = 2;
}
ItemMound::~ItemMound()
{

}

bool ItemMound::Update(float fTimeElapsed, CGameObject* target)
{
	m_fLifeCount -= fTimeElapsed;
	if (m_fLifeCount < 0)
		return true;
	return false;
}