#include "Simulation.h"

void Simulation::Init() {

	default_rotation = XMMatrixRotationY(-XM_PI / 4) * XMMatrixRotationZ(-XM_PI / 4);
	Reset();
}

void Simulation::Reset() {
	Q[0] = { 0,0,0,0 };
	Q[1] = { 0,0,0,0 };

	W[0] = { 0,0,0 };
	W[1] = { 0,0,0 };

	time = 0;
}

void Simulation::Update(float dt) {
	if (paused)
		return;

	time += dt;

	while (time >= delta_time)
	{
		Update();
		time -= delta_time;
	}
}

void Simulation::Update() {
}

XMMATRIX Simulation::GetWorldMatrix()
{

	return XMMATRIX();
}
