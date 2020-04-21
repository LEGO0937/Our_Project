#pragma once
#include "../stdafx.h"


struct FontChar
{
	int id;

	float u; 
	float v;
	float twidth; 
	float theight; 

	float width; 
	float height; 

	
	float xoffset;
	float yoffset; 
	float xadvance; 
};

struct FontKerning
{
	int firstid;
	int secondid; 
	float amount;
};

struct Font
{
	std::wstring name; 
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
	ID3D12Resource* textureBuffer; 
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
 
Font LoadFont(LPCWSTR filename, int windowWidth, int windowHeight);
