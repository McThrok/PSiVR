#pragma once
#include <vector>

class Simulation
{
public:
	float m, c, w, h, k, delta_time, time;
	int iterations_limit = 500;
	bool paused = false;

	std::vector<float> x;
	std::vector<float> xt;
	std::vector<float> xtt;
	std::vector<float> t;

	Simulation() {}
	void Init();
	void Reset();
	void Update(float dt);

private:
	float GetNextX();
	float GetNextXt();
	float GetNextXtt();
};

