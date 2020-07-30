#include "ItemObject.h"
ItemMound::ItemMound(int nMeshes) :CGameObject(nMeshes)
{
	m_fMass = 1000;
	m_ModelType = ModelType::Item_Mound;
	m_fLifeCount = 2;
}
ItemMound::~ItemMound()
{

}

bool ItemMound::Update(float fTimeElapsed, CGameObject* target)
{
	m_fLifeCount -= fTimeElapsed;
	m_fMass = 0;
	isKinematic = true;
	if (m_fLifeCount < 0)
		return true;
	return false;
}