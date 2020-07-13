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
	XMFLOAT4X4 m_xmf4x4ArrivePoint;
public:
	virtual bool Update(float fTimeElapsed, CGameObject* target);
	void SetArrivePoint(const XMFLOAT4X4& xmf4x4Mat) 
	{ 
		m_xmf4x4ArrivePoint = xmf4x4Mat;
		m_xmf4x4ArrivePoint._42 += 100;  //목표로 떨이지기 시작할 y값 -> y변위=100
	}

	ItemMeteorite(int nMeshes = 1);
	~ItemMeteorite();
};
