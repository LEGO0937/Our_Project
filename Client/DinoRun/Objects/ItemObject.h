#pragma once
#include "BaseObject.h"

class ItemObject : public CGameObject
{
private:
	float m_fEnableCount = 0;
public:
	void Animate(float fTimeElapsed); // 局聪皋捞记 贸府
	virtual bool Update(float fTimeElapsed, CGameObject* target);

	ItemObject(int nMeshes = 1);
	~ItemObject();
};

