#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <SimpleMath.h>

using namespace DirectX::SimpleMath;

struct VertexP
{
	VertexP() {}

	VertexP(Vector3 _pos)
		: pos(_pos) {}

	VertexP(float x, float y, float z)
		: pos(x, y, z) {}

	Vector3 pos;

	static D3D11_INPUT_ELEMENT_DESC layout[1];
};

struct VertexPN
{
	VertexPN() {}

	VertexPN(Vector3 _pos, Vector3 _normal)
		: pos(_pos), normal(_normal) {}

	VertexPN(float x, float y, float z, float nx, float ny, float nz)
		: pos(x, y, z), normal(nx, ny, nz) {}

	Vector3 pos;
	Vector3 normal;

	static D3D11_INPUT_ELEMENT_DESC layout[2];
};

struct VertexPT
{
	VertexPT() {}

	VertexPT(Vector3 _pos, Vector2 _tex)
		: pos(_pos), tex(_tex) {}

	VertexPT(float x, float y, float z, float u, float v)
		: pos(x, y, z), tex(u,v) {}

	Vector3 pos;
	Vector2 tex;

	static D3D11_INPUT_ELEMENT_DESC layout[2];
};

struct VertexPT3
{
	VertexPT3() {}

	VertexPT3(Vector3 _pos, Vector3 _tex)
		: pos(_pos), tex(_tex) {}

	VertexPT3(float x, float y, float z, float u, float v, float w)
		: pos(x, y, z), tex(u, v,w) {}

	Vector3 pos;
	Vector3 tex;

	static D3D11_INPUT_ELEMENT_DESC layout[2];
};


struct VertexPNT
{
	VertexPNT() {}

	VertexPNT(Vector3 _pos, Vector3 _normal, Vector2 _tex)
		: pos(_pos), normal(_normal), tex(_tex) {}

	VertexPNT(float x, float y, float z, float nx, float ny, float nz, float u, float v)
		: pos(x, y, z), normal(nx, ny, nz), tex(u, v) {}

	Vector3 pos;
	Vector3 normal;
	Vector2 tex;

	static D3D11_INPUT_ELEMENT_DESC layout[3];
};