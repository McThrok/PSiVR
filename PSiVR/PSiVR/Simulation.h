#pragma once
#include <d3d11.h>
#include <vector>
#include <math.h> 
#include <DirectXMath.h>
#include <SimpleMath.h>
#include "Graphics/Vertex.h"
#include <random>

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

class Simulation
{
public:

	Vector3 lb, ub;
	Vector3 f[2][2][2];

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

	Vector3 frameRotation;
	Vector3 framePosition;

	float delta_time;
	float time;
	bool paused;

	float cubeSize;
	float simulationSpeed;
	float m, c, cFrame, kk, kkFrame, mi, randomFactor;
	bool elastic, reduceAll;

	mt19937 gen{ std::random_device{}() };

	void Init();
	void Reset();
	void Update(float dt);
	void Update();

	Vector3 GetPart(int _i, int _j, int _k, Vector3 t[4][4][4], Vector3 tk[4][4][4], float ta, bool useL);
	Vector3 GetPartFrame(int _i, int _j, int _k, Vector3 t[4][4][4], Vector3 tk[4][4][4], float ta, bool p);
	float GetDiff(int i, int  j, int  k, int  _i, int  _j, int _k);
	void ApplyCollisions();

	void AdjustFrame(Vector3 v);
	Matrix GetFrameMatrix();
};

