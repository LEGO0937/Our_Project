#pragma once
#include "../stdafx.h"


struct FontChar
{
	int id;

	float u; // u��ǥ
	float v; // v��ǥ
	float twidth; // ���ڴ� �ʺ�
	float theight; // ���ڴ� ����

	float width; //��ũ�� ��ǥ�� �ʺ�
	float height; //��ũ�� ��ǥ�� ����

	float xoffset;
	float yoffset;
	float xadvance; // �������ڿ� �̾� ���̵��� �ϱ����� ��ŭ �̵��� �������� ����ؾ��ϴ����� ��Ÿ��
};

struct FontKerning
{
	int firstid; 
	int secondid;
	float amount;
};

struct Font
{
	std::wstring name; // ��Ʈ��
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
