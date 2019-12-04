#include "Simulation.h"

void Simulation::Init()
{
	simulationSpeed = 1;
	density = 1;
	cubeSize = 1;
	initialVelocity = 0;
	paused = false;
	gravityOn = true;
	gravityUp = false;
	maxProbes = 100;
	initialAngle = 0;
	delta_time = 0.001;
	probesCycleCount = 10;

	Reset();
}

void Simulation::Reset()
{
	UpdateTensor();
	Q = Quaternion::Identity;

	W = XMVector3TransformNormal(Vector3(1, 1, 1), initialRotation);
	W.Normalize();
	W *= initialVelocity;

	time = 0;
	probesCounter = 0;
	probes.clear();
}

void Simulation::UpdateTensor()
{
	I = Matrix(XMFLOAT3X3(
		2.0L / 3.0L, -0.25L, -0.25L,
		-0.25L, 2.0L / 3.0L, -0.25L,
		-0.25L, -0.25L, 2.0L / 3.0L));
	m = density * cubeSize* cubeSize* cubeSize;
	I *= m * cubeSize* cubeSize;

	initialRotation = Matrix::CreateRotationZ(-XM_PIDIV4);
	initialRotation *= Matrix::CreateRotationY(XMConvertToRadians(initialAngle) - 0.9553166181f);

	I = initialRotation.Transpose() * I * initialRotation;
	InvI = I.Invert();

	G = Vector3(0, 0, -m * 10L);
	R = XMVector3TransformNormal(Vector3(0.5L, 0.5L, 0.5L), Matrix::CreateScale(cubeSize)* initialRotation);
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
		UpdateProbes();
		time -= timePerStep;
	}
}

Vector3 Simulation::GetN(Quaternion q)
{
	Vector3 N = Vector3(0, 0, 0);
	if (gravityOn)
	{
		Quaternion invQ = Q;
		invQ.Normalize();
		invQ.Conjugate();

		N = XMVector3TransformNormal(XMVector3Cross(R, XMVector3Rotate(G, invQ)), InvI);
	}

	if (gravityUp)
		N = -N;

	return N;
}

void Simulation::Update()
{

	Vector3 N = GetN(Q);
	Vector3 w = W;
	Vector3 k1 = delta_time * (N + (Vector3)XMVector3TransformNormal((Vector3)XMVector3Cross(XMVector3TransformNormal(w, I), w), InvI));
	Quaternion qk1 = delta_time * 0.5L  * Quaternion(w, 0) * Q;


	N = GetN(Q + qk1 * 0.5L);
	w = W + k1 * 0.5L;
	Vector3 k2 = delta_time * (N + (Vector3)XMVector3TransformNormal((Vector3)XMVector3Cross(XMVector3TransformNormal(w, I), w), InvI));
	Quaternion qk2 = delta_time * 0.5L  * Quaternion(w, 0) * (Q + qk1 * 0.5L);


	N = GetN(Q + qk2 * 0.5L);
	w = W + k2 * 0.5L;
	Vector3 k3 = delta_time * (N + (Vector3)XMVector3TransformNormal((Vector3)XMVector3Cross(XMVector3TransformNormal(w, I), w), InvI));
	Quaternion qk3 = delta_time * 0.5L  * Quaternion(w, 0) * (Q + qk2 * 0.5L);


	N = GetN(Q + qk3);
	w = W + k3;
	Vector3 k4 = delta_time * (N + (Vector3)XMVector3TransformNormal((Vector3)XMVector3Cross(XMVector3TransformNormal(w, I), w), InvI));
	Quaternion qk4 = delta_time * 0.5L *  Quaternion(w, 0) * (Q + qk3);


	W = W + Vector3((k1 + 2.0L * k2 + 2.0L * k3 + k4) / 6.0L);
	Q = Q + (Quaternion)((qk1 + 2.0L * qk2 + 2.0L * qk3 + qk4) / 6.0L);
	Q.Normalize();
}

void Simulation::UpdateProbes()
{
	while (probesCounter >= probesCycleCount)
		probesCounter -= probesCycleCount;

	if (probesCounter == 0)
	{
		Vector3 v = XMVector3TransformNormal(Vector3(1, 1, 1), GetModelMatrix());
		probes.push_back(VertexPN(v, { 0,0,0 }));
		if (probes.size() > maxProbes)
			probes.erase(probes.begin(), probes.begin() + probes.size() - maxProbes);
	}

	probesCounter++;
}

Matrix Simulation::GetModelMatrix()
{
	return Matrix::CreateScale(cubeSize) * initialRotation * XMMatrixRotationQuaternion(Q);
}
