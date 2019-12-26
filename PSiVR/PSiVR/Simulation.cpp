#include "Simulation.h"

void Simulation::Init()
{
	m = 1;
	c = 1;
	k = 10;

	time = 0;
	simulationSpeed = 1;
	cubeSize = 2;
	paused = false;
	delta_time = 0.01;

	float boxSize = 10;
	lb = -(boxSize / 2) * Vector3(1, 1, 1);
	ub = (boxSize / 2) * Vector3(1, 1, 1);

	InitVector(p);
	InitVector(pPrev);
	InitVector(pPrevPrev);
	InitVector(f);
	InitVector(v);
	InitVector(vPrev);
	InitVector(vPrevPrev);

	Reset();
}

void Simulation::InitVector(vector<vector<vector<Vector3>>>& v)
{
	v = vector<vector<vector<Vector3>>>(4);
	for (int i = 0; i < 4; i++)
	{
		v[i] = vector<vector<Vector3>>(4);
		for (int j = 0; j < 4; j++)
			v[i][j] = vector<Vector3>(4);
	}
}

void Simulation::Reset()
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 4; k++)
			{
				p[i][j][k] = Vector3(i, j, k) * cubeSize / 3;
				p[i][j][k] -= Vector3(1, 1, 1) * (cubeSize / 2);
				pPrev[i][j][k] = p[i][j][k];
				pPrevPrev[i][j][k] = p[i][j][k];
				f[i][j][k] = p[i][j][k];

				v[i][j][k] = Vector3(0, 0, 0);
				vPrev[i][j][k] = v[i][j][k];
				vPrevPrev[i][j][k] = v[i][j][k];
			}
}


void Simulation::Update(float dt)
{
	if (paused)
		return;

	time += dt / 1000;
	float timePerStep = delta_time / simulationSpeed;

	while (time >= timePerStep)
	{
		Update();
		time -= timePerStep;
	}
}

void Simulation::Update()
{
	 
	//Vector3 N = GetN(Q);
	//Vector3 w = W;
	//Vector3 k1 = delta_time * (N + (Vector3)XMVector3TransformNormal((Vector3)XMVector3Cross(XMVector3TransformNormal(w, I), w), InvI));
	//Quaternion qk1 = delta_time * 0.5L * Quaternion(w, 0) * Q;


	//N = GetN(Q + qk1 * 0.5L);
	//w = W + k1 * 0.5L;
	//Vector3 k2 = delta_time * (N + (Vector3)XMVector3TransformNormal((Vector3)XMVector3Cross(XMVector3TransformNormal(w, I), w), InvI));
	//Quaternion qk2 = delta_time * 0.5L * Quaternion(w, 0) * (Q + qk1 * 0.5L);


	//N = GetN(Q + qk2 * 0.5L);
	//w = W + k2 * 0.5L;
	//Vector3 k3 = delta_time * (N + (Vector3)XMVector3TransformNormal((Vector3)XMVector3Cross(XMVector3TransformNormal(w, I), w), InvI));
	//Quaternion qk3 = delta_time * 0.5L * Quaternion(w, 0) * (Q + qk2 * 0.5L);


	//N = GetN(Q + qk3);
	//w = W + k3;
	//Vector3 k4 = delta_time * (N + (Vector3)XMVector3TransformNormal((Vector3)XMVector3Cross(XMVector3TransformNormal(w, I), w), InvI));
	//Quaternion qk4 = delta_time * 0.5L * Quaternion(w, 0) * (Q + qk3);


	//W = W + Vector3((k1 + 2.0L * k2 + 2.0L * k3 + k4) / 6.0L);
	//Q = Q + (Quaternion)((qk1 + 2.0L * qk2 + 2.0L * qk3 + qk4) / 6.0L);
	//Q.Normalize();
}

void Simulation::AdjustFrame(Vector3 v)
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 4; k++)
				f[i][j][k] += v;
}
