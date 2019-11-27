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
	QQwe = QQwe.identity();

	WQwe = vec3(1, 1, 1);
	WQwe.normalize();
	WQwe *= initialVelocity;

	time = 0;
	probesCounter = 0;
	probes.clear();
}

void Simulation::UpdateTensor()
{
	IQwe = mat3(
		2.0f / 3.0f, -0.25, -0.25,
		-0.25, 2.0f / 3.0f, -0.25,
		-0.25, -0.25, 2.0f / 3.0f);

	m = density;
	IQwe *= m;

	InvIQwe = IQwe.transposed();

	initialRotationQwe = initialRotationQwe.createRotationY(initialAngle * XM_PI / 180);
	initialRotationQwe *= initialRotationQwe.createRotationY(-0.955316618);
	initialRotationQwe *= initialRotationQwe.createRotationZ(-XM_PIDIV4);

	GQwe = initialRotationQwe.transposed().transform(vec3(0, 0, -m));
	RQwe = vec3(0.5f, 0.5f, 0.5f);
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
	quat newQQwe;
	{
		quat w0Qwe = quat(WQwe, 0);
		quat k1Qwe = delta_time * 0.5f * QQwe * w0Qwe;
		quat k2Qwe = delta_time * 0.5f * (QQwe + k1Qwe * 0.5f) * w0Qwe;
		quat k3Qwe = delta_time * 0.5f * (QQwe + k2Qwe * 0.5f) * w0Qwe;
		quat k4Qwe = delta_time * 0.5f * (QQwe + k3Qwe) * w0Qwe;

		newQQwe = QQwe + (k1Qwe + 2.0f * k2Qwe + 2.0f * k3Qwe + k4Qwe) / 6.0f;
		newQQwe.normalize();
	}

	vec3 newWQwe;
	{
		vec3 NQwe = vec3(0, 0, 0);
		if (gravityOn)
			NQwe = RQwe.cross(QQwe.inv().rotateVec(GQwe));

		vec3 IwwQwe = IQwe.transform(WQwe).cross(WQwe);
		vec3 k1Qwe = delta_time * InvIQwe.transform(NQwe + IwwQwe);

		IwwQwe = IQwe.transform(WQwe + k1Qwe * 0.5f).cross(WQwe + k1Qwe * 0.5f);
		vec3 k2Qwe = delta_time * InvIQwe.transform(NQwe + IwwQwe);

		IwwQwe = IQwe.transform(WQwe + k2Qwe * 0.5f).cross(WQwe + k2Qwe * 0.5f);
		vec3 k3Qwe = delta_time * InvIQwe.transform(NQwe + IwwQwe);

		IwwQwe = IQwe.transform(WQwe + k3Qwe).cross(WQwe + k3Qwe);
		vec3 k4Qwe = delta_time * InvIQwe.transform(NQwe + IwwQwe);

		newWQwe = WQwe + (k1Qwe + 2.0f * k2Qwe + 2.0f * k3Qwe + k4Qwe) / 6.0f;
	}

	QQwe = newQQwe;
	WQwe = newWQwe;
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
	mat3 mat = (initialRotationQwe * (QQwe.rotationMat()) * (initialRotationQwe.createScale(cubeSize))).transposed();
	return Matrix(XMFLOAT3X3(mat.m[0][0], mat.m[0][1], mat.m[0][2], mat.m[1][0], mat.m[1][1], mat.m[1][2], mat.m[2][0], mat.m[2][1], mat.m[2][2]));
}
