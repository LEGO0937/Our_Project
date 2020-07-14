#pragma once
#include "BaseObject.h"

class CheckPointObject : public CGameObject
{
private:
	float m_fEnableCount = 0;
public:
	virtual bool Update(float fTimeElapsed, CGameObject* target);

	CheckPointObject(int nMeshes = 1);
	~CheckPointObject();
};

