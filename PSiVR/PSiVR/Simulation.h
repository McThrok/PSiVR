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
	Matrix initialRotation;
	float delta_time;
	float time;
	bool paused;
	bool gravityUp;

	int initialAngle;
	float cubeSize;
	float simulationSpeed;
	float density;
	Vector3 initialVelocity;

	Matrix InvI;
	Vector3 G;
	Vector3 R;

	Quaternion Q;
	Vector3 W;
	Matrix I;

	vector<Vector3> probes;
	int probesCount;


	void Init();
	void Reset();
	void Update(float dt);
	void Update();

	Matrix GetWorldMatrix();
	void UpdateTensor();
};

