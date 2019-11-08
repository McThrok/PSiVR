#include "Engine.h"

bool Engine::Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
{
	timer.Start();

	if (!this->render_window.Initialize(this, hInstance, window_title, window_class, width, height))
		return false;

	if (!gfx.Initialize(this->render_window.GetHWND(), width, height))
		return false;

	gfx.simulation = &simulation;
	simulation.Init();

	return true;
}

bool Engine::ProcessMessages()
{
	return this->render_window.ProcessMessages();
}

void Engine::Update()
{
	float dt = timer.GetMilisecondsElapsed();
	timer.Restart();

	while (!keyboard.CharBufferIsEmpty())
	{
		unsigned char ch = keyboard.ReadChar();
	}

	while (!keyboard.KeyBufferIsEmpty())
	{
		KeyboardEvent kbe = keyboard.ReadKey();
		unsigned char keycode = kbe.GetKeyCode();
	}

	while (!mouse.EventBufferIsEmpty())
	{
		MouseEvent me = mouse.ReadEvent();
		if (mouse.IsRightDown())
		{
			if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
			{
				this->gfx.camera.AdjustRotation((float)me.GetPosY() * 0.01f, (float)me.GetPosX() * 0.01f, 0);
			}
		}
	}

	const float cameraSpeed = 0.006f;

	if (keyboard.KeyIsPressed('W'))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetForwardVector() * cameraSpeed * dt);
	}
	if (keyboard.KeyIsPressed('S'))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetBackwardVector() * cameraSpeed * dt);
	}
	if (keyboard.KeyIsPressed('A'))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetLeftVector() * cameraSpeed * dt);
	}
	if (keyboard.KeyIsPressed('D'))
	{
		this->gfx.camera.AdjustPosition(this->gfx.camera.GetRightVector() * cameraSpeed * dt);
	}
	if (keyboard.KeyIsPressed('Q'))
	{
		this->gfx.camera.AdjustPosition(0.0f, cameraSpeed * dt, 0.0f);
	}
	if (keyboard.KeyIsPressed('E'))
	{
		this->gfx.camera.AdjustPosition(0.0f, -cameraSpeed * dt, 0.0f);
	}

	simulation.Update(dt);
}

void Engine::RenderFrame()
{
	this->gfx.RenderFrame();
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
	/*VertexPN v[] = {
		VertexPN(-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f),
		VertexPN(+0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f),
		VertexPN(+0.5f, +0.5f, -0.5f, 0.0f, 0.0f, -1.0f),
		VertexPN(-0.5f, +0.5f, -0.5f, 0.0f, 0.0f, -1.0f),

		VertexPN(+0.5f, -0.5f, +0.5f, 0.0f, 0.0f, 1.0f),
		VertexPN(-0.5f, -0.5f, +0.5f, 0.0f, 0.0f, 1.0f),
		VertexPN(-0.5f, +0.5f, +0.5f, 0.0f, 0.0f, 1.0f),
		VertexPN(+0.5f, +0.5f, +0.5f, 0.0f, 0.0f, 1.0f),

		VertexPN(+0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f),
		VertexPN(+0.5f, -0.5f, +0.5f, 1.0f, 0.0f, 0.0f),
		VertexPN(+0.5f, +0.5f, +0.5f, 1.0f, 0.0f, 0.0f),
		VertexPN(+0.5f, +0.5f, -0.5f, 1.0f, 0.0f, 0.0f),

		VertexPN(-0.5f, -0.5f, +0.5f, -1.0f, 0.0f, 0.0f),
		VertexPN(-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f),
		VertexPN(-0.5f, +0.5f, -0.5f, -1.0f, 0.0f, 0.0f),
		VertexPN(-0.5f, +0.5f, +0.5f, -1.0f, 0.0f, 0.0f),

		VertexPN(-0.5f, +0.5f, -0.5f, 0.0f, 1.0f, 0.0f),
		VertexPN(+0.5f, +0.5f, -0.5f, 0.0f, 1.0f, 0.0f),
		VertexPN(+0.5f, +0.5f, +0.5f, 0.0f, 1.0f, 0.0f),
		VertexPN(-0.5f, +0.5f, +0.5f, 0.0f, 1.0f, 0.0f),

		VertexPN(-0.5f, -0.5f, +0.5f, 0.0f, 1.0f, 0.0f),
		VertexPN(+0.5f, -0.5f, +0.5f, 0.0f, 1.0f, 0.0f),
		VertexPN(+0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f),
		VertexPN(-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f)
	};*/

	XMFLOAT3 p[] = {
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
	//f 2//1 3//1 1//1
	//f 4//2 7//2 3//2

	//f 8//3 5//3 7//3
	//f 5//4 2//4 1//4

	//f 7//5 1//5 3//5
	//f 4//6 6//6 8//6

	//f 2//1 4//1 3//1
	//f 4//2 8//2 7//2

	//f 8//3 6//3 5//3
	//f 5//4 6//4 2//4

	//f 7//5 5//5 1//5
	//f 4//6 2//6 6//6

	//Load Vertex Data
		HRESULT hr = this->vbCube.Initialize(this->device.Get(), v, ARRAYSIZE(v));
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create vertex buffer.");
		return false;
	}

	int indices[36];
	for (int i = 0; i < 36; i++)
		indices[i] = i;

	//int indices[] =
	//{
	//	0,3,2,0,2,1,
	//	4,7,6,4,6,5,
	//	8,11,10,8,10,9,
	//	12,15,14,12,14,13,
	//	16,19,18,16,18,17,
	//	20,23,22,20,22,21
	//};

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

	camera.SetPosition(2.0f, -1.0f, -2.0f);
	camera.SetProjectionValues(90.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 1000.0f);

	return true;
}
