#pragma once
#include "../Shader.h"
class BillBoardShader : public CObInstancingShader
{
private:
	CB_BillBoard *billBoardCb;
public:
	BillBoardShader();
	virtual ~BillBoardShader();

	virtual void BuildObjects(CreateManager* pCreateManager, void* pInformation);
	virtual void BuildObjects(CreateManager* pCreateManager, const char *pszFileName, const char* filename = NULL) {};
	void BuildObjects(CreateManager* pCreateManager, float size, const char *pszFileName, const char* filename = NULL);

	virtual void Load(CreateManager* pCreateManager, const char* filename=NULL);

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	virtual void ShadowRender(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
};