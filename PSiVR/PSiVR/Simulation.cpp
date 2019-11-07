#include "Simulation.h"

void Simulation::Init() {
	Reset();
}

void Simulation::Reset() {
}

void Simulation::Update(float dt) {
	if (paused)
		return;

	time += dt;

	while (time >= delta_time)
	{
		time -= delta_time;
	}
}
