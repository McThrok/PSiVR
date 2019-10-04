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
	float bgcolor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), bgcolor);
	this->deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	this->deviceContext->IASetInputLayout(this->vertexshader.GetInputLayout());
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->deviceContext->RSSetState(this->rasterizerState.Get());
	this->deviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 0);
	this->deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());
	this->deviceContext->VSSetShader(vertexshader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(pixelshader.GetShader(), NULL, 0);

	UINT offset = 0;

	//Update Constant Buffer
	static float translationOffset[3] = { 0, 0, 0 };
	XMMATRIX world = XMMatrixTranslation(translationOffset[0], translationOffset[1], translationOffset[2]);
	constantBuffer.data.mat = world * camera.GetViewMatrix() * camera.GetProjectionMatrix();
	constantBuffer.data.mat = DirectX::XMMatrixTranspose(constantBuffer.data.mat);

	if (!constantBuffer.ApplyChanges())
		return;
	this->deviceContext->VSSetConstantBuffers(0, 1, this->constantBuffer.GetAddressOf());

	//Square
	this->deviceContext->PSSetShaderResources(0, 1, this->myTexture.GetAddressOf());
	this->deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.StridePtr(), &offset);
	this->deviceContext->IASetIndexBuffer(indicesBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	this->deviceContext->DrawIndexed(indicesBuffer.BufferSize(), 0, 0);

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
	ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_Once);
	if (!ImGui::Begin("Main Panel"))
	{
		ImGui::End();
		return;
	}

	static int counter = 0;
	ImGui::Text("This is example text.");
	if (ImGui::Button("CLICK ME!"))
		counter += 1;
	ImGui::SameLine();
	std::string clickCount = "Click Count: " + std::to_string(counter);
	ImGui::Text(clickCount.c_str());
	static float translationOffset[3] = { 0, 0, 0 };
	ImGui::DragFloat3("Translation X/Y/Z", translationOffset, 0.1f, -5.0f, 5.0f);

	static float f = 0;
	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);


	// Plot some values
	const float my_values[] = { 1,2,3,4,5 };
	ImGui::PlotLines("Frame Times", my_values, IM_ARRAYSIZE(my_values));

	ImGui::End();
}

void  Graphics::RenderCharts() {
	ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(600, 50), ImGuiCond_Once);
	if (ImGui::Begin("My Title"))
	{
		std::vector<ImVec2> data;
		for (size_t i = 0; i < simulation->x.size(); i++)
			data.push_back(ImVec2(simulation->t[i], simulation->x[i]));
	/*	for (size_t i = 0; i < 10; i++)
			data.push_back(ImVec2(i, i));*/

		ChartData cd(data, IM_COL32(200, 0, 0, 255));
		MyImGui::DrawChart({ &cd }, ImVec2(0, -2), ImVec2(10, 2));
		ImGui::End();
	}
}

