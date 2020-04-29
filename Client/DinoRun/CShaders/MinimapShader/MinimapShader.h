#pragma once
#include "../Shader.h"
class MinimapShader: public CObInstancingShader
{
public:
	MinimapShader();
	virtual ~MinimapShader();

	virtual void BuildObjects(shared_ptr<CreateManager> pCreateManager, void* pInformation) {}
	virtual void BuildObjects(shared_ptr<CreateManager> pCreateManager, const char *pszFileName, const char* filename = NULL);
};