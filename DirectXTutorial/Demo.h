#pragma once

#include <memory>
#include "DirectXMath.h"
#include "Model.h"

using namespace Microsoft::WRL;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Core;
using namespace DirectX;

class Demo {
	ComPtr<ID3D11Device1> device;
	ComPtr<ID3D11DeviceContext1> deviceContext;
	ComPtr<IDXGISwapChain1> swapChain;
	ComPtr<ID3D11RenderTargetView> renderTargetView;
	std::shared_ptr<Model> model;

	void InitializeDeviceDependentResources();
	void InitializeDeviceAndDeviceContext();

	void InitializeWindowSizeDependentResources();
	void InitializeSwapChain();
	void InitializeBackbuffer();
	void InitializeViewport();

	void UnbindWindowSizeDependentResources();

	struct Vertex {
		XMFLOAT3 pos;
		XMFLOAT3 col;
	};

public:
	void Initialize();
	void Update();
	void Render();
	void Resize();
};

