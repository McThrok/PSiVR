#pragma once
#include <vector>
#include <math.h> 
#include <DirectXMath.h>

using namespace std;
using namespace DirectX;

class Simulation
{
public:
	float delta_time;
	float time;
	bool paused;

	void Init();
	void Reset();
	void Update(float dt);
};

