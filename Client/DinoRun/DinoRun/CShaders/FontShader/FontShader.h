#pragma once

#include "../Shader.h"
#include "../../Common/Font/Font.h"

struct TextVertex {
	TextVertex(float r, float g, float b, float a, float u, float v, float tw, float th, float x, float y, float w, float h) : color(r, g, b, a), texCoord(u, v, tw, th), pos(x, y, w, h) {}
	XMFLOAT4 pos;
	XMFLOAT4 texCoord;
	XMFLOAT4 color;
};

class FontShader : public CShader
{
private:
	Font arialFont;

	ID3D12Resource* textVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW textVertexBufferView; // a view for our text vertex buffer
	UINT8* textVBGPUAddress;

	int maxNumTextCharacters = 1024;
	CTexture * fontTex;
public:
	FontShader();
	virtual ~FontShader();

	virtual void BuildObjects(shared_ptr<CreateManager> pCreateManager, void* terrain);
	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList) {}
	virtual void ReleaseShaderVariables();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	void RenderText(ID3D12GraphicsCommandList *pd3dCommandList, Font font, string text, XMFLOAT2 pos, XMFLOAT2 scale = XMFLOAT2(1.0f, 1.0f), XMFLOAT2 padding = XMFLOAT2(0.5f, 0.0f), XMFLOAT4 color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

};

