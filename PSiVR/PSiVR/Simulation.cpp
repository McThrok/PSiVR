#include "Simulation.h"

void Simulation::Init() {

	simulationSpeed = 1;
	density = 1;
	startVelocity = Vector3::Zero;
	paused = false;

	default_rotation = XMMatrixRotationZ(-XM_PI / 4) * XMMatrixRotationY(-XM_PI / 4);
	default_rotation = Matrix::Identity;
	//default_rotation = XMMatrixRotationZ(-XM_PI / 4);
	//default_rotation = XMMatrixRotationY(-XM_PI / 4);
	default_rotation = XMMatrixRotationZ(-XM_PI / 4) * XMMatrixRotationY(-XM_PI / 4);


	Matrix i(XMFLOAT3X3(
		2.0f / 3.0f, -0.25, -0.25,
		-0.25, 2.0f / 3.0f, -0.25,
		-0.25, -0.25, 2.0f / 3.0f));

	I = i;

	//I = default_rotation.Transpose() * i * default_rotation;
	//I = default_rotation * i * default_rotation.Transpose();

	G = { 0,0,-1 };

	Reset();


	invI = I.Invert();
	g = XMVector3Normalize(XMVector3TransformNormal(G, default_rotation.Transpose()));
	//r = XMVector3TransformCoord({ 0.5,0.5,0.5 }, default_rotation);
	r = { 0.5,0.5,0.5 };
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

void Simulation::Update() {
	Quaternion w0 = { W.x, W.y,W.z, 0.0f };
	Quaternion k1 = delta_time * 0.5f * XMQuaternionMultiply((Q), w0);
	Quaternion k2 = delta_time * 0.5f * XMQuaternionMultiply((Q + k1 * 0.5f), w0);
	Quaternion k3 = delta_time * 0.5f * XMQuaternionMultiply((Q + k2 * 0.5f), w0);
	Quaternion k4 = delta_time * 0.5f * XMQuaternionMultiply((Q + k3), w0);

	Quaternion newQ = XMQuaternionNormalize(Q + (Quaternion)((k1 + 2 * k2 + 2 * k3 + k4) / 6));

	//Vector3 r = { 0.5,0.5,0.5 };

	Matrix rot = XMMatrixRotationQuaternion(Q);
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
}

Matrix Simulation::GetWorldMatrix()
{
	return (Matrix)XMMatrixRotationQuaternion(Q) * default_rotation;
}
