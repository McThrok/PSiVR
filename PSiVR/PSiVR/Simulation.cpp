#include "Simulation.h"

void Simulation::Init()
{
	simulationSpeed = 1;
	density = 1;
	cubeSize = 1;
	initialVelocity = 0;
	paused = false;
	gravityOn = true;
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

	W = Vector3(1, 1, 1);
	W.Normalize();
	W *= initialVelocity;

	time = 0;
	probesCounter = 0;
	probes.clear();
}

void Simulation::UpdateTensor()
{
	I = XMFLOAT3X3(
		2.0f / 3.0f, -0.25, -0.25,
		-0.25, 2.0f / 3.0f, -0.25,
		-0.25, -0.25, 2.0f / 3.0f);

	m = density;
	I *= m;

	InvI = I.Transpose();

	initialRotation = XMMatrixRotationZ(-XM_PIDIV4);
	initialRotation *= XMMatrixRotationY(-0.955316618);//XMMatrixRotationY(-XMScalarACos(sqrtf(3.0f) / 3.0f));
	initialRotation *= XMMatrixRotationY(XMConvertToRadians(initialAngle));

	G = XMVector3Normalize(XMVector3TransformNormal(Vector3(0, 0, -m), initialRotation.Transpose()));
	R = Vector3(0.5f, 0.5f, 0.5f);
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

void Simulation::Update()
{
	Quaternion newQ;
	{
		Quaternion w0 = Quaternion(W, 0);
		Quaternion k1 = delta_time * 0.5f * XMQuaternionMultiply(w0, Q);
		Quaternion k2 = delta_time * 0.5f * XMQuaternionMultiply(w0, (Q + k1 * 0.5f));
		Quaternion k3 = delta_time * 0.5f * XMQuaternionMultiply(w0, (Q + k2 * 0.5f));
		Quaternion k4 = delta_time * 0.5f * XMQuaternionMultiply(w0, (Q + k3));

		newQ = XMQuaternionNormalize(Q + (Quaternion)((k1 + 2.0f * k2 + 2.0f * k3 + k4) / 6.0f));
	}

	Vector3 newW;
	{
		
		Vector3 N = Vector3(0, 0, 0);
		if (gravityOn)
			N = XMVector3Cross(R, XMVector3Rotate(G, XMQuaternionInverse(Q)));
		
		Vector3 Iww = XMVector3Cross(XMVector3TransformNormal(W, I), W);
		Vector3 k1 = delta_time * XMVector3TransformNormal(N + Iww, InvI);

		Iww = XMVector3Cross(XMVector3TransformNormal(W + k1 * 0.5f, I), W + k1 * 0.5f);
		Vector3 k2 = delta_time * XMVector3TransformNormal(N + Iww, InvI);

		Iww = XMVector3Cross(XMVector3TransformNormal(W + k2 * 0.5f, I), W + k2 * 0.5f);
		Vector3 k3 = delta_time * XMVector3TransformNormal(N + Iww, InvI);

		Iww = XMVector3Cross(XMVector3TransformNormal(W + k3, I), W + k3);
		Vector3 k4 = delta_time * XMVector3TransformNormal(N + Iww, InvI);

		newW = W + (Vector3)((k1 + 2.0f * k2 + 2.0f * k3 + k4) / 6.0f);
	}

	Q = newQ;
	W = newW;
}

void Simulation::UpdateProbes()
{
	while (probesCounter >= probesCycleCount)
		probesCounter -= probesCycleCount;

	if (probesCounter == 0)
	{
		Vector3 v = XMVector3TransformNormal(Vector3(1,1,1), GetModelMatrix());
		probes.push_back(VertexPN(v, { 0,0,0 }));
		if (probes.size() > maxProbes)
			probes.erase(probes.begin(), probes.begin() + probes.size() - maxProbes);
	}

	probesCounter++;
}

Matrix Simulation::GetModelMatrix()
{
	return Matrix::CreateScale(cubeSize) * (Matrix)XMMatrixRotationQuaternion(Q) * initialRotation;
}
