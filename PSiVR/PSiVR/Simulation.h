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
	bool gravityUp;

	XMFLOAT4 Q;
	XMFLOAT3 W;
	XMMATRIX I;
	XMFLOAT3 G;
	

	void Init();
	void Reset();
	void Update(float dt);
	void Update();

	XMMATRIX GetWorldMatrix();
};

