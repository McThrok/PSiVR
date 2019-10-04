#include "Simulation.h"

void Simulation::Init() {
	delta_time = 0.05;
	m = 0.75;
	c = 5;
	k = 0.1;
	time = 0;
	h_func = 0;
	w_func = 0;

	Reset();
}

void Simulation::Reset() {
	x.clear();
	xt.clear();
	xtt.clear();
	t.clear();
	w.clear();
	h.clear();

	x.push_back(1);
	xt.push_back(0);
	xtt.push_back(0);
	t.push_back(-delta_time * 2);
	w.push_back(0);
	h.push_back(0);

	x.push_back(1);
	xt.push_back(0);
	xtt.push_back(0);
	AddNextT();
	w.push_back(0);
	h.push_back(0);

	AddNextX();
	AddNextXt();
	AddNextXtt();
	AddNextT();
	AddNextW();
	AddNextH();
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

	AddNextX();
	AddNextXt();
	AddNextXtt();
	AddNextT();
	AddNextW();
	AddNextH();
}

void Simulation::AddNextX() {
	float x0 = x.rbegin()[1];
	float x1 = x.back();
	float result = (2 * x1 - x0 + (delta_time *delta_time / m) * (c*(w.back() - x1) + k * x0 / (2 * delta_time) + h.back())) / (1 + k * delta_time / (2 * m));
	x.push_back(result);

}

void Simulation::AddNextXt() {
	float x0 = x.back();
	float x2 = x.rbegin()[2];
	float result = (x2 - x0) / (2 * delta_time);
	xt.push_back(result);
}

void Simulation::AddNextXtt() {
	float x0 = x.back();
	float x1 = x.rbegin()[1];
	float x2 = x.rbegin()[2];
	float result = (x2 - 2 * x1 + x0) / (delta_time * delta_time);
	xtt.push_back(result);
}

void Simulation::AddNextT() {
	t.push_back(t.back() + delta_time);
}

void Simulation::AddNextW() {
	w.push_back(GetFunc(w_func));
}

void Simulation::AddNextH() {
	h.push_back(GetFunc(h_func));
}

float Simulation::GetFunc(int func) {
	float a = 1;
	float fi = 0;
	float om = 1;

	switch (func)
	{
	case 1: return a;
	case 2: return t.back() > 0 ? a : 0;
	case 3:
	{
		float val = sin(om * t.back() + fi);
		return a * (val > 0 ? 1 : val < 0 ? -1 : 0);
	}
	case 4:		return a * sin(om * t.back() + fi);
	default: return 0;
	}
}