#include "Simulation.h"

void Simulation::Init()
{
	m = 1;
	c = 10;
	kk = 1;
	l0 = 1;

	time = 0;
	simulationSpeed = 1;
	cubeSize = 2;
	paused = false;
	delta_time = 0.001;

	float boxSize = 10;
	lb = -(boxSize / 2) * Vector3(1, 1, 1);
	ub = (boxSize / 2) * Vector3(1, 1, 1);

	Reset();
}

void Simulation::Reset()
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 4; k++)
			{
				p[i][j][k] = Vector3(i, j, k) * cubeSize / 3;
				p[i][j][k] -= Vector3(1, 1, 1) * (cubeSize / 2);

				v[i][j][k] = Vector3(0, 0, 0);
			}

	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			for (int k = 0; k < 2; k++)
				f[i][j][k] = p[3 * i][3 * j][3 * k];
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

void Simulation::AdjustFrame(Vector3 v)
{
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			for (int k = 0; k < 2; k++)
				f[i][j][k] += v;
}

void Simulation::Update()
{
	//int j = 0, k = 0, i = 0;

	////for (int i = 0; i < 4; i++)
	//for (int k = 0; k < 4; k++)
	//{
	//	vk1[i][j][k] = delta_time * (-c * GetPart(i, j, k, p, p, 0, true) - kk * GetPart(i, j, k, v, v, 0, false)) / m;
	//	pk1[i][j][k] = delta_time * v[i][j][k];
	//}

	////for (int i = 0; i < 4; i++)
	//for (int k = 0; k < 4; k++)
	//{
	//	vk2[i][j][k] = delta_time * (-c * GetPart(i, j, k, p, pk1, 0.5f, true) - kk * GetPart(i, j, k, v, vk1, 0.5f, false)) / m;
	//	pk2[i][j][k] = delta_time * (v[i][j][k] + vk1[i][j][k] * 0.5f);
	//}

	////for (int i = 0; i < 4; i++)
	//for (int k = 0; k < 4; k++)
	//{
	//	vk3[i][j][k] = delta_time * (-c * GetPart(i, j, k, p, pk2, 0.5f, true) - kk * GetPart(i, j, k, v, vk2, 0.5f, false)) / m;
	//	pk3[i][j][k] = delta_time * (v[i][j][k] + vk2[i][j][k] * 0.5f);
	//}
	//
	////for (int i = 0; i < 4; i++)
	//for (int k = 0; k < 4; k++)
	//{
	//	vk4[i][j][k] = delta_time * (-c * GetPart(i, j, k, p, pk3, 1, true) - kk * GetPart(i, j, k, v, vk3, 1, false)) / m;
	//	pk4[i][j][k] = delta_time * (v[i][j][k] + vk3[i][j][k]);
	//}

	////for (int i = 0; i < 4; i++)
	//for (int k = 0; k < 4; k++)
	//{
	//	v[i][j][k] += (vk1[i][j][k] + 2 * vk2[i][j][k] + 2 * vk3[i][j][k] + vk4[i][j][k]) / 6;
	//	p[i][j][k] += (pk1[i][j][k] + 2 * pk2[i][j][k] + 2 * pk3[i][j][k] + pk4[i][j][k]) / 6;
	//}
	//return;

	////------------------------------------------------------
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 4; k++)
			{
				vk1[i][j][k] = delta_time * (-c * GetPart(i, j, k, p, p, 0, true) - kk * GetPart(i, j, k, v, v, 0, false)) / m;
				pk1[i][j][k] = delta_time * v[i][j][k];
			}

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 4; k++)
			{
				vk2[i][j][k] = delta_time * (-c * GetPart(i, j, k, p, pk1, 0.5f, true) - kk * GetPart(i, j, k, v, vk1, 0.5f, false)) / m;
				pk2[i][j][k] = delta_time * (v[i][j][k] + vk1[i][j][k] * 0.5f);
			}

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 4; k++)
			{
				vk3[i][j][k] = delta_time * (-c * GetPart(i, j, k, p, pk2, 0.5f, true) - kk * GetPart(i, j, k, v, vk2, 0.5f, false)) / m;
				pk3[i][j][k] = delta_time * (v[i][j][k] + vk2[i][j][k] * 0.5f);
			}

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 4; k++)
			{
				vk4[i][j][k] = delta_time * (-c * GetPart(i, j, k, p, pk3, 1, true) - kk * GetPart(i, j, k, v, vk3, 1, false)) / m;
				pk4[i][j][k] = delta_time * (v[i][j][k] + vk3[i][j][k]);
			}

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 4; k++)
			{
				v[i][j][k] += (vk1[i][j][k] + 2 * vk2[i][j][k] + 2 * vk3[i][j][k] + vk4[i][j][k]) / 6;
				p[i][j][k] += (pk1[i][j][k] + 2 * pk2[i][j][k] + 2 * pk3[i][j][k] + pk4[i][j][k]) / 6;
			}
}

Vector3 Simulation::GetPart(int _i, int _j, int _k, Vector3 t[4][4][4], Vector3 tk[4][4][4], float ta, bool useL)
{
	Vector3 sum = Vector3::Zero;

	for (int i = _i - 1; i <= _i + 1; i++)
		for (int j = _j - 1; j <= _j + 1; j++)
			for (int k = _k - 1; k <= _k + 1; k++)
			{
				if (i == _i && j == _j && k == _k) continue;
				if (i != _i && j != _j && k != _k) continue;
				if (i < 0 || j < 0 || k < 0 || i > 3 || j > 3 || k > 3) continue;

				//if (k != _k || j != _j) continue;

				Vector3 vec = (t[_i][_j][_k] + ta * tk[_i][_j][_k]) - (t[i][j][k] + ta * tk[i][j][k]);
				if (useL)
				{
					Vector3 tmp = vec;
					//if (vec.Length() < 0.0001)
					//	vec = { 1,0,0 };
					tmp.Normalize();
					tmp *= -l0 * sqrt(GetDiff(i, j, k, _i, _j, _k));
					vec += tmp;

					//float length = vec.Length() - l0 * sqrt(GetDiff(i, j, k, _i, _j, _k));
					//vec.Normalize();
					//vec *= length;
				}
				sum += vec;
			}

	return sum;
}
float Simulation::GetDiff(int i, int  j, int  k, int  _i, int  _j, int _k)
{
	return abs(i - _i) + abs(j - _j) + abs(k - _k);
}
