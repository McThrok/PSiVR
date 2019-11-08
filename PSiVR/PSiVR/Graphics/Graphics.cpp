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
	updateFPSCounter();

	this->swapchain->Present(0, NULL);
}

void Graphics::updateFPSCounter() {
	static int fpsCounter = 0;
	static std::string fpsString = "FPS: 0";
	fpsCounter += 1;
	if (fpsTimer.GetMilisecondsElapsed() > 1000.0)
	{
		fpsString = "FPS: " + std::to_string(fpsCounter);
		fpsCounter = 0;
		fpsTimer.Restart();
	}
	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch.get(), StringConverter::StringToWide(fpsString).c_str(), DirectX::XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	spriteBatch->End();
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
	ImGui::SetNextWindowSize(ImVec2(300, 950), ImGuiCond_Once);
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

	/*ImGui::SameLine();
	if (ImGui::Button("Reset")) {
		simulation->Reset();
	}*/

	ImGui::SliderFloat("delta time", &simulation->delta_time, 0.01f, 0.2f);

	ImGui::End();
}

void Graphics::RenderVisualisation()
{
	this->deviceContext->IASetInputLayout(this->vertexshader.GetInputLayout());
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->deviceContext->RSSetState(this->rasterizerState.Get());
	this->deviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 0);
	this->deviceContext->VSSetShader(vertexshader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(pixelshader.GetShader(), NULL, 0);

	UINT offset = 0;

	cbVS.data.worldMatrix = simulation->GetWorldMatrix();
	cbVS.data.wvpMatrix = cbVS.data.worldMatrix * camera.GetViewMatrix() * camera.GetProjectionMatrix();

	if (!cbVS.ApplyChanges()) return;
	this->deviceContext->VSSetConstantBuffers(0, 1, this->cbVS.GetAddressOf());
	this->deviceContext->IASetVertexBuffers(0, 1, vbCube.GetAddressOf(), vbCube.StridePtr(), &offset);
	this->deviceContext->IASetIndexBuffer(ibCube.Get(), DXGI_FORMAT_R32_UINT, 0);
	this->deviceContext->DrawIndexed(ibCube.BufferSize(), 0, 0);

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

	spriteBatch = std::make_unique<DirectX::SpriteBatch>(this->deviceContext.Get());
	spriteFont = std::make_unique<DirectX::SpriteFont>(this->device.Get(), L"Data\\Fonts\\comic_sans_ms_16.spritefont");


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

	return true;
}

bool Graphics::InitializeScene()
{
	VertexPN v[] = {
		VertexPN(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f),
		VertexPN(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f),
		VertexPN(1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f),
		VertexPN(0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f),

		VertexPN(1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f),
		VertexPN(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f),
		VertexPN(0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f),
		VertexPN(1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f),

		VertexPN(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f),
		VertexPN(1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f),
		VertexPN(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f),
		VertexPN(1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f),

		VertexPN(0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f),
		VertexPN(0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f),
		VertexPN(0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f),
		VertexPN(0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f),

		VertexPN(0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f),
		VertexPN(1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f),
		VertexPN(1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f),
		VertexPN(0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f),

		VertexPN(0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f),
		VertexPN(1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f),
		VertexPN(1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f),
		VertexPN(0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f)
	};

	int indices[] =
	{
		0, 2, 3, 0,1, 2,
		4, 6, 7, 4,5, 6,
		8, 10, 11, 8, 9, 10,
		12, 14, 15, 12, 13, 14,
		16, 18, 19, 16, 17, 18,
		20, 22, 23, 20, 21, 22,

		//0,3,2,0,2,1,
		//4,7,6,4,6,5,
		//8,11,10,8,10,9,
		//12,15,14,12,14,13,
		//16,19,18,16,18,17,
		//20,23,22,20,22,21
	};

	/*XMFLOAT3 p[] = {
		{ 0.000000 ,0.000000, 0.000000 },
		{ 0.000000 ,1.154701, 1.632993 },
		{ -1.414214 ,1.154701, -0.816497 },
		{ -1.414214 ,2.309401, 0.816497 },
		{ 1.414214 ,1.154701, -0.816497 },
		{ 1.414214 ,2.309401, 0.816497 },
		{ 0.000000 ,2.309401, -1.632993 },
		{ 0.000000 ,3.464102, 0.000000 },
	};

	XMFLOAT3 n[] = {
		{ -0.7071, -0.5774, 0.4082 },
		{ -0.7071, 0.5774 , -0.4082},
		{ 0.7071 , 0.5774 , -0.4082},
		{ 0.7071 , -0.5774, 0.4082 },
		{ -0.0000, -0.5774, -0.8165},
		{ 0.0000 , 0.5774 ,0.8165 },
	};

	VertexPN v[] = {
		VertexPN(p[1],n[0]),VertexPN(p[2],n[0]),VertexPN(p[0],n[0]),
		VertexPN(p[3],n[1]),VertexPN(p[6],n[1]),VertexPN(p[2],n[1]),

		VertexPN(p[7],n[2]),VertexPN(p[4],n[2]),VertexPN(p[6],n[2]),
		VertexPN(p[4],n[3]),VertexPN(p[1],n[3]),VertexPN(p[0],n[3]),

		VertexPN(p[6],n[4]),VertexPN(p[0],n[4]),VertexPN(p[2],n[4]),
		VertexPN(p[3],n[5]),VertexPN(p[5],n[5]),VertexPN(p[7],n[5]),

		VertexPN(p[1],n[0]),VertexPN(p[3],n[0]),VertexPN(p[2],n[0]),
		VertexPN(p[3],n[1]),VertexPN(p[7],n[1]),VertexPN(p[6],n[1]),

		VertexPN(p[7],n[2]),VertexPN(p[5],n[2]),VertexPN(p[4],n[2]),
		VertexPN(p[4],n[3]),VertexPN(p[5],n[3]),VertexPN(p[1],n[3]),

		VertexPN(p[6],n[4]),VertexPN(p[4],n[4]),VertexPN(p[0],n[4]),
		VertexPN(p[3],n[5]),VertexPN(p[1],n[5]),VertexPN(p[5],n[5]),
	};

	int indices[36];
	for (int i = 0; i < 36; i++)
		indices[i] = i;*/

		//Load Vertex Data
	HRESULT hr = this->vbCube.Initialize(this->device.Get(), v, ARRAYSIZE(v));
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create vertex buffer.");
		return false;
	}

	//Load Index Data
	hr = this->ibCube.Initialize(this->device.Get(), indices, ARRAYSIZE(indices));
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create indices buffer.");
		return hr;
	}

	//Initialize Constant Buffer(s)
	hr = this->cbVS.Initialize(this->device.Get(), this->deviceContext.Get());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to initialize constant buffer.");
		return false;
	}

	camera.SetPosition(0, -5.0f, 0);
	camera.SetProjectionValues(90.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 1000.0f);

	return true;
}
