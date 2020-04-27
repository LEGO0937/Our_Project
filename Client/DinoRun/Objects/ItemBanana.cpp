#include "ItemObject.h"
ItemBanana::ItemBanana(int nMeshes) :CGameObject(nMeshes)
{
	isKinematic = true;
	m_ModelType = ModelType::Item_Banana;
}
ItemBanana::~ItemBanana()
{

}

bool ItemBanana::Update(float fTimeElapsed, CGameObject* target)
{
	return false;
}