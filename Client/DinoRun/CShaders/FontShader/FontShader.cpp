#include "FontShader.h"

#include "../../Common/FrameWork/CreateManager.h"

FontShader::FontShader()
{
	m_nReferences = 1;
}
FontShader::~FontShader()
{

}

void FontShader::BuildObjects(shared_ptr<CreateManager> pCreateManager, void* terrain)
{
	
	arialFont = LoadFont(L"Resources/Fonts/Arial.fnt", FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

	fontTex = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	fontTex->LoadTextureFromFile(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), arialFont.fontImage.c_str(), 0);

	CreateCbvSrvDescriptorHeaps(pCreateManager, 0, 1);

	CreateShaderResourceViews(pCreateManager, fontTex, 8, true);

	HRESULT hr;
	for (int i = 0; i < 18; ++i)
	{
		pCreateManager->GetDevice().Get()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(maxNumTextCharacters * sizeof(TextVertex)),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&textVertexBuffer[i]));
		textVertexBuffer[i]->SetName(L"Text Vertex Buffer Upload Resource Heap");

		hr = textVertexBuffer[i]->Map(0, NULL, reinterpret_cast<void**>(&textVBGPUAddress[i]));

		textVertexBufferView[i].BufferLocation = textVertexBuffer[i]->GetGPUVirtualAddress();
		textVertexBufferView[i].StrideInBytes = sizeof(TextVertex);
		textVertexBufferView[i].SizeInBytes = maxNumTextCharacters * sizeof(TextVertex);
	}
}

void FontShader::ReleaseObjects()
{

}

void FontShader::ReleaseShaderVariables()
{
	for (int i = 0; i < 18; ++i)
	{
		if (textVertexBuffer)
		{
			textVertexBuffer[i]->Unmap(0, NULL);
			textVertexBuffer[i]->Release();
			textVertexBuffer[i] = NULL;
		}
	}
	if (fontTex)
	{
		fontTex->ReleaseShaderVariables();
		fontTex->Release();
	}
}
void FontShader::ReleaseUploadBuffers()
{
	if (fontTex)
	{
		fontTex->ReleaseUploadBuffers();
	}
}


void FontShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, vector<GameText>& vec)
{
	CShader::Render(pd3dCommandList, pCamera);
	pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	fontTex->UpdateShaderVariable(pd3dCommandList, 0);
	//string t = "abcdefghijk";
	//RenderText(pd3dCommandList,arialFont, t, XMFLOAT2(0.02f, 0.8f), XMFLOAT2(1.0f, 1.0f), XMFLOAT2(0.5f, 0.0f),XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	int i = 0;
	for (const GameText& text : vec)
	{
		pd3dCommandList->IASetVertexBuffers(0, 1, &textVertexBufferView[i]);
		RenderText(pd3dCommandList, i++,arialFont, text.text, text.pos, text.scale, XMFLOAT2(0.5f, 0.0f), text.color);
	}
}

void FontShader::RenderText(ID3D12GraphicsCommandList *pd3dCommandList, int idx, Font font, string text, XMFLOAT2 pos, XMFLOAT2 scale, XMFLOAT2 padding, XMFLOAT4 color)
{
	int numCharacters = 0;

	float topLeftScreenX = (pos.x * 2.0f) - 1.0f;
	float topLeftScreenY = ((1.0f - pos.y) * 2.0f) - 1.0f;

	float x = topLeftScreenX;
	float y = topLeftScreenY;

	float horrizontalPadding = (font.leftpadding + font.rightpadding) * padding.x;
	float verticalPadding = (font.toppadding + font.bottompadding) * padding.y;

	TextVertex* vert = (TextVertex*)textVBGPUAddress[idx];

	wchar_t lastChar = -1; 

	for (int i = 0; i < text.size(); ++i)
	{
		wchar_t c = text[i];

		FontChar* fc = font.GetChar(c);

		if (fc == nullptr)
			continue;

		if (c == L'\0')
			break;

		if (c == L'\n')
		{
			x = topLeftScreenX;
			y -= (font.lineHeight + verticalPadding) * scale.y;
			continue;
		}

		if (numCharacters >= maxNumTextCharacters)
			break;

		float kerning = 0.0f;
		if (i > 0)
			kerning = font.GetKerning(lastChar, c);
		
		vert[numCharacters] = TextVertex(color.x,
			color.y,
			color.z,
			color.w,
			fc->u,
			fc->v,
			fc->twidth,
			fc->theight,
			x + ((fc->xoffset + kerning) * scale.x),
			y - (fc->yoffset * scale.y),
			fc->width * scale.x,
			fc->height * scale.y);
			
		numCharacters++;

		x += (fc->xadvance - horrizontalPadding) * scale.x;

		lastChar = c;
	}

	pd3dCommandList->DrawInstanced(4, numCharacters, 0, 0);
}
