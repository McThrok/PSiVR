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
	mat3 initialRotationQwe;
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

	mat3 InvIQwe;
	vec3 GQwe;
	vec3 RQwe;
	quat QQwe;
	vec3 WQwe;
	mat3 IQwe;

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

