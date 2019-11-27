#pragma once
#include <d3d11.h>
#include <vector>
#include <math.h> 
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "Graphics/Vertex.h"
#include "Geometry/quat.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

class Simulation
{
public:
	mat3 initialRotation;
	float delta_time;
	float time;
	bool paused;
	bool gravityOn;

	int initialAngle;
	float cubeSize;
	float simulationSpeed;
	float density;
	float initialVelocity;
	float m;

	mat3 InvI;
	vec3 G;
	vec3 R;
	quat Q;
	vec3 W;
	mat3 I;

	vector<VertexPN> probes;
	int maxProbes;
	int probesCycleCount;
	int probesCounter;


	void Init();
	void Reset();
	void Update(float dt);
	void Update();

	Matrix GetModelMatrix();
	void UpdateTensor();
	void UpdateProbes();
};

