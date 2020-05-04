#include "ItemObject.h"
ItemStone::ItemStone(int nMeshes) :CGameObject(nMeshes)
{
	isKinematic = true;
	m_ModelType = ModelType::Item_Stone;
}
ItemStone::~ItemStone()
{

}

bool ItemStone::Update(float fTimeElapsed, CGameObject* target)
{
	return false;
}