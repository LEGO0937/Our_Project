#pragma once
#include "BaseObject.h"
#include "EventHandler/EventHandler.h"

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
	float m_fLifeCount = 0;
public:
	virtual bool Update(float fTimeElapsed, CGameObject* target);

	ItemBanana(int nMeshes = 1);
	~ItemBanana();
};

class ItemMud : public CGameObject
{
private:
	float m_fLifeCount = 0;
public:
	virtual bool Update(float fTimeElapsed, CGameObject* target);

	ItemMud(int nMeshes = 1);
	~ItemMud();
};


class ItemStone : public CGameObject
{
private:
	float m_fLifeCount = 0;
public:
	virtual bool Update(float fTimeElapsed, CGameObject* target);

	ItemStone(int nMeshes = 1);
	~ItemStone();
};


class ItemMeteorite : public CGameObject
{
private:
	float m_fLifeCount = 0;
public:
	virtual bool Update(float fTimeElapsed, CGameObject* target);
	virtual void OnUpdateCallback(float fTimeElapsed);

	ItemMeteorite(int nMeshes = 1);
	~ItemMeteorite();
};

class ItemMeteoriteSpot : public CGameObject
{
private:
	float m_fLifeCount = 0;
public:
	virtual bool Update(float fTimeElapsed, CGameObject* target);

	ItemMeteoriteSpot(int nMeshes = 1);
	~ItemMeteoriteSpot();
};

class ItemMound : public CGameObject
{
private:
	float m_fLifeCount = 0;
public:
	virtual bool Update(float fTimeElapsed, CGameObject* target);

	ItemMound(int nMeshes = 1);
	~ItemMound();
};

class ItemFogBox : public CGameObject
{
private:
	float m_fLifeCount = 0;
public:
	virtual bool Update(float fTimeElapsed, CGameObject* target);

	ItemFogBox(int nMeshes = 1);
	~ItemFogBox();
};
