#pragma once
#include <vector>
#include <math.h> 


class Simulation
{
public:
	float m, c, k, delta_time, time;
	int iterations_limit = 500;
	int h_func, w_func;
	bool paused = false;

	std::vector<float> x;
	std::vector<float> xt;
	std::vector<float> xtt;
	std::vector<float> t;
	std::vector<float> w;
	std::vector<float> h;

	Simulation() {}
	void Init();
	void Reset();
	void Update(float dt);

private:
	void AddNextX();
	void AddNextXt();
	void AddNextXtt();
	void AddNextT();
	void AddNextW();
	void AddNextH();
	float GetFunc(int func);
};

