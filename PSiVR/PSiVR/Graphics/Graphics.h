#pragma once
#include "AdapterReader.h"
#include "Shaders.h"
#include "Vertex.h"
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <WICTextureLoader.h>
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "Camera.h"
#include "..\\Timer.h"
#include "MyImGui.h"
#include "..\\Simulation.h"
#include "ImGUI\\imgui.h"
#include "ImGUI\\imgui_impl_win32.h"
#include "ImGUI\\imgui_impl_dx11.h"
#include "..\\GuiData.h"

using namespace std;
using Microsoft::WRL::ComPtr;

class Graphics
{
public:
	~Graphics();
	bool Initialize(HWND hwnd, int width, int height);
	void RenderFrame();
	Camera camera;
	Simulation* simulation;
	GuiData * guiData;

	void UpdateFrameMesh();
	void UpdateJellyMesh();
private:
	bool InitializeDirectX(HWND hwnd);
	bool InitializeShaders();
	bool InitializeScene();
	void GetFrame(Vector3 lb, Vector3 ub,  vector<VertexPN>& vertices, vector<int>& indices);
	void InitBox();
	void InitFrame();
	void InitJelly();
	void InitConstantBuffers();
	void InitUAV();

	void InitGui(HWND hwnd);
	void RendeGui();
	void RenderMainPanel();
	void RenderVisualisation();
	void RenderFrame(VertexBuffer<VertexPN>& vb, IndexBuffer& ib, Vector4 color, Matrix matrix);

	ComPtr<ID3D11Buffer> uav_buffer;
	ComPtr<ID3D11UnorderedAccessView> uav_view;

	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> deviceContext;
	ComPtr<IDXGISwapChain> swapchain;
	ComPtr<ID3D11RenderTargetView> renderTargetView;

	VertexShader vertexshader;
	PixelShader pixelshader;
	PixelShader pureColorPixelshader;

	ConstantBuffer<ColoredObjectBuffer> cbColoredObject;
	ConstantBuffer<LightBuffer> cbLight;


	VertexBuffer<VertexPN> vbBox, vbJelly, vbFrame;
	IndexBuffer ibBox, ibJelly, ibFrame, ibJellySides[6];

	ComPtr<ID3D11DepthStencilView> depthStencilView;
	ComPtr<ID3D11Texture2D> depthStencilBuffer;
	ComPtr<ID3D11DepthStencilState> depthStencilState;
	ComPtr<ID3D11RasterizerState> rasterizerState;

	int windowWidth = 0;
	int windowHeight = 0;
	Timer fpsTimer;
};