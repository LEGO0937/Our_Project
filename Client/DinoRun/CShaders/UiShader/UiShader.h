#pragma once
#include "../Shader.h"

//ui들을 인스턴싱하는 경우 이 구조체를 사용.
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
	virtual void Update(float fTimeElapsed, void* pInformation);
	virtual void BuildObjects(CreateManager* pCreateManager, void* pInformation);
};

class GaugeShader : public CUiShader
{
public:
	GaugeShader();
	virtual ~GaugeShader();

	virtual void BuildObjects(CreateManager* pCreateManager, void* pInformation);
	virtual void Update(float fTimeElapsed){}
	virtual void Update(float fTimeElapsed, void* pInformation);
};


class TrackCountShader : public CUiShader
{
private:

public:
	TrackCountShader();
	virtual ~TrackCountShader();

	virtual void AnimateObjects(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed, void* pInformation);
	virtual void BuildObjects(CreateManager* pCreateManager, void* pInformation);
};


class BackGroundShader : public CUiShader
{
private:

public:
	BackGroundShader();
	virtual ~BackGroundShader();

	virtual void AnimateObjects(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed, void* pInformation);
	virtual void BuildObjects(CreateManager* pCreateManager, void* pInformation);
};

class ButtonShader : public CUiShader
{
private:

public:
	ButtonShader();
	virtual ~ButtonShader();

	virtual void AnimateObjects(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed, void* pInformation);
	virtual void BuildObjects(CreateManager* pCreateManager, void* pInformation);
};

class SelectButtonShader : public CUiShader
{
private:

public:
	SelectButtonShader();
	virtual ~SelectButtonShader();

	virtual void AnimateObjects(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed, void* pInformation);
	virtual void BuildObjects(CreateManager* pCreateManager, void* pInformation);
};

class IconShader : public CUiShader
{
private:

public:
	IconShader();
	virtual ~IconShader();

	virtual void AnimateObjects(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed) {}
	virtual void Update(float fTimeElapsed, void* pInformation);
	virtual void BuildObjects(CreateManager* pCreateManager, void* pInformation);
};