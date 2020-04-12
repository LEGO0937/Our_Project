#pragma once
#include "../Shader.h"
class BillBoardShader : public CObInstancingShader
{
public:
	BillBoardShader();
	virtual ~BillBoardShader();

	virtual void BuildObjects(shared_ptr<CreateManager> pCreateManager, void* terrain) {}
	virtual void BuildObjects(shared_ptr<CreateManager> pCreateManager, const char *pszFileName, const char* filename = NULL);

	virtual void Load(shared_ptr<CreateManager> pCreateManager, const char* filename=NULL);
};