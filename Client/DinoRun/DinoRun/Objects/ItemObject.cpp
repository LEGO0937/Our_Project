#include "ItemObject.h"
ItemObject::ItemObject(int nMeshes) :CGameObject(nMeshes)
{
}
ItemObject::~ItemObject()
{

}

void ItemObject::Animate(float fTimeElapsed)
{
	CGameObject::Animate(fTimeElapsed);
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