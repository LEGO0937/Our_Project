#include "ItemObject.h"
ItemObject::ItemObject(int nMeshes) :CGameObject(nMeshes)
{
	isKinematic = true;
	m_ModelType = ModelType::Meat_Item;
}
ItemObject::~ItemObject()
{

}

bool ItemObject::Update(float fTimeElapsed, CGameObject* target)
{
	Rotate(0, 360*fTimeElapsed ,0);
	
	if (!isEnable)
	{
		m_fEnableCount += fTimeElapsed;
		if (m_fEnableCount > 3)
		{
			isEnable = true;
			m_fEnableCount = 0.f;
		}
	}
	return false;
}