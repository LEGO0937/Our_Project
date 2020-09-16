#include "ItemObject.h"
ItemStone::ItemStone(int nMeshes) :CGameObject(nMeshes)
{
	m_fMass = 150;
	isKinematic = false;
	m_ModelType = ModelType::Item_Stone;
	m_fLifeCount = 10;
}
ItemStone::~ItemStone()
{

}

bool ItemStone::Update(float fTimeElapsed, CGameObject* target)
{
	m_fLifeCount -= fTimeElapsed;
	if (m_fLifeCount < 0)
		return true;
	return false;
}