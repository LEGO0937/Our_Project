#include "FontManager.h"


void FontManager::Initialize()
{
	m_pFontShader = new FontShader;
	m_pFontShader->BuildObjects(NULL);
}
void FontManager::Release()
{
	m_pFontShader->ReleaseShaderVariables();
	m_pFontShader->Release();
}
void FontManager::ReleaseUploadBuffers()
{
	m_pFontShader->ReleaseUploadBuffers();
}