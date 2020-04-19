#pragma once
#include "../Shader.h"


struct UI_INFO
{
	vector<XMFLOAT3> positions;
	vector<float> f_uvY;
	XMFLOAT2 meshSize;
	string textureName;
};

class TimeCountShader : public CUiShader
{
private:
	float								m_fSecond = 0.0f;
	int									m_iMinute = 0;
public:
	TimeCountShader();
	virtual ~TimeCountShader();

	virtual void AnimateObjects(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed, CPlayer* player);
	virtual void BuildObjects(shared_ptr<CreateManager> pCreateManager, void* terrain);
};

class GaugeShader : public CUiShader
{
public:
	GaugeShader();
	virtual ~GaugeShader();

	virtual void BuildObjects(shared_ptr<CreateManager> pCreateManager, void* terrain);
	virtual void Update(float fTimeElapsed){}
	virtual void Update(float fTimeElapsed, CPlayer* player);
};


class TrackCountShader : public CUiShader
{
private:

public:
	TrackCountShader();
	virtual ~TrackCountShader();

	virtual void AnimateObjects(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed, CPlayer* player);
	virtual void BuildObjects(shared_ptr<CreateManager> pCreateManager, void* terrain);
};


class BackGroundShader : public CUiShader
{
private:

public:
	BackGroundShader();
	virtual ~BackGroundShader();

	virtual void AnimateObjects(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed, CPlayer* player);
	virtual void BuildObjects(shared_ptr<CreateManager> pCreateManager, void* terrain);
};

class ButtonShader : public CUiShader
{
private:

public:
	ButtonShader();
	virtual ~ButtonShader();

	virtual void AnimateObjects(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed, CPlayer* player);
	virtual void BuildObjects(shared_ptr<CreateManager> pCreateManager, void* terrain);
};

class SelectButtonShader : public CUiShader
{
private:

public:
	SelectButtonShader();
	virtual ~SelectButtonShader();

	virtual void AnimateObjects(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed, CPlayer* player);
	virtual void BuildObjects(shared_ptr<CreateManager> pCreateManager, void* terrain);
};