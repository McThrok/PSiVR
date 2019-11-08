#include "Simulation.h"

void Simulation::Init() {

	default_rotation = XMMatrixRotationY(-XM_PI / 4) * XMMatrixRotationZ(-XM_PI / 4);
	default_rotation = XMMatrixIdentity();

	g = { 0,0,1 };

	Reset();
}

void Simulation::Reset() {
	Q[0] = { 0,0,0,0 };
	Q[1] = { 0,0,0,0 };

	W[0] = { 0,0,0 };
	W[1] = { 0,0,0 };

	time = 0;
	delta_time = 0.01;
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
	XMVECTOR N = XMVector3TransformNormal(XMLoadFloat3(&g), default_rotation);
	XMVECTOR IWW = XMVector3Cross(XMVector3Transform(XMLoadFloat3(&W[1]), I), XMLoadFloat3(&W[1]));
	XMVECTOR new_W = 2 * delta_time * (XMVector3Transform(IWW, XMMatrixInverse(nullptr, I))) + XMLoadFloat3(&W[0]);

	//W[1].x += 0.02;
	//W[1].y += 0.02;
	//W[1].z += 0.02;
}

XMMATRIX Simulation::GetWorldMatrix()
{
	return default_rotation * XMMatrixRotationZ(W[1].z) * XMMatrixRotationY(W[1].y) * XMMatrixRotationX(W[1].x);
}
