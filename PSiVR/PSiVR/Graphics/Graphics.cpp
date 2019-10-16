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

void Graphics::UpdateChartData()
{
	int start = dataX.size();

	for (size_t i = start; i < simulation->x.size(); i++)
	{
		dataX.push_back(ImVec2(simulation->t[i], simulation->x[i]));
		dataXt.push_back(ImVec2(simulation->t[i], simulation->xt[i]));
		dataXtt.push_back(ImVec2(simulation->t[i], simulation->xtt[i]));
		dataW.push_back(ImVec2(simulation->t[i], simulation->w[i]));
		dataH.push_back(ImVec2(simulation->t[i], simulation->h[i]));
		dataS.push_back(ImVec2(simulation->x[i], simulation->xt[i]));
	}
}

void Graphics::ResetChartData()
{
	dataX.clear();
	dataXt.clear();
	dataXtt.clear();
	dataW.clear();
	dataH.clear();
	dataS.clear();
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
	RenderCharts();

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

	ImGui::SameLine();
	if (ImGui::Button("Reset")) {
		simulation->Reset();
		ResetChartData();
		UpdateChartData();
	}

	ImGui::SliderFloat("delta time", &simulation->delta_time, 0.01f, 0.2f);
	ImGui::SliderFloat("m", &simulation->m, 0.3, 2);
	ImGui::SliderFloat("c", &simulation->c, 0, 1);
	ImGui::SliderFloat("k", &simulation->k, 0, 1);
	ImGui::SliderFloat("x0", &simulation->x0, 0, 2);
	ImGui::SliderFloat("v0", &simulation->v0, 0, 2);

	ImGui::Separator();
	ImGui::Text("function w(t):");
	ImGui::RadioButton("Zero##w", &simulation->w_func, 0); ImGui::SameLine();
	ImGui::RadioButton("Const##w", &simulation->w_func, 1); ImGui::SameLine();
	ImGui::RadioButton("Jump##w", &simulation->w_func, 2);
	ImGui::RadioButton("Sgn Sin##w", &simulation->w_func, 3); ImGui::SameLine();
	ImGui::RadioButton("Sin##w", &simulation->w_func, 4);

	ImGui::Separator();
	ImGui::Text("function h(t):");
	ImGui::RadioButton("Zero##h", &simulation->h_func, 0); ImGui::SameLine();
	ImGui::RadioButton("Const##h", &simulation->h_func, 1); ImGui::SameLine();
	ImGui::RadioButton("Jump##h", &simulation->h_func, 2);
	ImGui::RadioButton("Sgn Sin##h", &simulation->h_func, 3); ImGui::SameLine();
	ImGui::RadioButton("Sin##h", &simulation->h_func, 4);

	ImGui::End();
}

void Graphics::RenderCharts() {
	ImGui::SetNextWindowSize(ImVec2(1570, 370), ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(320, 370), ImGuiCond_Once);
	if (ImGui::Begin("x(t)-red  x'(t)-green  x''(t)-blue"))
	{
		ChartData cdX(&dataX, IM_COL32(200, 0, 0, 255));
		ChartData cdXt(&dataXt, IM_COL32(0, 200, 0, 255));
		ChartData cdXtt(&dataXtt, IM_COL32(0, 0, 200, 255));

		MyImGui::DrawChart({ cdXtt, cdXt, cdX }, ImVec2(0, -2), ImVec2(100, 2));
		ImGui::End();
	}

	ImGui::SetNextWindowSize(ImVec2(1570, 230), ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(320, 750), ImGuiCond_Once);
	if (ImGui::Begin("w(t)-red  h(t)-green"))
	{
		ChartData cdW(&dataW, IM_COL32(200, 0, 0, 255));
		ChartData cdH(&dataH, IM_COL32(0, 200, 0, 255));

		MyImGui::DrawChart({ cdW, cdH }, ImVec2(0, -2), ImVec2(100, 2));
		ImGui::End();
	}

	ImGui::SetNextWindowSize(ImVec2(350, 350), ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(1540, 10), ImGuiCond_Once);
	if (ImGui::Begin("State"))
	{
		ChartData cd(&dataS, IM_COL32(200, 0, 0, 255));

		MyImGui::DrawChart({ cd }, ImVec2(-2, -2), ImVec2(2, 2));
		ImGui::End();
	}
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

	float val = 2 + simulation->x.back();

	//mass
	XMMATRIX world = XMMatrixTranslation(0.5, 0, 0) * XMMatrixScaling(0.4, 0.4, 1) * XMMatrixTranslation(val, 0, 0);

	cbMVP.data.mat = world * camera.GetViewMatrix() * camera.GetProjectionMatrix();
	cbMVP.data.mat = DirectX::XMMatrixTranspose(cbMVP.data.mat);

	if (!cbMVP.ApplyChanges()) return;
	this->deviceContext->VSSetConstantBuffers(0, 1, this->cbMVP.GetAddressOf());
	this->deviceContext->IASetVertexBuffers(0, 1, vbMass.GetAddressOf(), vbMass.StridePtr(), &offset);
	this->deviceContext->IASetIndexBuffer(ibMass.Get(), DXGI_FORMAT_R32_UINT, 0);
	this->deviceContext->DrawIndexed(ibMass.BufferSize(), 0, 0);

	//spring
	world = XMMatrixScaling(val, 0.2 / val, 1) * XMMatrixTranslation(val / 2, 0, 0);

	cbMVP.data.mat = world * camera.GetViewMatrix() * camera.GetProjectionMatrix();
	cbMVP.data.mat = DirectX::XMMatrixTranspose(cbMVP.data.mat);

	if (!cbMVP.ApplyChanges()) return;
	this->deviceContext->VSSetConstantBuffers(0, 1, this->cbMVP.GetAddressOf());
	this->deviceContext->IASetVertexBuffers(0, 1, vbSpring.GetAddressOf(), vbMass.StridePtr(), &offset);
	this->deviceContext->IASetIndexBuffer(ibSpring.Get(), DXGI_FORMAT_R32_UINT, 0);
	this->deviceContext->DrawIndexed(ibSpring.BufferSize(), 0, 0);
}

