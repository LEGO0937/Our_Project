#pragma once

#include "../Shader.h"
#include "../../Common/Font/Font.h"

struct TextVertex {
	TextVertex(float r, float g, float b, float a, float u, float v, float tw, float th, float x, float y, float w, float h) : color(r, g, b, a), texCoord(u, v, tw, th), pos(x, y, w, h) {}
	XMFLOAT4 pos;
	XMFLOAT4 texCoord;
	XMFLOAT4 color;
};
struct GameText {
	string text ="";
	XMFLOAT2 pos;
	XMFLOAT2 scale = XMFLOAT2(1,1);
	XMFLOAT4 color = XMFLOAT4(1, 1, 1, 1);
	GameText(XMFLOAT2 position, XMFLOAT2 size = XMFLOAT2(1,1),XMFLOAT4 Color = XMFLOAT4(1, 1, 1, 1))
	{
		pos = position;
		scale = size;
		color = Color;
	}
};
class FontShader : public CShader
{
private:
	Font arialFont;

	ID3D12Resource* textVertexBuffer[18];
	D3D12_VERTEX_BUFFER_VIEW textVertexBufferView[18]; // a view for our text vertex buffer
	UINT8* textVBGPUAddress[18];

	int maxNumTextCharacters = 1024;
	CTexture * pFontTex;
public:
	FontShader();
	virtual ~FontShader();

	virtual void BuildObjects(CreateManager* pCreateManager, void* pInformation);
	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList) {}
	virtual void ReleaseShaderVariables();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, vector<GameText>& vec);
	void RenderText(ID3D12GraphicsCommandList *pd3dCommandList, const int& idx, Font& font, const string& text, const XMFLOAT2& pos, const XMFLOAT2& scale = XMFLOAT2(1.0f, 1.0f), const XMFLOAT2& padding = XMFLOAT2(0.5f, 0.0f), const XMFLOAT4& color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

};

