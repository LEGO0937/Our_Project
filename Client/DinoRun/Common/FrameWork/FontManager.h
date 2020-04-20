#pragma once
#include "../../CShaders/FontShader/FontShader.h"
class FontManager
{
private:
	shared_ptr<FontShader> m_pFontShader;
public:
	FontManager(){}
	~FontManager() { Release(); }
	void Initialize(shared_ptr<CreateManager> createMgr);
	void Release();

	shared_ptr<FontShader> getFontShader() { return m_pFontShader; }
	
	void ReleaseUploadBuffers();
};

