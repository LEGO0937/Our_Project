#include "ItemObject.h"
ItemMud::ItemMud(int nMeshes) :CGameObject(nMeshes)
{
	isKinematic = true;
	m_ModelType = ModelType::Item_Mud;
	m_fLifeCount = 20;
}
ItemMud::~ItemMud()
{

}

bool ItemMud::Update(float fTimeElapsed, CGameObject* target)
{
	m_fLifeCount -= fTimeElapsed;
	if (m_fLifeCount < 0)
		return true;
	return false;
}