#pragma once
#include "../Shader.h"

class ModelShader : public CObInstancingShader
{
protected:
	CB_BillBoard* billBoardCb;
public:
	ModelShader();
	virtual ~ModelShader();
	void BuildObjects(CreateManager* pCreateManager, void* pInformation);
	virtual void Update(float fTimeElapsed){}
	virtual void addObject(CreateManager* pCreateManager, const XMFLOAT4X4& xmf3DepartPosition) {}

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
	virtual void addObject(CreateManager* pCreateManager, const XMFLOAT4X4& xmf3DepartPosition);
};

class MudShader : public ModelShader
{
public:
	MudShader();
	virtual ~MudShader();

	virtual void Load(CreateManager* pCreateManager, const char* filename = NULL, const char* Loadname = NULL);
	virtual void Update(float fTimeElapsed);
	virtual void addObject(CreateManager* pCreateManager, const XMFLOAT4X4& xmf3DepartPosition);
};

class StoneShader : public ModelShader
{
public:
	StoneShader();
	virtual ~StoneShader();

	virtual void Load(CreateManager* pCreateManager, const char* filename = NULL, const char* Loadname = NULL);
	virtual void Update(float fTimeElapsed);
	virtual void addObject(CreateManager* pCreateManager, const XMFLOAT4X4& xmf3DepartPosition);
};

class MeteoriteShader : public ModelShader
{
public:
	MeteoriteShader();
	virtual ~MeteoriteShader();

	virtual void Load(CreateManager* pCreateManager, const char* filename = NULL, const char* Loadname = NULL);
	virtual void Update(float fTimeElapsed);
	virtual void addObject(CreateManager* pCreateManager, const XMFLOAT4X4& xmf3DepartPosition);
};

class MoundShader : public ModelShader
{
public:
	MoundShader();
	virtual ~MoundShader();

	virtual void Load(CreateManager* pCreateManager, const char* filename = NULL, const char* Loadname = NULL);
	virtual void Update(float fTimeElapsed);
	virtual void addObject(CreateManager* pCreateManager, const XMFLOAT4X4& xmf3DepartPosition);
};