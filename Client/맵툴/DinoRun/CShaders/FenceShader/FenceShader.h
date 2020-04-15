#pragma once
#pragma once
#include "../Shader.h"
class RectShader : public CObInstancingShader
{
public:
	RectShader();
	virtual ~RectShader();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* terrain);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	virtual void Add(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, const XMFLOAT4X4& matrix);
	virtual void Delete();
	virtual void Save();
	virtual void Load(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
};


class MFenceShader : public CObInstancingShader
{
public:
	MFenceShader();
	virtual ~MFenceShader();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* terrain);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	virtual void Add(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, const XMFLOAT4X4& matrix);
	virtual void Delete();
	virtual void Save();
	virtual void Load(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
};


class LFenceShader : public CObInstancingShader
{
public:
	LFenceShader();
	virtual ~LFenceShader();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* terrain);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	virtual void Add(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, const XMFLOAT4X4& matrix);
	virtual void Delete();
	virtual void Save();
	virtual void Load(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
};


class MeatShader : public CObInstancingShader
{
public:
	MeatShader();
	virtual ~MeatShader();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* terrain);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	virtual void Add(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, const XMFLOAT4X4& matrix);
	virtual void Delete();
	virtual void Save();
	virtual void Load(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
};
