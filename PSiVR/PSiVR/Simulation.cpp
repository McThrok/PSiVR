#include "Simulation.h"

void Simulation::Init() {

	simulationSpeed = 1;
	density = 1;
	cubeSize = 1;
	startVelocity = Vector3::Zero;
	paused = false;
	probesCount = 100;
	G = { 0,0,-1 };

	initial_rotation = XMMatrixRotationZ(-XM_PI / 4) * XMMatrixRotationY(-XMScalarACos(sqrtf(3) / 3));

	Reset();
	UpdateTensor();
}

void Simulation::Reset() {
	Q = Quaternion::Identity;
	W = { 0,0,0 };

	time = 0;
	delta_time = 0.001;
	gravityUp = false;
}

void Simulation::Update(float dt) {
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

void Simulation::UpdateTensor()
{
	I = XMFLOAT3X3(
		2.0f / 3.0f, -0.25, -0.25,
		-0.25, 2.0f / 3.0f, -0.25,
		-0.25, -0.25, 2.0f / 3.0f);

	float m = cubeSize * cubeSize * cubeSize * density;
	I *= m;

	invI = I.Invert();
	g = XMVector3Normalize(XMVector3TransformNormal(G, initial_rotation.Transpose()));
	r = cubeSize * Vector3(0.5, 0.5, 0.5);
}

void Simulation::Update() {
	Quaternion w0 = { W.x, W.y,W.z, 0.0f };
	Quaternion k1 = delta_time * 0.5f * XMQuaternionMultiply((Q), w0);
	Quaternion k2 = delta_time * 0.5f * XMQuaternionMultiply((Q + k1 * 0.5f), w0);
	Quaternion k3 = delta_time * 0.5f * XMQuaternionMultiply((Q + k2 * 0.5f), w0);
	Quaternion k4 = delta_time * 0.5f * XMQuaternionMultiply((Q + k3), w0);

	Quaternion newQ = XMQuaternionNormalize(Q + (Quaternion)((k1 + 2 * k2 + 2 * k3 + k4) / 6));


	Vector3 N = XMVector3Cross(r, XMVector3Rotate(g, XMQuaternionInverse(Q)));

	Vector3 Iww = XMVector3Cross(XMVector3Transform(W, I), W);
	k1 = delta_time * XMVector3Transform(N + Iww, invI);

	Iww = XMVector3Cross(XMVector3Transform(W + k1 * 0.5f, I), W + k1 * 0.5f);
	k2 = delta_time * XMVector3Transform(N + Iww, invI);

	Iww = XMVector3Cross(XMVector3Transform(W + k2 * 0.5f, I), W + k2 * 0.5f);
	k3 = delta_time * XMVector3Transform(N + Iww, invI);

	Iww = XMVector3Cross(XMVector3Transform(W + k3, I), W + k3);
	k4 = delta_time * XMVector3Transform(N + Iww, invI);

	Vector3 newW = W + (Vector3)((k1 + 2 * k2 + 2 * k3 + k4) / 6);

	Q = newQ;
	W = newW;

	probes.push_back(XMVector3TransformNormal(r, GetWorldMatrix()));
	if (probes.size() > probesCount)
		probes.erase(probes.begin(), probes.begin() + probes.size() - probesCount);
}

Matrix Simulation::GetWorldMatrix()
{
	return Matrix::CreateScale(cubeSize) * (Matrix)XMMatrixRotationQuaternion(Q) * initial_rotation;
}
