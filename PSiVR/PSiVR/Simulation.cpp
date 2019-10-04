#include "Simulation.h"

void Simulation::Init() {
	x.push_back(1);
	x.push_back(1);

	time_break = 0.1;
	t.push_back(-time_break);
	t.push_back(0);

	v.push_back(0);
	v.push_back(0);

	m = 100;
	c = 1000;
	k = 0.001;
	w = 0;
	h = 0;
	time = 0;
}

void Simulation::Update(float dt) {
	time += dt;
	if (time < time_break * 1000)
		return;

	time -= time_break * 1000;
	t.push_back(t.back() + time_break);
	x.push_back(GetNextX());
	v.push_back(GetNextV());
}

float Simulation::GetNextX() {
	float x0 = x.rbegin()[1];
	float x1 = x.back();
	//return (2 * x0 - x1 + (time_break*time_break / m)*(c*(w - x0) + k * x1 / (2 * time_break)))*(1 + k * time_break / (2 * m));
	return 2 * x1 - x0 + (time_break * time_break / m) * (c * (w - x1));

}

float Simulation::GetNextV() {
	float x0 = x.back();
	float x2 = x.rbegin()[2];
	return (x2 + x0) / (2 * time_break);

}
