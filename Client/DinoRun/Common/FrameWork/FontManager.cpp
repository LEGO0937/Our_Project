#include "FontManager.h"


void FontManager::Initialize(shared_ptr<CreateManager> createMgr)
{
	m_pFontShader = shared_ptr<FontShader>(new FontShader);
	m_pFontShader->BuildObjects(createMgr, NULL);
}
void FontManager::Release()
{
	m_pFontShader->ReleaseShaderVariables();
}
void FontManager::ReleaseUploadBuffers()
{
	m_pFontShader->ReleaseUploadBuffers();
}