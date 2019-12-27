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
	Vector3 f[4][4][4];

	Vector3 p[4][4][4];
	Vector3 pk1[4][4][4];
	Vector3 pk2[4][4][4];
	Vector3 pk3[4][4][4];
	Vector3 pk4[4][4][4];

	Vector3 v[4][4][4];
	Vector3 vk1[4][4][4];
	Vector3 vk2[4][4][4];
	Vector3 vk3[4][4][4];
	Vector3 vk4[4][4][4];

	float delta_time;
	float time;
	bool paused;

	float cubeSize;
	float simulationSpeed;
	float m, c, k, l0;

	void Init();
	void Reset();
	void Update(float dt);
	void Update();

	Vector3 GetPart(int _i, int _j, int _k, Vector3 t[4][4][4], Vector3 tk[4][4][4], float ta, bool useL);
	float GetDiff(int i, int  j, int  k, int  _i, int  _j, int _k);

	void AdjustFrame(Vector3 v);

};

