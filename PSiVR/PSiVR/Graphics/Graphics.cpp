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

	//this->deviceContext->IASetInputLayout(this->vertexshader.GetInputLayout());
	//this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//this->deviceContext->RSSetState(this->rasterizerState.Get());
	//this->deviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 0);
	//this->deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());
	//this->deviceContext->VSSetShader(vertexshader.GetShader(), NULL, 0);
	//this->deviceContext->PSSetShader(pixelshader.GetShader(), NULL, 0);

	//UINT offset = 0;

	////Update Constant Buffer
	//static float translationOffset[3] = { 0, 0, 0 };
	//XMMATRIX world = XMMatrixTranslation(translationOffset[0], translationOffset[1], translationOffset[2]);
	//constantBuffer.data.mat = world * camera.GetViewMatrix() * camera.GetProjectionMatrix();
	//constantBuffer.data.mat = DirectX::XMMatrixTranspose(constantBuffer.data.mat);

	//if (!constantBuffer.ApplyChanges())
	//	return;
	//this->deviceContext->VSSetConstantBuffers(0, 1, this->constantBuffer.GetAddressOf());

	////Square
	//this->deviceContext->PSSetShaderResources(0, 1, this->myTexture.GetAddressOf());
	//this->deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.StridePtr(), &offset);
	//this->deviceContext->IASetIndexBuffer(indicesBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	//this->deviceContext->DrawIndexed(indicesBuffer.BufferSize(), 0, 0);

	//Draw Text
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

	if (ImGui::Button("Reset"))
		simulation->Reset();

	ImGui::SliderFloat("delta time", &simulation->delta_time, 0.01f, 0.2f);
	ImGui::SliderFloat("m", &simulation->m, 10.0f, 100.0f);
	ImGui::SliderFloat("c", &simulation->c, 50.0f, 500.0f);
	ImGui::SliderFloat("k", &simulation->k, 0.1, 10);


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

void  Graphics::RenderCharts() {
	ImGui::SetNextWindowSize(ImVec2(1570, 250), ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(320, 460), ImGuiCond_Once);
	if (ImGui::Begin("x(t)-red  x'(t)-green  x''(t)-blue"))
	{
		std::vector<ImVec2> dataX;
		for (size_t i = 0; i < simulation->x.size(); i++)
			dataX.push_back(ImVec2(simulation->t[i], simulation->x[i]));
		ChartData cdX(dataX, IM_COL32(200, 0, 0, 255));

		std::vector<ImVec2> dataXt;
		for (size_t i = 0; i < simulation->xt.size(); i++)
			dataXt.push_back(ImVec2(simulation->t[i], simulation->xt[i]));
		ChartData cdXt(dataXt, IM_COL32(0, 200, 0, 255));

		std::vector<ImVec2> dataXtt;
		for (size_t i = 0; i < simulation->xtt.size(); i++)
			dataXtt.push_back(ImVec2(simulation->t[i], simulation->xtt[i]));
		ChartData cdXtt(dataXtt, IM_COL32(0, 0, 200, 255));

		MyImGui::DrawChart({ &cdX,&cdXt,&cdXtt, }, ImVec2(0, -20), ImVec2(30, 20));
		ImGui::End();
	}

	ImGui::SetNextWindowSize(ImVec2(1570, 250), ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(320, 730), ImGuiCond_Once);
	if (ImGui::Begin("w(t)-red  h(t)-green"))
	{
		std::vector<ImVec2> dataW;
		for (size_t i = 0; i < simulation->w.size(); i++)
			dataW.push_back(ImVec2(simulation->t[i], simulation->w[i]));
		ChartData cdW(dataW, IM_COL32(200, 0, 0, 255));

		std::vector<ImVec2> dataH;
		for (size_t i = 0; i < simulation->h.size(); i++)
			dataH.push_back(ImVec2(simulation->t[i], simulation->h[i]));
		ChartData cdH(dataH, IM_COL32(0, 200, 0, 255));


		MyImGui::DrawChart({ &cdW,&cdH }, ImVec2(0, -2), ImVec2(20, 2));
		ImGui::End();
	}

	ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(1490, 30), ImGuiCond_Once);
	if (ImGui::Begin("State"))
	{
		std::vector<ImVec2> data;
		for (size_t i = 0; i < simulation->x.size(); i++)
			data.push_back(ImVec2(simulation->x[i], simulation->xt[i]));
		ChartData cd(data, IM_COL32(200, 0, 0, 255));


		MyImGui::DrawChart({ &cd }, ImVec2(-2, -5), ImVec2(2, 5));
		ImGui::End();
	}
}

