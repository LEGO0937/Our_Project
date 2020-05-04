#pragma once
#include "../Shader.h"
class ModelShader : public CObInstancingShader
{
public:
	ModelShader();
	virtual ~ModelShader();

	virtual void BuildObjects(CreateManager* pCreateManager, const char *pszFileName, const char* filename = NULL);
	virtual void Update(float fTimeElapsed){}
	virtual void FixedUpdate(float fTimeElapsed) {}
	virtual void addObject(CreateManager* pCreateManager, const XMFLOAT3& xmf3Position) {}

	virtual void Load(CreateManager* pCreateManager, const char* filename = NULL, const char* Loadname = NULL);
};

class TreeShader : public ModelShader
{
public:
	TreeShader();
	virtual ~TreeShader();

	virtual void Load(CreateManager* pCreateManager, const char* filename = NULL, const char* Loadname = NULL);
};

class FenceShader : public ModelShader
{
public:
	FenceShader();
	virtual ~FenceShader();

	virtual void Load(CreateManager* pCreateManager, const char* filename = NULL, const char* Loadname = NULL);
	virtual void Update(float fTimeElapsed);
};

class BlockShader : public ModelShader
{
public:
	BlockShader();
	virtual ~BlockShader();

	virtual void Load(CreateManager* pCreateManager, const char* filename = NULL, const char* Loadname = NULL);
};

class ItemShader : public ModelShader
{
public:
	ItemShader();
	virtual ~ItemShader();

	virtual void Load(CreateManager* pCreateManager, const char* filename = NULL, const char* Loadname = NULL);
	virtual void Update(float fTimeElapsed);
};

class MeatShader : public ModelShader
{
public:
	MeatShader();
	virtual ~MeatShader();

	virtual void Load(CreateManager* pCreateManager, const char* filename = NULL, const char* Loadname = NULL);
	virtual void Update(float fTimeElapsed);
};


class BananaShader : public ModelShader
{
public:
	BananaShader();
	virtual ~BananaShader();

	virtual void Load(CreateManager* pCreateManager, const char* filename = NULL, const char* Loadname = NULL);
	virtual void Update(float fTimeElapsed);
	virtual void addObject(CreateManager* pCreateManager, const XMFLOAT3& xmf3Position);
};

class OilShader : public ModelShader
{
public:
	OilShader();
	virtual ~OilShader();

	virtual void Load(CreateManager* pCreateManager, const char* filename = NULL, const char* Loadname = NULL);
	virtual void Update(float fTimeElapsed);
	virtual void addObject(CreateManager* pCreateManager, const XMFLOAT3& xmf3Position);
};

class StoneShader : public ModelShader
{
public:
	StoneShader();
	virtual ~StoneShader();

	virtual void Load(CreateManager* pCreateManager, const char* filename = NULL, const char* Loadname = NULL);
	virtual void Update(float fTimeElapsed);
	virtual void addObject(CreateManager* pCreateManager, const XMFLOAT3& xmf3Position);
};

