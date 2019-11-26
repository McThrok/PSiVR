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
	QQwe = QQwe.identity();

	W = Vector3(1, 1, 1);
	W.Normalize();
	W *= initialVelocity;

	WQwe = vec3(1, 1, 1);
	WQwe.normalize();
	WQwe *= initialVelocity;



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

	IQwe = mat3(
		2.0f / 3.0f, -0.25, -0.25,
		-0.25, 2.0f / 3.0f, -0.25,
		-0.25, -0.25, 2.0f / 3.0f);

	m = density;
	I *= m;
	IQwe *= m;

	InvI = I.Transpose();
	InvIQwe = IQwe.transposed();

	initialRotation = XMMatrixRotationZ(-XM_PIDIV4);
	initialRotation *= XMMatrixRotationY(-0.955316618);//XMMatrixRotationY(-XMScalarACos(sqrtf(3.0f) / 3.0f));
	initialRotation *= XMMatrixRotationY(XMConvertToRadians(initialAngle));

	initialRotationQwe = initialRotationQwe.createRotationY(initialAngle * XM_PI / 180);
	initialRotationQwe *= initialRotationQwe.createRotationY(-0.955316618);
	initialRotationQwe *= initialRotationQwe.createRotationZ(-XM_PIDIV4);

	G = XMVector3TransformNormal(Vector3(0, 0, -m), initialRotation.Transpose());
	GQwe = initialRotationQwe.transposed().transform(vec3(0, 0, -m));
	R = Vector3(0.5f, 0.5f, 0.5f);
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
	Quaternion newQ;
	quat newQQwe;
	{
		Quaternion w0 = Quaternion(W, 0);
		Quaternion k1 = delta_time * 0.5f * XMQuaternionMultiply(w0, Q);
		Quaternion k2 = delta_time * 0.5f * XMQuaternionMultiply(w0, (Q + k1 * 0.5f));
		Quaternion k3 = delta_time * 0.5f * XMQuaternionMultiply(w0, (Q + k2 * 0.5f));
		Quaternion k4 = delta_time * 0.5f * XMQuaternionMultiply(w0, (Q + k3));

		newQ = XMQuaternionNormalize(Q + (Quaternion)((k1 + 2.0f * k2 + 2.0f * k3 + k4) / 6.0f));
		Quaternion qwenewQ = Q + (Quaternion)((k1 + 2.0f * k2 + 2.0f * k3 + k4) / 6.0f);

		quat w0Qwe = quat(WQwe, 0);
		quat k1Qwe = delta_time * 0.5f * QQwe * w0Qwe;
		quat k2Qwe = delta_time * 0.5f * (QQwe + k1Qwe * 0.5f) * w0Qwe;
		quat k3Qwe = delta_time * 0.5f * (QQwe + k2Qwe * 0.5f) * w0Qwe;
		quat k4Qwe = delta_time * 0.5f * (QQwe + k3Qwe) * w0Qwe;

		newQQwe = QQwe + (k1Qwe + 2.0f * k2Qwe + 2.0f * k3Qwe + k4Qwe) / 6.0f;
		newQQwe.normalize();
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


	vec3 newWQwe;
	//{
	//	Vector3 N = Vector3(0, 0, 0);
	//	//vec3 NQwe = RQwe.cross(QQwe.inv().rotateVec(GQwe));
	//	if (gravityOn)
	//	{
	//		quat qwe = quat(Q.x, Q.y, Q.z, Q.w);
	//		qwe = qwe.inv();
	//		qwe.normalize();
	//		Quaternion q(qwe.x, qwe.y, qwe.z, qwe.w);
	//		//q = Q;
	//		//q.Inverse(q);
	//		N = XMVector3Cross(R, XMVector3Rotate(G, q));
	//		//N = Vector3(NQwe.x, NQwe.y, NQwe.z);

	//	}


	//	Vector3 Iww = XMVector3Cross(XMVector3TransformNormal(W, I), W);
	//	Vector3 k1 = delta_time * XMVector3TransformNormal(N + Iww, InvI);

	//	Iww = XMVector3Cross(XMVector3TransformNormal(W + k1 * 0.5f, I), W + k1 * 0.5f);
	//	Vector3 k2 = delta_time * XMVector3TransformNormal(N + Iww, InvI);

	//	Iww = XMVector3Cross(XMVector3TransformNormal(W + k2 * 0.5f, I), W + k2 * 0.5f);
	//	Vector3 k3 = delta_time * XMVector3TransformNormal(N + Iww, InvI);

	//	Iww = XMVector3Cross(XMVector3TransformNormal(W + k3, I), W + k3);
	//	Vector3 k4 = delta_time * XMVector3TransformNormal(N + Iww, InvI);

	//	Vector3 tmp = (Vector3)((k1 + 2.0f * k2 + 2.0f * k3 + k4) / 6.0f);

	//	newWQwe = WQwe + vec3(tmp.x, tmp.y, tmp.z);
	//}
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
		int fd = 10;

	}

	Q = newQ;
	W = newW;

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

	Matrix mm = Matrix::CreateScale(cubeSize) * (Matrix)XMMatrixRotationQuaternion(Q) * initialRotation;
	mat3 mat = initialRotationQwe * (QQwe.rotationMat()) * (initialRotationQwe.createScale(cubeSize));
	mat.transpose();

	Matrix mmf = Matrix(XMFLOAT3X3(mat.m[0][0], mat.m[0][1], mat.m[0][2], mat.m[1][0], mat.m[1][1], mat.m[1][2], mat.m[2][0], mat.m[2][1], mat.m[2][2]));

	static int i = 0;
	if (++i % 2 != 0)
		//if (false)
	{
		return Matrix::CreateScale(cubeSize) * (Matrix)XMMatrixRotationQuaternion(Q) * initialRotation;
	}
	else
	{
		mat3 mat = initialRotationQwe * (QQwe.rotationMat()) * (initialRotationQwe.createScale(cubeSize));
		mat.transpose();
		return Matrix(XMFLOAT3X3(mat.m[0][0], mat.m[0][1], mat.m[0][2], mat.m[1][0], mat.m[1][1], mat.m[1][2], mat.m[2][0], mat.m[2][1], mat.m[2][2]));
	}
}
