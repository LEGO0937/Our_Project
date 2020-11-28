#pragma once
#include "../Shader.h"
class BillBoardShader : public CObInstancingShader
{
private:
	CB_BillBoard *billBoardCb;
public:
	BillBoardShader();
	virtual ~BillBoardShader();

	virtual void BuildObjects(void* pInformation);
	virtual void BuildObjects(const char *pszFileName, const char* filename = NULL) {};
	void BuildObjects(float size, const char *pszFileName, const char* filename = NULL);

	virtual void Load(const char* filename=NULL);

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	virtual void ShadowRender(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
};