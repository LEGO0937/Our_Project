#include "ItemObject.h"
ItemOil::ItemOil(int nMeshes) :CGameObject(nMeshes)
{
	isKinematic = true;
	m_ModelType = ModelType::Item_Oil;
}
ItemOil::~ItemOil()
{

}

bool ItemOil::Update(float fTimeElapsed, CGameObject* target)
{
	return false;
}