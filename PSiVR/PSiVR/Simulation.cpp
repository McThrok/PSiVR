#include "Simulation.h"

void Simulation::Init()
{

	simulationSpeed = 1;
	density = 1;
	cubeSize = 1;
	initialVelocity = Vector3::Zero;
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
	W = XMVector3TransformNormal(initialVelocity, initialRotation.Transpose());
	time = 0;
	probesCounter = 0;
}

void Simulation::UpdateTensor()
{
	I = XMFLOAT3X3(
		2.0f / 3.0f, -0.25, -0.25,
		-0.25, 2.0f / 3.0f, -0.25,
		-0.25, -0.25, 2.0f / 3.0f);

	float m = cubeSize * cubeSize * cubeSize * density;
	I *= m;

	InvI = I.Invert();

	initialRotation = XMMatrixRotationZ(-XM_PI / 4) * XMMatrixRotationY(-XMScalarACos(sqrtf(3) / 3));
	initialRotation *= XMMatrixRotationY(XM_PI * initialAngle / 180);
	G = XMVector3Normalize(XMVector3TransformNormal(Vector3(0, 0, -1), initialRotation.Transpose()));
	R = cubeSize * Vector3(0.5, 0.5, 0.5);
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
	Quaternion w0 = { W.x, W.y,W.z, 0.0f };
	Quaternion k1 = delta_time * 0.5f * XMQuaternionMultiply((Q), w0);
	Quaternion k2 = delta_time * 0.5f * XMQuaternionMultiply((Q + k1 * 0.5f), w0);
	Quaternion k3 = delta_time * 0.5f * XMQuaternionMultiply((Q + k2 * 0.5f), w0);
	Quaternion k4 = delta_time * 0.5f * XMQuaternionMultiply((Q + k3), w0);

	Quaternion newQ = XMQuaternionNormalize(Q + (Quaternion)((k1 + 2 * k2 + 2 * k3 + k4) / 6));


	Vector3 N = Vector3(0, 0, 0);
	if (gravityOn)
		N = XMVector3Cross(R, XMVector3Rotate(G, XMQuaternionInverse(Q)));

	Vector3 Iww = XMVector3Cross(XMVector3Transform(W, I), W);
	k1 = delta_time * XMVector3Transform(N + Iww, InvI);

	Iww = XMVector3Cross(XMVector3Transform(W + k1 * 0.5f, I), W + k1 * 0.5f);
	k2 = delta_time * XMVector3Transform(N + Iww, InvI);

	Iww = XMVector3Cross(XMVector3Transform(W + k2 * 0.5f, I), W + k2 * 0.5f);
	k3 = delta_time * XMVector3Transform(N + Iww, InvI);

	Iww = XMVector3Cross(XMVector3Transform(W + k3, I), W + k3);
	k4 = delta_time * XMVector3Transform(N + Iww, InvI);

	Vector3 newW = W + (Vector3)((k1 + 2 * k2 + 2 * k3 + k4) / 6);

	Q = newQ;
	W = newW;

	UpdateProbes();
}

void Simulation::UpdateProbes()
{
	while (probesCounter >= probesCycleCount)
		probesCounter -= probesCycleCount;

	if (probesCounter == 0)
	{
		Vector3 v = XMVector3TransformNormal(2 * R, GetWorldMatrix());
		probes.push_back(VertexPN(v, { 0,0,0 }));
		if (probes.size() > maxProbes)
			probes.erase(probes.begin(), probes.begin() + probes.size() - maxProbes);
	}

	probesCounter++;
}

Matrix Simulation::GetWorldMatrix()
{
	return Matrix::CreateScale(cubeSize) * (Matrix)XMMatrixRotationQuaternion(Q) * initialRotation;
}
