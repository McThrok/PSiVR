#include "Simulation.h"

void Simulation::Init() {
	delta_time = 0.05;
	m = 25;
	c = 300;
	k = 3;
	w = 0;
	h = 0;
	time = 0;

	Reset();
}

void Simulation::Reset() {
	x.clear();
	xt.clear();
	xtt.clear();
	t.clear();

	x.push_back(1);
	xt.push_back(0);
	xtt.push_back(0);
	t.push_back(-delta_time * 2);

	x.push_back(1);
	xt.push_back(0);
	xtt.push_back(0);
	t.push_back(-delta_time);

	x.push_back(1);
	xt.push_back(GetNextXt());
	xtt.push_back(GetNextXtt());
	t.push_back(0);
}

void Simulation::Update(float dt) {
	if (paused)
		return;
	
	if (x.size() > iterations_limit)
		Reset();

	time += dt;
	if (time < delta_time * 1000)
		return;

	time -= delta_time * 1000;
	t.push_back(t.back() + delta_time);
	x.push_back(GetNextX());
	xt.push_back(GetNextXt());
	xtt.push_back(GetNextXtt());
}

float Simulation::GetNextX() {
	float x0 = x.rbegin()[1];
	float x1 = x.back();
	return (2 * x1 - x0 + (delta_time *delta_time / m) * (c*(w - x1) + k * x0 / (2 * delta_time))) / (1 + k * delta_time/(2 * m) );

}

float Simulation::GetNextXt() {
	float x0 = x.back();
	float x2 = x.rbegin()[2];
	return (x2 - x0) / (2 * delta_time);

}

float Simulation::GetNextXtt() {
	float x0 = x.back();
	float x1 = x.rbegin()[1];
	float x2 = x.rbegin()[2];
	return (x2 - 2 * x1 + x0) / (delta_time * delta_time);

}
