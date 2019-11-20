#pragma once
#include <d3d11.h>
#include <vector>
#include <math.h> 
#include <DirectXMath.h>
#include <SimpleMath.h>

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

class Simulation
{
public:
	Matrix default_rotation;
	float delta_time;
	float time;
	bool paused;
	bool gravityUp;

	float simulationSpeed;
	float density;
	Vector3 startVelocity;

	Quaternion Q;
	Vector3 W;
	Matrix I;
	Vector3 G;


	void Init();
	void Reset();
	void Update(float dt);
	void Update();

	Matrix GetWorldMatrix();
};

