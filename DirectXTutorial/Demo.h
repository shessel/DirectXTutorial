#pragma once

using namespace Microsoft::WRL;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Core;
using namespace DirectX;

class Demo {
	ComPtr<ID3D11Device1> device;
	ComPtr<ID3D11DeviceContext1> deviceContext;
	ComPtr<IDXGISwapChain1> swapChain;
	ComPtr<ID3D11RenderTargetView> renderTargetView;
	ComPtr<ID3D11Buffer> vertexBuffer;
	ComPtr<ID3D11VertexShader> vertexShader;
	ComPtr<ID3D11PixelShader> pixelShader;
	ComPtr<ID3D11InputLayout> inputLayout;

	void InitializeDeviceDependentResources();
	void InitializeDeviceAndDeviceContext();
	void InitializeData();
	void InitializePipeline();

	void InitializeWindowSizeDependentResources();
	void InitializeSwapChain();
	void InitializeBackbuffer();
	void InitializeViewport();

	void UnbindWindowSizeDependentResources();

	struct Vertex {
		float x;
		float y;
		float z;
	};

public:
	void Initialize();
	void Update();
	void Render();
	void Resize();
};

