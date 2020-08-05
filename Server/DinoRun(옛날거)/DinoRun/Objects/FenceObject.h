#pragma once
#include "BaseObject.h"

class FenceObject : public CGameObject
{
private:
	float m_fEnableCount = 0;
public:
	FenceObject(int nMeshes = 1);
	~FenceObject();
};

