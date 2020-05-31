#include "FontShader.h"

#include "../../Common/FrameWork/CreateManager.h"

FontShader::FontShader()
{
	m_nReferences = 1;
}
FontShader::~FontShader()
{

}

void FontShader::BuildObjects(CreateManager* pCreateManager, void* pInformation)
{
	
	arialFont = LoadFont(L"Resources/Fonts/Arial.fnt", FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

	fontTex = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	fontTex->LoadTextureFromFile(pCreateManager->GetDevice().Get(), pCreateManager->GetCommandList().Get(), arialFont.fontImage.c_str(), 0);

	CreateCbvSrvDescriptorHeaps(pCreateManager, 0, 1);

	CreateShaderResourceViews(pCreateManager, fontTex, 8, true);

	HRESULT hr;
	//18개의 텍스트 출력을 위한 정점 버퍼 생성.
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
	if (textVertexBuffer)
	{
		for (int i = 0; i < 18; ++i)
		{
			if (textVertexBuffer[i])
			{
				textVertexBuffer[i]->Unmap(0, NULL);
				textVertexBuffer[i]->Release();
				textVertexBuffer[i] = NULL;
			}
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

	float leftRightPadding = (font.leftpadding + font.rightpadding) * padding.x; //텍스처 상 공간에서의 안쪽의 여백너비
	float upDownPadding = (font.toppadding + font.bottompadding) * padding.y; //텍스처 상 공간에서의 안쪽의 여백너비

	TextVertex* vert = (TextVertex*)textVBGPUAddress[idx];

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
			y -= (font.lineHeight + upDownPadding) * scale.y;
			continue;
		}

		if (numCharacters >= maxNumTextCharacters)
			break;
		
		vert[numCharacters] = TextVertex(color.x,
			color.y,
			color.z,
			color.w,
			fc->u,
			fc->v,
			fc->twidth,
			fc->theight,
			x + (fc->xoffset * scale.x),   
			y - (fc->yoffset * scale.y),
			fc->width * scale.x,
			fc->height * scale.y);
			
		numCharacters++;

		x += (fc->xadvance - leftRightPadding) * scale.x; //leftRightPadding은 가로축의 빈공간 
		//텍스처에 각 문자간격마다 빈공간이 있는데 이 빈공간을 없애기 위한 패딩값임. xadvance는 
		//한 문자의 크기(빈공간 포함)
	}

	pd3dCommandList->DrawInstanced(4, numCharacters, 0, 0);
}
