#include "Graphics.h"

bool Graphics::Initialize(HWND hwnd, int width, int height)
{
	this->windowWidth = width;
	this->windowHeight = height;
	this->fpsTimer.Start();

	if (!InitializeDirectX(hwnd))
		return false;

	if (!InitializeShaders())
		return false;

	if (!InitializeScene())
		return false;

	InitGui(hwnd);

	return true;
}
void Graphics::RenderFrame()
{
	float bgcolor[] = { 0.05f, 0.05f, 0.1f, 1.0f };
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), bgcolor);
	this->deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	RenderVisualisation();
	RendeGui();

	this->swapchain->Present(0, NULL);
}
void Graphics::InitGui(HWND hwnd) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(this->device.Get(), this->deviceContext.Get());
	ImGui::StyleColorsDark();
}

void Graphics::RendeGui() {
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	RenderMainPanel();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
void Graphics::RenderMainPanel() {
	ImGui::SetNextWindowSize(ImVec2(400, 600), ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(10, 30), ImGuiCond_Once);
	if (!ImGui::Begin("Main Panel"))
	{
		ImGui::End();
		return;
	}

	if (simulation->paused) {
		if (ImGui::Button("Start"))
			simulation->paused = false;
	}
	else {
		if (ImGui::Button("Pause"))
			simulation->paused = true;
	}

	ImGui::SameLine();
	if (ImGui::Button("Reset")) {
		simulation->Reset();
	}

	ImGui::SliderFloat("delta time", &simulation->delta_time, 0.0005f, 0.05f, "%.4f");
	ImGui::SliderFloat("m", &simulation->m, 0.1f, 10.0f);
	ImGui::SliderFloat("c", &simulation->c, 0.01f, 100.0f);
	ImGui::SliderFloat("k", &simulation->kk, 0.01f, 100.0f);
	ImGui::SliderFloat("c frame", &simulation->cFrame, 0.01f, 100.0f);
	ImGui::SliderFloat("k frame", &simulation->kkFrame, 0.01f, 100.0f);

	ImGui::Separator();
	ImGui::SliderFloat3("frame position", &simulation->framePosition.x, -5, 5);
	ImGui::SliderFloat3("frame rotation", &simulation->frameRotation.x, -180, 180);

	ImGui::Separator();
	ImGui::Checkbox("elastic", &simulation->elastic);
	ImGui::Checkbox("reduce all", &simulation->reduceAll);
	ImGui::SliderFloat("mi", &simulation->mi, 0.00f, 1.0f);

	ImGui::Separator();
	ImGui::Checkbox("show jelly", &guiData->showJelly);
	ImGui::Checkbox("show frame", &guiData->showFrame);
	ImGui::Checkbox("show box", &guiData->showBox);

	ImGui::Separator();
	ImGui::SliderFloat("cube size", &simulation->cubeSize, 0.2, 2);
	ImGui::SliderFloat("random factor", &simulation->randomFactor, 0.2, 2);

	ImGui::Separator();
	ImGui::SliderFloat("simulation speed", &simulation->simulationSpeed, 0.1, 10);

	ImGui::End();
}
void Graphics::RenderVisualisation()
{
	this->deviceContext->IASetInputLayout(this->vertexshader.GetInputLayout());
	//this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	this->deviceContext->RSSetState(this->rasterizerState.Get());
	this->deviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 0);
	this->deviceContext->VSSetShader(vertexshader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(pureColorPixelshader.GetShader(), NULL, 0);

	this->deviceContext->VSSetConstantBuffers(0, 1, this->cbColoredObject.GetAddressOf());
	this->deviceContext->PSSetConstantBuffers(0, 1, this->cbColoredObject.GetAddressOf());

	if (guiData->showBox) RenderFrame(vbBox, ibBox, { 0.8f ,0.8f ,0.8f ,1 }, Matrix::Identity);
	if (guiData->showFrame)RenderFrame(vbFrame, ibFrame, { 0.8f ,0.8f ,0.8f ,1 }, simulation->GetFrameMatrix());
	UpdateJellyMesh();
	if (guiData->showJelly)RenderFrame(vbJelly, ibJelly, { 0.4f ,0.4f ,0.4f ,1 }, Matrix::Identity);

	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	RenderShading();
}
void Graphics::RenderFrame(VertexBuffer<VertexP>& vb, IndexBuffer& ib, Vector4 color, Matrix matrix)
{
	UINT offset = 0;

	cbColoredObject.data.worldMatrix = matrix;
	cbColoredObject.data.wvpMatrix = cbColoredObject.data.worldMatrix * camera.GetViewMatrix() * camera.GetProjectionMatrix();
	cbColoredObject.data.color = color;

	if (!cbColoredObject.ApplyChanges()) return;
	deviceContext->IASetVertexBuffers(0, 1, vb.GetAddressOf(), vb.StridePtr(), &offset);
	deviceContext->IASetIndexBuffer(ib.Get(), DXGI_FORMAT_R32_UINT, 0);
	deviceContext->DrawIndexed(ib.BufferSize(), 0, 0);
}

void Graphics::RenderShading()
{
	UINT offset = 0;
	cbColoredObject.data.worldMatrix = Matrix::Identity;
	cbColoredObject.data.wvpMatrix = cbColoredObject.data.worldMatrix * camera.GetViewMatrix() * camera.GetProjectionMatrix();
	cbColoredObject.data.color = { 0,0,1,1 };
	if (!cbColoredObject.ApplyChanges()) return;

	this->deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get(), 1, 1, uav_view.GetAddressOf(), &offset);

	{
		D3D11_MAPPED_SUBRESOURCE resource;
		this->deviceContext->Map(uav_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

		vector<VertexP> verts;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				verts.push_back(VertexP(simulation->p[j][0][i]));

		memcpy(resource.pData, verts.data(), verts.size() * sizeof(VertexP));
		this->deviceContext->Unmap(uav_buffer.Get(), 0);

		deviceContext->IASetVertexBuffers(0, 1, vbJellySides[0].GetAddressOf(), vbJellySides[0].StridePtr(), &offset);
		deviceContext->IASetIndexBuffer(ibJellySides.Get(), DXGI_FORMAT_R32_UINT, 0);
		deviceContext->DrawIndexed(ibJellySides.BufferSize(), 0, 0);
	}

}

bool Graphics::InitializeDirectX(HWND hwnd)
{
	std::vector<AdapterData> adapters = AdapterReader::GetAdapters();

	if (adapters.size() < 1)
	{
		ErrorLogger::Log("No IDXGI Adapters found.");
		return false;
	}

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferDesc.Width = this->windowWidth;
	scd.BufferDesc.Height = this->windowHeight;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;

	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = hwnd;
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hr;
	hr = D3D11CreateDeviceAndSwapChain(adapters[0].pAdapter, //IDXGI Adapter
		D3D_DRIVER_TYPE_UNKNOWN,
		NULL, //FOR SOFTWARE DRIVER TYPE
		NULL, //FLAGS FOR RUNTIME LAYERS
		NULL, //FEATURE LEVELS ARRAY
		0, //# OF FEATURE LEVELS IN ARRAY
		D3D11_SDK_VERSION,
		&scd, //Swapchain description
		this->swapchain.GetAddressOf(), //Swapchain Address
		this->device.GetAddressOf(), //Device Address
		NULL, //Supported feature level
		this->deviceContext.GetAddressOf()); //Device Context Address

	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create device and swapchain.");
		return false;
	}

	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	hr = this->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	if (FAILED(hr)) //If error occurred
	{
		ErrorLogger::Log(hr, "GetBuffer Failed.");
		return false;
	}

	hr = this->device->CreateRenderTargetView(backBuffer.Get(), NULL, this->renderTargetView.GetAddressOf());
	if (FAILED(hr)) //If error occurred
	{
		ErrorLogger::Log(hr, "Failed to create render target view.");
		return false;
	}

	//Describe our Depth/Stencil Buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = this->windowWidth;
	depthStencilDesc.Height = this->windowHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	hr = this->device->CreateTexture2D(&depthStencilDesc, NULL, this->depthStencilBuffer.GetAddressOf());
	if (FAILED(hr)) //If error occurred
	{
		ErrorLogger::Log(hr, "Failed to create depth stencil buffer.");
		return false;
	}

	hr = this->device->CreateDepthStencilView(this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf());
	if (FAILED(hr)) //If error occurred
	{
		ErrorLogger::Log(hr, "Failed to create depth stencil view.");
		return false;
	}

	this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());

	//Create depth stencil state
	D3D11_DEPTH_STENCIL_DESC depthstencildesc;
	ZeroMemory(&depthstencildesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	depthstencildesc.DepthEnable = true;
	depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

	hr = this->device->CreateDepthStencilState(&depthstencildesc, this->depthStencilState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create depth stencil state.");
		return false;
	}

	//Create the Viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = this->windowWidth;
	viewport.Height = this->windowHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	//Set the Viewport
	this->deviceContext->RSSetViewports(1, &viewport);

	//Create Rasterizer State
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create rasterizer state.");
		return false;
	}

	return true;
}
bool Graphics::InitializeShaders()
{
	wstring root = L"Resources\Shaders";

	if (!vertexshader.Initialize(this->device, root + L"my_vs.cso", VertexP::layout, ARRAYSIZE(VertexP::layout)))
		return false;

	if (!pixelshader.Initialize(this->device, root + L"my_ps.cso"))
		return false;

	if (!pureColorPixelshader.Initialize(this->device, root + L"pureColor_ps.cso"))
		return false;

	return true;
}

void Graphics::UpdateFrameMesh()
{
	D3D11_MAPPED_SUBRESOURCE resource;
	this->deviceContext->Map(vbFrame.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	vector<VertexP> verts;
	vector<int> inds;
	GetFrame(simulation->f[0][0][0], simulation->f[1][1][1], verts, inds);
	memcpy(resource.pData, verts.data(), verts.size() * sizeof(VertexP));
	this->deviceContext->Unmap(vbFrame.Get(), 0);
}
void Graphics::UpdateJellyMesh()
{
	D3D11_MAPPED_SUBRESOURCE resource;
	this->deviceContext->Map(vbJelly.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

	vector<VertexP> verts;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 4; k++)
				verts.push_back(VertexP(simulation->p[i][j][k]));

	memcpy(resource.pData, verts.data(), verts.size() * sizeof(VertexP));
	this->deviceContext->Unmap(vbJelly.Get(), 0);
}
void Graphics::UpdateJellySides()
{
	{
		D3D11_MAPPED_SUBRESOURCE resource;
		this->deviceContext->Map(vbJellySides[0].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

		vector<VertexPT> verts;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				verts.push_back(VertexPT(simulation->p[j][0][i], { j / 3.0f, i / 3.0f }));

		memcpy(resource.pData, verts.data(), verts.size() * sizeof(VertexP));
		this->deviceContext->Unmap(vbJellySides[0].Get(), 0);
	}
	{
		D3D11_MAPPED_SUBRESOURCE resource;
		this->deviceContext->Map(vbJellySides[1].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

		vector<VertexPT> verts;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				verts.push_back(VertexPT(simulation->p[3][j][i], { j / 3.0f, i / 3.0f }));

		memcpy(resource.pData, verts.data(), verts.size() * sizeof(VertexP));
		this->deviceContext->Unmap(vbJellySides[1].Get(), 0);
	}
	{
		D3D11_MAPPED_SUBRESOURCE resource;
		this->deviceContext->Map(vbJellySides[2].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

		vector<VertexPT> verts;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				verts.push_back(VertexPT(simulation->p[3 - j][3][i], { j / 3.0f, i / 3.0f }));

		memcpy(resource.pData, verts.data(), verts.size() * sizeof(VertexP));
		this->deviceContext->Unmap(vbJellySides[2].Get(), 0);
	}
	{
		D3D11_MAPPED_SUBRESOURCE resource;
		this->deviceContext->Map(vbJellySides[2].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

		vector<VertexPT> verts;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				verts.push_back(VertexPT(simulation->p[0][3 - j][i], { j / 3.0f, i / 3.0f }));

		memcpy(resource.pData, verts.data(), verts.size() * sizeof(VertexP));
		this->deviceContext->Unmap(vbJellySides[2].Get(), 0);
	}
	{
		D3D11_MAPPED_SUBRESOURCE resource;
		this->deviceContext->Map(vbJellySides[4].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

		vector<VertexPT> verts;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				verts.push_back(VertexPT(simulation->p[j][i][3], { j / 3.0f, i / 3.0f }));

		memcpy(resource.pData, verts.data(), verts.size() * sizeof(VertexP));
		this->deviceContext->Unmap(vbJellySides[4].Get(), 0);
	}
	{
		D3D11_MAPPED_SUBRESOURCE resource;
		this->deviceContext->Map(vbJellySides[4].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

		vector<VertexPT> verts;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				verts.push_back(VertexPT(simulation->p[j][3 - i][0], { j / 3.0f, i / 3.0f }));

		memcpy(resource.pData, verts.data(), verts.size() * sizeof(VertexP));
		this->deviceContext->Unmap(vbJellySides[4].Get(), 0);
	}
}

void Graphics::LoadBunny()
{
	ofstream myfile;
	myfile.open("bunny.obj");
	myfile << "Writing this to a file.\n";
	myfile.close();
}

void Graphics::GetFrame(Vector3 lb, Vector3 ub, vector<VertexP>& vertices, vector<int>& indices)
{
	vertices = {
		VertexP(lb.x,lb.y,lb.z),
		VertexP(ub.x,lb.y,lb.z),
		VertexP(lb.x,ub.y,lb.z),
		VertexP(ub.x,ub.y,lb.z),

		VertexP(lb.x,lb.y,ub.z),
		VertexP(ub.x,lb.y,ub.z),
		VertexP(lb.x,ub.y,ub.z),
		VertexP(ub.x,ub.y,ub.z),
	};

	indices =
	{
		0,1,2,3,0,2,1,3,
		4,5,6,7,4,6,5,7,
		0,4,1,5,2,6,3,7
	};
}
void Graphics::InitBox()
{
	vector<VertexP> vertices;
	vector<int> indices;
	GetFrame(simulation->lb, simulation->ub, vertices, indices);

	HRESULT hr = this->vbBox.Initialize(this->device.Get(), vertices.data(), vertices.size());
	if (FAILED(hr))
		ErrorLogger::Log(hr, "Failed to create vertex buffer.");

	hr = this->ibBox.Initialize(this->device.Get(), indices.data(), indices.size());
	if (FAILED(hr))
		ErrorLogger::Log(hr, "Failed to create indices buffer.");
}
void Graphics::InitFrame()
{
	vector<VertexP> vertices;
	vector<int> indices;
	GetFrame(simulation->f[0][0][0], simulation->f[1][1][1], vertices, indices);

	HRESULT hr = this->vbFrame.Initialize(this->device.Get(), vertices.data(), vertices.size(), true);
	if (FAILED(hr))
		ErrorLogger::Log(hr, "Failed to create vertex buffer.");

	hr = this->ibFrame.Initialize(this->device.Get(), indices.data(), indices.size());
	if (FAILED(hr))
		ErrorLogger::Log(hr, "Failed to create indices buffer.");
}
void Graphics::InitJelly()
{
	vector<VertexP> vertices;
	vector<int> indices;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 4; k++)
				vertices.push_back(VertexP(simulation->p[i][j][k]));

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 3; k++)
			{
				indices.push_back(i * 16 + j * 4 + k);
				indices.push_back(i * 16 + j * 4 + k + 1);
			}

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 3; j++)
			for (int k = 0; k < 4; k++)
			{
				indices.push_back(i * 16 + j * 4 + k);
				indices.push_back(i * 16 + (j + 1) * 4 + k);
			}

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 4; k++)
			{
				indices.push_back(i * 16 + j * 4 + k);
				indices.push_back((i + 1) * 16 + j * 4 + k);
			}

	HRESULT hr = this->vbJelly.Initialize(this->device.Get(), vertices.data(), vertices.size(), true);
	if (FAILED(hr))
		ErrorLogger::Log(hr, "Failed to create vertex buffer.");

	hr = this->ibJelly.Initialize(this->device.Get(), indices.data(), indices.size());
	if (FAILED(hr))
		ErrorLogger::Log(hr, "Failed to create indices buffer.");
}
void Graphics::InitJellySides()
{
	for (int i = 0; i < 6; i++)
	{
		HRESULT hr = this->vbJellySides[i].Initialize(this->device.Get(), nullptr, 16, true);
		if (FAILED(hr))
			ErrorLogger::Log(hr, "Failed to create vertex buffer.");
	}

	vector<int> indices
	{
		0,1,4,1,5,4,1,2,5,2,6,5,2,3,6,3,7,6,
		4,5,8,5,9,8,5,6,9,6,10,9,6,7,9,7,11,10,
		8,9,12,9,13,12,9,10,13,10,14,13,10,11,14,11,15,14
	};

	HRESULT hr = this->ibJellySides.Initialize(this->device.Get(), indices.data(), indices.size());
	if (FAILED(hr))
		ErrorLogger::Log(hr, "Failed to create vertex buffer.");
}
void Graphics::InitConstantBuffers()
{
	HRESULT hr = this->cbColoredObject.Initialize(this->device.Get(), this->deviceContext.Get());
	if (FAILED(hr))
		ErrorLogger::Log(hr, "Failed to initialize constant buffer.");

	hr = this->cbLight.Initialize(this->device.Get(), this->deviceContext.Get());
	if (FAILED(hr))
		ErrorLogger::Log(hr, "Failed to initialize constant buffer.");
}
void Graphics::InitUAV()
{
	D3D11_BUFFER_DESC buff_desc;
	memset(&buff_desc, 0, sizeof(buff_desc));

	buff_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;
	buff_desc.ByteWidth = 16 * sizeof(Vector3);
	buff_desc.MiscFlags = 0;
	buff_desc.StructureByteStride = sizeof(Vector3);
	//buff_desc.Usage = D3D11_USAGE_DYNAMIC;
	//buff_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = device->CreateBuffer(&buff_desc, nullptr, uav_buffer.GetAddressOf());

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = 16;

	hr = device->CreateUnorderedAccessView(uav_buffer.Get(), &uavDesc, uav_view.GetAddressOf());
}
void Graphics::InitDeformation()
{
	D3D11_TEXTURE3D_DESC tex_desc;
	memset(&tex_desc, 0, sizeof(tex_desc));
	tex_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tex_desc.Usage = D3D11_USAGE_DYNAMIC;
	tex_desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	tex_desc.MipLevels = 1;
	tex_desc.MiscFlags = 0;
	tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tex_desc.Width = 4;
	tex_desc.Height = 4;
	tex_desc.Depth = 4;
	device->CreateTexture3D(&tex_desc, nullptr, tex3D.GetAddressOf());

	D3D11_SHADER_RESOURCE_VIEW_DESC view_desc;
	memset(&view_desc, 0, sizeof(view_desc));
	view_desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
	view_desc.Texture3D.MipLevels = -1;
	view_desc.Texture3D.MostDetailedMip = 0;
	device->CreateShaderResourceView(tex3D.Get(), &view_desc, texSRV.GetAddressOf());

	D3D11_SAMPLER_DESC sampler_desc;
	memset(&sampler_desc, 0, sizeof(sampler_desc));
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.Filter = D3D11_FILTER_COMPARISON_ANISOTROPIC;
	sampler_desc.MipLODBias = 0;
	sampler_desc.MaxAnisotropy = 1;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampler_desc.BorderColor[0] = 1.0f;
	sampler_desc.BorderColor[1] = 1.0f;
	sampler_desc.BorderColor[2] = 1.0f;
	sampler_desc.BorderColor[3] = 1.0f;
	sampler_desc.MinLOD = -3.402823466e+38F; // -FLT_MAX
	sampler_desc.MaxLOD = 3.402823466e+38F; // FLT_MAX
	device->CreateSamplerState(&sampler_desc, sampler.GetAddressOf());
}
bool Graphics::InitializeScene()
{
	InitBox();
	InitFrame();
	InitJelly();

	InitConstantBuffers();
	InitDeformation();
	//InitUAV();

	camera.SetPosition(0, -5.0f, 0);
	camera.SetProjectionValues(90.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 1000.0f);

	return true;
}
