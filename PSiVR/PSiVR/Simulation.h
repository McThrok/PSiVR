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
	Matrix initial_rotation;
	float delta_time;
	float time;
	bool paused;
	bool gravityUp;

	float cubeSize;
	float simulationSpeed;
	float density;
	Vector3 startVelocity;

	Matrix invI;
	Vector3 g;
	Vector3 r;

	Quaternion Q;
	Vector3 W;
	Matrix I;
	Vector3 G;

	vector<Vector3> probes;
	int probesCount;


	void Init();
	void Reset();
	void Update(float dt);
	void Update();

	Matrix GetWorldMatrix();
	void UpdateTensor();
};

