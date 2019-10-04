#pragma once
#include <vector>

class Simulation
{
public:
	float m, c, w, h, k, time_break, time;
	std::vector<float> x;
	std::vector<float> t;
	std::vector<float> v;

	Simulation() {}
	void Init();
	void Update(float dt);

private:
	float GetNextX();
	float GetNextV();
};

