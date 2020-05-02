#pragma once
#include "../Shader.h"
class BillBoardShader : public CObInstancingShader
{
public:
	BillBoardShader();
	virtual ~BillBoardShader();

	virtual void BuildObjects(CreateManager* pCreateManager, void* pInformation) {}
	virtual void BuildObjects(CreateManager* pCreateManager, const char *pszFileName, const char* filename = NULL);

	virtual void Load(CreateManager* pCreateManager, const char* filename=NULL);
};