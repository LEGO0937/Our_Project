#include "ItemObject.h"
ItemMeat::ItemMeat(int nMeshes) :CGameObject(nMeshes)
{
	isKinematic = true;
	m_ModelType = ModelType::Item_Meat;
}
ItemMeat::~ItemMeat()
{

}

bool ItemMeat::Update(float fTimeElapsed, CGameObject* target)
{
	Rotate(0, 360 * fTimeElapsed, 0);

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