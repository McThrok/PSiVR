#include "Graphics.h"

Graphics::~Graphics()
{
}

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

	ImGui::Checkbox("show cube", &guiData->showCube);

	ImGui::Separator();
	bool update = false;
	if (ImGui::SliderFloat("cube size", &simulation->cubeSize, 0.2, 2)) update = true;
	if (update) simulation->Reset();
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

	RenderFrame(vbBox, ibBox, { 0.8f ,0.8f ,0.8f ,1 });
	RenderFrame(vbFrame, ibFrame, { 0.8f ,0.8f ,0.8f ,1 });
	UpdateJellyMesh();
	RenderFrame(vbJelly, ibJelly, { 0.4f ,0.4f ,0.4f ,1 });
}
void Graphics::RenderFrame(VertexBuffer<VertexPN>& vb, IndexBuffer& ib, Vector4 color)
{
	UINT offset = 0;

	cbColoredObject.data.worldMatrix = Matrix::Identity;
	cbColoredObject.data.wvpMatrix = cbColoredObject.data.worldMatrix * camera.GetViewMatrix() * camera.GetProjectionMatrix();
	cbColoredObject.data.color = color;

	if (!cbColoredObject.ApplyChanges()) return;
	deviceContext->IASetVertexBuffers(0, 1, vb.GetAddressOf(), vb.StridePtr(), &offset);
	deviceContext->IASetIndexBuffer(ib.Get(), DXGI_FORMAT_R32_UINT, 0);
	deviceContext->DrawIndexed(ib.BufferSize(), 0, 0);
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
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

	if (!vertexshader.Initialize(this->device, L"my_vs.cso", layout, numElements))
		return false;

	if (!pixelshader.Initialize(this->device, L"my_ps.cso"))
		return false;

	if (!pureColorPixelshader.Initialize(this->device, L"pureColor_ps.cso"))
		return false;

	return true;
}

void Graphics::UpdateFrameMesh()
{
	D3D11_MAPPED_SUBRESOURCE resource;
	this->deviceContext->Map(vbFrame.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	vector<VertexPN> verts;
	vector<int> inds;
	GetFrame(simulation->f[0][0][0], simulation->f[1][1][1], verts, inds);
	memcpy(resource.pData, verts.data(), verts.size() * sizeof(VertexPN));
	this->deviceContext->Unmap(vbFrame.Get(), 0);
}
void Graphics::UpdateJellyMesh()
{
	D3D11_MAPPED_SUBRESOURCE resource;
	this->deviceContext->Map(vbJelly.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

	vector<VertexPN> verts;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 4; k++)
				verts.push_back(VertexPN(simulation->p[i][j][k], { 0,0,0 }));

	memcpy(resource.pData, verts.data(), verts.size() * sizeof(VertexPN));
	this->deviceContext->Unmap(vbJelly.Get(), 0);
}

void Graphics::GetFrame(Vector3 lb, Vector3 ub, vector<VertexPN>& vertices, vector<int>& indices)
{
	vertices = {
		VertexPN(lb.x,lb.y,lb.z, 0.0f, 0.0f, 0.0f),
		VertexPN(ub.x,lb.y,lb.z, 0.0f, 0.0f, 0.0f),
		VertexPN(lb.x,ub.y,lb.z, 0.0f, 0.0f, 0.0f),
		VertexPN(ub.x,ub.y,lb.z, 0.0f, 0.0f, 0.0f),

		VertexPN(lb.x,lb.y,ub.z, 0.0f, 0.0f, 0.0f),
		VertexPN(ub.x,lb.y,ub.z, 0.0f, 0.0f, 0.0f),
		VertexPN(lb.x,ub.y,ub.z, 0.0f, 0.0f, 0.0f),
		VertexPN(ub.x,ub.y,ub.z, 0.0f, 0.0f, 0.0f),
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
	vector<VertexPN> vertices;
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
	vector<VertexPN> vertices;
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
	vector<VertexPN> vertices;
	vector<int> indices;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 4; k++)
				vertices.push_back(VertexPN(simulation->p[i][j][k], { 0,0,0 }));

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
void Graphics::InitConstantBuffers()
{
	HRESULT hr = this->cbColoredObject.Initialize(this->device.Get(), this->deviceContext.Get());
	if (FAILED(hr))
		ErrorLogger::Log(hr, "Failed to initialize constant buffer.");

	hr = this->cbLight.Initialize(this->device.Get(), this->deviceContext.Get());
	if (FAILED(hr))
		ErrorLogger::Log(hr, "Failed to initialize constant buffer.");
}
bool Graphics::InitializeScene()
{
	InitBox();
	InitFrame();
	InitJelly();

	InitConstantBuffers();

	camera.SetPosition(0, -5.0f, 0);
	camera.SetProjectionValues(90.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 1000.0f);

	return true;
}
