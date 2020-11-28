#pragma once
#include "../Shader.h"
class MinimapShader: public CObInstancingShader
{
public:
	MinimapShader();
	virtual ~MinimapShader();

	virtual void BuildObjects(void* pInformation) {}
	virtual void BuildObjects(const char *pszFileName, const char* filename = NULL);
};