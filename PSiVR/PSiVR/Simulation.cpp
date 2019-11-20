#include "Simulation.h"

void Simulation::Init() {

	simulationSpeed = 1;
	density = 1;
	startVelocity = Vector3::Zero;

	default_rotation = XMMatrixRotationZ(-XM_PI / 4) * XMMatrixRotationY(-XM_PI / 4);
	default_rotation = Matrix::Identity;
	//default_rotation = XMMatrixRotationZ(-XM_PI / 4);
	//default_rotation = XMMatrixRotationY(-XM_PI / 4);
	//default_rotation = XMMatrixRotationZ(-3*XM_PI / 4) * XMMatrixRotationY(-3 * XM_PI / 4);

	Matrix i(XMFLOAT3X3(
		2.0f / 3.0f, -0.25, -0.25,
		-0.25, 2.0f / 3.0f, -0.25,
		-0.25, -0.25, 2.0f / 3.0f));

	I = default_rotation.Transpose() * i * default_rotation;

	G = { 0,0,-1 };

	Reset();
}

void Simulation::Reset() {
	Q = Quaternion::Identity;
	W = { 0,0,0 };

	time = 0;
	delta_time = 0.001;
	paused = false;
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
	Quaternion w0 = { W.x,W.y,W.z,0.0f };
	Quaternion k1 = delta_time * 0.5f * XMQuaternionMultiply((Q), w0);
	Quaternion k2 = delta_time * 0.5f * XMQuaternionMultiply((Q + (Quaternion)(k1 / 2)), w0);
	Quaternion k3 = delta_time * 0.5f * XMQuaternionMultiply((Q + (Quaternion)(k2 / 2)), w0);
	Quaternion k4 = delta_time * 0.5f * XMQuaternionMultiply((Q + k3), w0);

	Quaternion newQ = XMQuaternionNormalize(Q + (Quaternion)((k1 + k2 + k3 + k4) / 6));

	XMMATRIX invI = I.Invert();
	Vector3 g = XMVector3Normalize(XMVector3TransformNormal(G, default_rotation.Transpose()));
	Vector3 r = XMVector3TransformCoord({ 0.5,0.5,0.5 }, default_rotation);
	//Vector3 r = { 0.5,0.5,0.5 };

	Matrix rot = XMMatrixRotationQuaternion(Q);
	Vector3 gRot = XMVector3Rotate(g, XMQuaternionInverse(Q));
	Vector3 N = XMVector3Cross(r, gRot);

	Vector3 Iww = XMVector3Cross(XMVector3Transform(W, I), W);
	k1 = delta_time * XMVector3Transform(N + Iww, invI);

	Iww = XMVector3Cross(XMVector3Transform(W + (Vector3)(k1 / 2), I), W + (Vector3)(k1 / 2));
	k2 = delta_time * XMVector3Transform(N + Iww, invI);

	Iww = XMVector3Cross(XMVector3Transform(W + (Vector3)(k2 / 2), I), W + (Vector3)(k2 / 2));
	k3 = delta_time * XMVector3Transform(N + Iww, invI);

	Iww = XMVector3Cross(XMVector3Transform(W + k3, I), W + k3);
	k4 = delta_time * XMVector3Transform(N + Iww, invI);

	Vector3 newW = W + (Vector3)((k1 + k2 + k3 + k4) / 6);

	Q = newQ;
	W = newW;
}

Matrix Simulation::GetWorldMatrix()
{
	return default_rotation * XMMatrixRotationQuaternion(Q);
}
