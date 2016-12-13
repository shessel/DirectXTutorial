#include "pch.h"
#include "Demo.h"

void Demo::Initialize() {
	InitializeDeviceDependentResources();
	InitializeWindowSizeDependentResources();
	model = std::make_shared<Model>(device, deviceContext);
	model->Initialize();
}

void Demo::InitializeDeviceDependentResources() {
	InitializeDeviceAndDeviceContext();
	InitializeSwapChain();
}

void Demo::InitializeDeviceAndDeviceContext() {
	ComPtr<ID3D11Device> baseVersionDevice;
	ComPtr<ID3D11DeviceContext> baseVersionDeviceContext;
	
	UINT creationFlags = 0;

#if defined(_DEBUG)
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		creationFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&baseVersionDevice,
		nullptr,
		&baseVersionDeviceContext
	);
	baseVersionDevice.As(&device);
	baseVersionDeviceContext.As(&deviceContext);
}

void Demo::InitializeSwapChain() {
	ComPtr<IDXGIDevice1> dxgiDevice;
	device.As(&dxgiDevice);

	ComPtr<IDXGIAdapter> dxgiAdapter;
	dxgiDevice->GetAdapter(&dxgiAdapter);

	ComPtr<IDXGIFactory2> dxgiFactory;
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), &dxgiFactory);

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swapChainDesc.SampleDesc.Count = 1;

	CoreWindow^ Window = CoreWindow::GetForCurrentThread();
	dxgiFactory->CreateSwapChainForCoreWindow(
		device.Get(),
		reinterpret_cast<IUnknown*>(Window),
		&swapChainDesc,
		nullptr,
		&swapChain
	);
}

void Demo::InitializeWindowSizeDependentResources() {
	InitializeBackbuffer();
	InitializeViewport();
}

void Demo::InitializeBackbuffer() {
	ComPtr<ID3D11Texture2D> backbuffer;
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backbuffer);
	device->CreateRenderTargetView(backbuffer.Get(), nullptr, &renderTargetView);
}

void Demo::InitializeViewport() {
	DisplayInformation^ displayInformation = DisplayInformation::GetForCurrentView();
	float dpi = displayInformation->LogicalDpi;
	CoreWindow^ Window = CoreWindow::GetForCurrentThread();

	D3D11_VIEWPORT viewport = { 0 };
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = Window->Bounds.Width * dpi / 96.0f;
	viewport.Height = Window->Bounds.Height * dpi / 96.0f;
	deviceContext->RSSetViewports(1, &viewport);
}

void Demo::Update() {
}

void Demo::Render() {	
	float color[4] = { 0.1f, 0.2f, 0.3f, 1.0f };
	deviceContext->ClearRenderTargetView(renderTargetView.Get(), color);
	deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), nullptr);

	model->Render();

	swapChain->Present(1, 0);
}

void Demo::Resize() {
	UnbindWindowSizeDependentResources();
	swapChain->ResizeBuffers(
		2,
		0,
		0,
		DXGI_FORMAT_B8G8R8A8_UNORM,
		0
	);
	InitializeWindowSizeDependentResources();
}

void Demo::UnbindWindowSizeDependentResources() {
	deviceContext->OMSetRenderTargets(0, nullptr, nullptr);
	renderTargetView = nullptr;
}