#pragma once
#include "../../CShaders/FontShader/FontShader.h"
class FontManager
{
private:
	FontShader* m_pFontShader;
public:
	FontManager(){}
	~FontManager() { Release(); }
	void Initialize(CreateManager* createMgr);
	void Release();

	FontShader* getFontShader() { return m_pFontShader; }
	
	void ReleaseUploadBuffers();
};

