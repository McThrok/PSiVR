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
	Q = Q.identity();

	W = initialRotation.transform(vec3(1, 1, 1));
	W.normalize();
	W *= initialVelocity;

	time = 0;
	probesCounter = 0;
	probes.clear();
}

void Simulation::UpdateTensor()
{
	I = mat3(
		2.0L / 3.0L, -0.25L, -0.25L,
		-0.25L, 2.0L / 3.0L, -0.25L,
		-0.25L, -0.25L, 2.0L / 3.0L);

	m = density;
	I *= m;
	InvI = mat3(2.72727, 1.63636, 1.63636, 1.63636, 2.72727, 1.63636, 1.63636, 1.63636, 2.72727);
	InvI /= m;

	long double pi = 3.14159265358979323846264338327950288419716939937510L;

	initialRotation = initialRotation.createRotationY(initialAngle * pi / 180.0L);
	initialRotation *= initialRotation.createRotationY(-0.95531661812450927816385710251575775424341469501001L);//XMMatrixRotationY(-XMScalarACos(sqrtf(3.0L) / 3.0L));
	initialRotation *= initialRotation.createRotationZ(-pi / 4);

	I = initialRotation * I * initialRotation.transposed();
	InvI = initialRotation * InvI * initialRotation.transposed();

	G = vec3(0, 0, -m*9.81L);
	R = initialRotation.transform(vec3(0.5L, 0.5L, 0.5L));
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

vec3 Simulation::GetN(quat q)
{
	vec3 N = vec3(0, 0, 0);
	if (gravityOn)
		N = InvI.transform(R.cross(Q.inv().rotateVec(G)));

	if (gravityUp)
		N = -N;

	return N;
}

void Simulation::Update()
{
	quat newQ;
	vec3 newW;

	{
		quat w0 = quat(W, 0);
		quat qk1 = delta_time * 0.5L * Q * w0;
		quat qk2 = delta_time * 0.5L * (Q + qk1 * 0.5L) * w0;
		quat qk3 = delta_time * 0.5L * (Q + qk2 * 0.5L) * w0;
		quat qk4 = delta_time * 0.5L * (Q + qk3) * w0;

		newQ = Q + (qk1 + 2.0L * qk2 + 2.0L * qk3 + qk4) / 6.0L;
		newQ.normalize();

		vec3 N = GetN(Q);
		vec3 w = W;
		vec3 k1 = delta_time * (N + w.cross(InvI.transform(w)));

		N = GetN(Q + qk1 * 0.5L);
		w = W + k1 * 0.5L;
		vec3 k2 = delta_time * (N + w.cross(InvI.transform(w)));

		N = GetN(Q + qk2 * 0.5L);
		w = W + k2 * 0.5L;
		vec3 k3 = delta_time * (N + w.cross(InvI.transform(w)));

		N = GetN(Q + qk3);
		w = W + k3;
		vec3 k4 = delta_time * (N + w.cross(InvI.transform(w)));

		newW = W + (k1 + 2.0L * k2 + 2.0L * k3 + k4) / 6.0L;
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
		Vector3 v = XMVector3TransformNormal(Vector3(1, 1, 1), GetModelMatrix());
		probes.push_back(VertexPN(v, { 0,0,0 }));
		if (probes.size() > maxProbes)
			probes.erase(probes.begin(), probes.begin() + probes.size() - maxProbes);
	}

	probesCounter++;
}

Matrix Simulation::GetModelMatrix()
{
	mat3 mat = (Q.rotationMat() * initialRotation).transposed();
	return Matrix(XMFLOAT3X3(mat.m[0][0], mat.m[0][1], mat.m[0][2], mat.m[1][0], mat.m[1][1], mat.m[1][2], mat.m[2][0], mat.m[2][1], mat.m[2][2]));
}
