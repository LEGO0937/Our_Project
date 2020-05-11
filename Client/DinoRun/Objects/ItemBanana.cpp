#include "ItemObject.h"
ItemBanana::ItemBanana(int nMeshes) :CGameObject(nMeshes)
{
	isKinematic = true;
	m_ModelType = ModelType::Item_Banana;
	m_fLifeCount = 30;
}
ItemBanana::~ItemBanana()
{

}

bool ItemBanana::Update(float fTimeElapsed, CGameObject* target)
{
	m_fLifeCount -= fTimeElapsed;
	if (m_fLifeCount < 0)
		return true;
	return false;
}