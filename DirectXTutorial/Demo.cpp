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
	InitializeViews();
	InitializeViewport();
}

void Demo::InitializeViews() {
	ComPtr<ID3D11Texture2D> backbuffer;
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backbuffer);

	D3D11_TEXTURE2D_DESC backBufferDesc = { 0 };
	backbuffer->GetDesc(&backBufferDesc);

	device->CreateRenderTargetView(backbuffer.Get(), nullptr, &renderTargetView);

	D3D11_TEXTURE2D_DESC depthStencilDesc = { 0 };
	depthStencilDesc.Width = backBufferDesc.Width;
	depthStencilDesc.Height = backBufferDesc.Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	ComPtr<ID3D11Texture2D> depthStencil;
	device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencil);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	depthStencilViewDesc.Format = depthStencilDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Flags = 0;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	device->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, &depthStencilView);
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
	deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

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