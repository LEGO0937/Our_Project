#pragma once
#include "BaseObject.h"

class ItemObject : public CGameObject
{
private:
	float m_fEnableCount = 0;
public:
	virtual bool Update(float fTimeElapsed, CGameObject* target);

	ItemObject(int nMeshes = 1);
	~ItemObject();
};

class ItemMeat : public CGameObject
{
private:
	float m_fEnableCount = 0;
public:
	virtual bool Update(float fTimeElapsed, CGameObject* target);

	ItemMeat(int nMeshes = 1);
	~ItemMeat();
};

class ItemBanana : public CGameObject
{
private:
	float m_fEnableCount = 0;
public:
	virtual bool Update(float fTimeElapsed, CGameObject* target);

	ItemBanana(int nMeshes = 1);
	~ItemBanana();
};

class ItemOil : public CGameObject
{
private:
	float m_fEnableCount = 0;
public:
	virtual bool Update(float fTimeElapsed, CGameObject* target);

	ItemOil(int nMeshes = 1);
	~ItemOil();
};


