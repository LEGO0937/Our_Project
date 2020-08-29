#include "ItemObject.h"
ItemFogBox::ItemFogBox(int nMeshes) :CGameObject(nMeshes)
{
	m_fMass = 0;
	m_ModelType = ModelType::Item_FogBox;
	m_fLifeCount = 20;
}
ItemFogBox::~ItemFogBox()
{

}

bool ItemFogBox::Update(float fTimeElapsed, CGameObject* target)
{
	m_fLifeCount -= fTimeElapsed;
	if (m_fLifeCount < 0)
		return true;
	return false;
}