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

	pCreateManager->GetDevice().Get()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // upload heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&CD3DX12_RESOURCE_DESC::Buffer(maxNumTextCharacters * sizeof(TextVertex)), // resource description for a buffer
		D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
		nullptr,
		IID_PPV_ARGS(&textVertexBuffer));
	textVertexBuffer->SetName(L"Text Vertex Buffer Upload Resource Heap");

	hr = textVertexBuffer->Map(0, NULL, reinterpret_cast<void**>(&textVBGPUAddress));

	textVertexBufferView.BufferLocation = textVertexBuffer->GetGPUVirtualAddress();
	textVertexBufferView.StrideInBytes = sizeof(TextVertex);
	textVertexBufferView.SizeInBytes = maxNumTextCharacters * sizeof(TextVertex);

}

void FontShader::ReleaseObjects()
{
	/*
	if (objectList.size())
	{
		for (CGameObject* ob : objectList)
		{
			ob->Release();
		}
		objectList.clear();
	}
	if (m_ppObjects)
	{
		m_ppObjects->Release();
		m_ppObjects = NULL;
	}
	*/
}

void FontShader::ReleaseShaderVariables()
{
	if (textVertexBuffer)
	{
		textVertexBuffer->Unmap(0, NULL);
		textVertexBuffer->Release();
		textVertexBuffer = NULL;
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


void FontShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);
	pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	pd3dCommandList->IASetVertexBuffers(0, 1, &textVertexBufferView);
	fontTex->UpdateShaderVariable(pd3dCommandList, 0);
	string t = "abcdefghijk";
	RenderText(pd3dCommandList,arialFont, t, XMFLOAT2(0.02f, 0.8f), XMFLOAT2(1.0f, 1.0f), XMFLOAT2(0.5f, 0.0f),XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
}

void FontShader::RenderText(ID3D12GraphicsCommandList *pd3dCommandList, Font font, string text, XMFLOAT2 pos, XMFLOAT2 scale, XMFLOAT2 padding, XMFLOAT4 color)
{
	//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//
	//// set the text vertex buffer
	//commandList->IASetVertexBuffers(0, 1, &textVertexBufferView[frameIndex]);

	int numCharacters = 0;

	float topLeftScreenX = (pos.x * 2.0f) - 1.0f;
	float topLeftScreenY = ((1.0f - pos.y) * 2.0f) - 1.0f;

	float x = topLeftScreenX;
	float y = topLeftScreenY;

	float horrizontalPadding = (font.leftpadding + font.rightpadding) * padding.x;
	float verticalPadding = (font.toppadding + font.bottompadding) * padding.y;

	// cast the gpu virtual address to a textvertex, so we can directly store our vertices there
	TextVertex* vert = (TextVertex*)textVBGPUAddress;

	wchar_t lastChar = -1; // no last character to start with

	for (int i = 0; i < text.size(); ++i)
	{
		wchar_t c = text[i];

		FontChar* fc = font.GetChar(c);

		// character not in font char set
		if (fc == nullptr)
			continue;

		// end of string
		if (c == L'\0')
			break;

		// new line
		if (c == L'\n')
		{
			x = topLeftScreenX;
			y -= (font.lineHeight + verticalPadding) * scale.y;
			continue;
		}

		// don't overflow the buffer. In your app if this is true, you can implement a resize of your text vertex buffer
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

		// remove horrizontal padding and advance to next char position
		x += (fc->xadvance - horrizontalPadding) * scale.x;

		lastChar = c;
	}

	// we are going to have 4 vertices per character (trianglestrip to make quad), and each instance is one character
	pd3dCommandList->DrawInstanced(4, numCharacters, 0, 0);
}
