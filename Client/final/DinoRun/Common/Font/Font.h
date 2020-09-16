#pragma once
#include "../stdafx.h"


struct FontChar
{
	int id;

	float u; // u좌표
	float v; // v좌표
	float twidth; // 문자당 너비
	float theight; // 문자당 높이

	float width; //스크린 좌표계 너비
	float height; //스크린 좌표계 높이

	float xoffset;
	float yoffset;
	float xadvance; // 다음문자와 이어 붙이도록 하기위해 얼만큼 이동한 지점에서 출력해야하는지를 나타냄
};

struct FontKerning
{
	int firstid; 
	int secondid;
	float amount;
};

struct Font
{
	std::wstring name; // 폰트명
	std::wstring fontImage;
	int size; 
	float lineHeight;
	float baseHeight;
	int textureWidth;
	int textureHeight;
	int numCharacters;
	FontChar* CharList;
	int numKernings;
	FontKerning* KerningsList;
	ComPtr<ID3D12Resource> textureBuffer; 
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandle;

	float leftpadding;
	float toppadding;
	float rightpadding;
	float bottompadding;

	float GetKerning(wchar_t first, wchar_t second)
	{
		for (int i = 0; i < numKernings; ++i)
		{
			if ((wchar_t)KerningsList[i].firstid == first && (wchar_t)KerningsList[i].secondid == second)
				return KerningsList[i].amount;
		}
		return 0.0f;
	}

	FontChar* GetChar(wchar_t c)
	{
		for (int i = 0; i < numCharacters; ++i)
		{
			if (c == (wchar_t)CharList[i].id)
				return &CharList[i];
		}
		return nullptr;
	}
};

Font LoadFont(LPCWSTR filename, int windowWidth, int windowHeight); // load a font
