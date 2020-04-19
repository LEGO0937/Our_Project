#pragma once
#include "../../Objects/BaseObject.h"
#define LOOP 0
#define ONES 1

class ParticleSystem
{
private:
	char m_cTipe = ONES;
	
	vector<CGameObject> m_Vobject;
public:
	ParticleSystem(char type, CGameObject* object, XMFLOAT3 offset, XMFLOAT3 velocity, const char *pTextureName, float life);
	ParticleSystem(char type, vector<CGameObject*> objects, XMFLOAT3 offset, XMFLOAT3 velocity, const char *pTextureName, float life);
	~ParticleSystem() {}
	vector<CGameObject> GetObjects() { return m_Vobject; }

};

