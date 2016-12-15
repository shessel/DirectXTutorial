#pragma once

#include <memory>
#include "DirectXMath.h"
#include "Model.h"

using namespace Microsoft::WRL;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Core;
using namespace DirectX;

struct ConstantBufferData {
	XMMATRIX model;
	XMMATRIX view;
	XMMATRIX projection;
};

class Demo {
	ComPtr<ID3D11Device1> device;
	ComPtr<ID3D11DeviceContext1> deviceContext;
	ComPtr<IDXGISwapChain1> swapChain;
	ComPtr<ID3D11RenderTargetView> renderTargetView;
	ComPtr<ID3D11DepthStencilView> depthStencilView;
	ComPtr<ID3D11Buffer> constantBuffer;
	ConstantBufferData constantBufferData;
	std::shared_ptr<Model> model;

	void InitializeDeviceDependentResources();
	void InitializeDeviceAndDeviceContext();

	void InitializeWindowSizeDependentResources();
	void InitializeSwapChain();
	void InitializeConstantBuffer();
	void InitializeViews();
	void InitializeViewport();

	void UnbindWindowSizeDependentResources();
public:
	void Initialize();
	void Update();
	void Render();
	void Resize();
};

