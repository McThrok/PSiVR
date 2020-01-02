#pragma once
#include <DirectXMath.h>
#include <d3d11.h>

struct VertexP
{
	VertexP() {}

	VertexP(DirectX::XMFLOAT3 _pos)
		: pos(_pos) {}

	VertexP(float x, float y, float z)
		: pos(x, y, z) {}

	DirectX::XMFLOAT3 pos;

	static D3D11_INPUT_ELEMENT_DESC layout[1];
};

struct VertexPN
{
	VertexPN() {}

	VertexPN(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _normal)
		: pos(_pos), normal(_normal) {}

	VertexPN(float x, float y, float z, float nx, float ny, float nz)
		: pos(x, y, z), normal(nx, ny, nz) {}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;

	static D3D11_INPUT_ELEMENT_DESC layout[2];
};

struct VertexPT
{
	VertexPT() {}

	VertexPT(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT2 _tex)
		: pos(_pos), tex(_tex) {}

	VertexPT(float x, float y, float z, float u, float v)
		: pos(x, y, z), tex(u,v) {}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 tex;

	static D3D11_INPUT_ELEMENT_DESC layout[2];
};


struct VertexPNT
{
	VertexPNT() {}

	VertexPNT(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _normal, DirectX::XMFLOAT2 _tex)
		: pos(_pos), normal(_normal), tex(_tex) {}

	VertexPNT(float x, float y, float z, float nx, float ny, float nz, float u, float v)
		: pos(x, y, z), normal(nx, ny, nz), tex(u, v) {}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 tex;

	static D3D11_INPUT_ELEMENT_DESC layout[3];
};