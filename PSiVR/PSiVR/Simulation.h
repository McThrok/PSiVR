#pragma once
#include <d3d11.h>
#include <vector>
#include <math.h> 
#include <DirectXMath.h>
#include <SimpleMath.h>
#include "Graphics/Vertex.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

class Simulation
{
public:

	Vector3 lb, ub;
	vector<vector<vector<Vector3>>> f;

	vector<vector<vector<Vector3>>> p, pPrev, pPrevPrev;
	vector<vector<vector<Vector3>>> v, vPrev, vPrevPrev;

	float delta_time;
	float time;
	bool paused;

	float cubeSize;
	float simulationSpeed;
	float m, c, k;

	void Init();
	void Reset();
	void Update(float dt);
	void Update();
	void AdjustFrame(Vector3 v);

	void InitVector(vector<vector<vector<Vector3>>>& v);

};

