#pragma once
#include <vector>
#include <math.h> 
#include <DirectXMath.h>

using namespace std;
using namespace DirectX;

class Simulation
{
public:
	XMMATRIX default_rotation;
	float delta_time;
	float time;
	bool paused;

	XMFLOAT4 Q[2];
	XMFLOAT3 W[2];
	XMMATRIX I;

	void Init();
	void Reset();
	void Update(float dt);
	void Update();

	XMMATRIX GetWorldMatrix();
};

