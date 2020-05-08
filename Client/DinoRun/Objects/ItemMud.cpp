#include "ItemObject.h"
ItemMud::ItemMud(int nMeshes) :CGameObject(nMeshes)
{
	isKinematic = true;
	m_ModelType = ModelType::Item_Oil;
}
ItemMud::~ItemMud()
{

}

bool ItemMud::Update(float fTimeElapsed, CGameObject* target)
{
	return false;
}