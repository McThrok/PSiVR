#include "Simulation.h"

void Simulation::Init() {

	default_rotation = XMMatrixRotationY(-XM_PI / 4) * XMMatrixRotationZ(-XM_PI / 4);
	default_rotation = XMMatrixIdentity();

	XMFLOAT3X3 i = XMFLOAT3X3(2.0f / 3.0f, -(1.0f / 4.0f), -(1.0f / 4.0f), -(1.0f / 4.0f), 2.0f / 3.0f, -(1.0f / 4.0f), -(1.0f / 4.0f), -(1.0f / 4.0f), 2.0f / 3.0f);
	I = default_rotation * XMLoadFloat3x3(&i) * XMMatrixTranspose(default_rotation);

	g = { 0,0,-1 };

	Reset();
}

void Simulation::Reset() {
	XMStoreFloat4(&Q[0], XMQuaternionIdentity());
	XMStoreFloat4(&Q[1], XMQuaternionIdentity());

	W[0] = { 0,0,1 };
	W[1] = { 0,0,1 };

	time = 0;
	delta_time = 0.0001;
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
	XMVECTOR new_Q = delta_time * XMVector3Rotate(XMLoadFloat3(&W[1]), XMLoadFloat4(&Q[1])) + XMLoadFloat4(&Q[0]);

	XMVECTOR N = { 0,0,0 };// XMVector3Normalize(XMVector3Rotate(XMLoadFloat3(&g), XMQuaternionInverse(XMLoadFloat4(&Q[1]))));
	//XMVECTOR N = XMVector3TransformNormal(XMVector3Rotate(XMLoadFloat3(&g), XMQuaternionInverse(XMLoadFloat4(&Q[1]))), XMMatrixTranspose(default_rotation));
	//XMVECTOR N = XMVector3TransformNormal(XMLoadFloat3(&g), default_rotation);
	XMVECTOR IWW = XMVector3Cross(XMVector3Transform(XMLoadFloat3(&W[1]), I), XMLoadFloat3(&W[1]));
	XMVECTOR new_W = 2 * delta_time * (XMVector3Transform(N + IWW, XMMatrixInverse(nullptr, I))) + XMLoadFloat3(&W[0]);

	Q[0] = Q[1];
	XMStoreFloat4(&Q[1], XMQuaternionNormalize(new_Q));

	W[0] = W[1];
	XMStoreFloat3(&W[1], new_W);

}

XMMATRIX Simulation::GetWorldMatrix()
{
	return default_rotation * XMMatrixRotationQuaternion(XMLoadFloat4(&Q[1]));
}
