#include "ItemObject.h"
ItemStone::ItemStone(int nMeshes) :CGameObject(nMeshes)
{
	m_fMass = 200;
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