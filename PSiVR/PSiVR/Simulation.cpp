#include "Simulation.h"

void Simulation::Init() {

	default_rotation = XMMatrixRotationZ(-XM_PI / 4) * XMMatrixRotationY(-XM_PI / 4);
	default_rotation = XMMatrixIdentity();

	XMFLOAT3X3 i = XMFLOAT3X3(
		2.0f / 3.0f, -0.25, -0.25,
		-0.25, 2.0f / 3.0f, -0.25,
		-0.25, -0.25, 2.0f / 3.0f);

	I = default_rotation * XMLoadFloat3x3(&i) * XMMatrixTranspose(default_rotation);

	g = { 0,0,-1 };

	Reset();
}

void Simulation::Reset() {
	XMStoreFloat4(&Q, XMQuaternionIdentity());
	W = { 0,0,0 };

	time = 0;
	delta_time = 0.001;
	paused = false;
	gravityUp = false;
}

void Simulation::Update(float dt) {
	if (paused)
		return;

	time += dt / 1000;

	while (time >= delta_time)
	{
		Update();
		time -= delta_time;
	}
}

void Simulation::Update() {
	XMVECTOR q = XMLoadFloat4(&Q);
	XMVECTOR w = XMLoadFloat3(&W);

	XMVECTOR k1 = delta_time * 0.5f * XMVector3Rotate(w, q);
	XMVECTOR k2 = delta_time * 0.5f * XMVector3Rotate(w, q + k1 / 2);
	XMVECTOR k3 = delta_time * 0.5f * XMVector3Rotate(w, q + k2 / 2);
	XMVECTOR k4 = delta_time * 0.5f * XMVector3Rotate(w, q + k3);

	XMVECTOR newQ = XMQuaternionNormalize(XMLoadFloat4(&Q) + (k1 + k2 + k3 + k4) / 6);


	XMMATRIX invI = XMMatrixInverse(nullptr, I);
	//XMVECTOR N = XMVector3Normalize(XMVector3TransformNormal(XMVector3Rotate(XMLoadFloat3(&g), XMQuaternionInverse(q)), XMMatrixTranspose(default_rotation)));
	//XMVECTOR N = XMVector3Normalize(XMVector3Rotate(XMVector3TransformNormal(XMLoadFloat3(&g), default_rotation), q));
	XMVECTOR N = XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&g), default_rotation));

	XMVECTOR Iww = XMVector3Cross(XMVector3Transform(w, I), w);
	k1 = delta_time * XMVector3Transform(N + Iww, invI);

	Iww = XMVector3Cross(XMVector3Transform(w + k1 / 2, I), w + k1 / 2);
	k2 = delta_time * XMVector3Transform(N + Iww, invI);

	Iww = XMVector3Cross(XMVector3Transform(w + k2 / 2, I), w + k2 / 2);
	k3 = delta_time * XMVector3Transform(N + Iww, invI);

	Iww = XMVector3Cross(XMVector3Transform(w + k3, I), w + k3);
	k4 = delta_time * XMVector3Transform(N + Iww, invI);

	XMVECTOR newW = XMLoadFloat3(&W) + (k1 + k2 + k3 + k4) / 6;


	XMStoreFloat4(&Q, newQ);
	XMStoreFloat3(&W, newW);
}

XMMATRIX Simulation::GetWorldMatrix()
{
	return default_rotation * XMMatrixRotationQuaternion(XMLoadFloat4(&Q));
}
