#pragma once
#include "../Shader.h"
class MinimapShader: public CObInstancingShader
{
public:
	MinimapShader();
	virtual ~MinimapShader();

	virtual void BuildObjects(CreateManager* pCreateManager, void* pInformation) {}
	virtual void BuildObjects(CreateManager* pCreateManager, const char *pszFileName, const char* filename = NULL);
};