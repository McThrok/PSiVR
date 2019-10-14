#pragma once
#include <vector>
#include <math.h> 
#include <DirectXMath.h>

using namespace std;
using namespace DirectX;

class Simulation
{
public:
	float m, c, k, delta_time, time;
	int h_func, w_func;
	bool paused = false;

	vector<float> x;
	vector<float> xt;
	vector<float> xtt;
	vector<float> t;
	vector<float> w;
	vector<float> h;


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

