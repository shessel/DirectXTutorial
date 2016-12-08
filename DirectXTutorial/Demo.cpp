#include "pch.h"
#include "Demo.h"

void Demo::Initialize() {
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;

	D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&device,
		nullptr,
		&context
	);
	device.As(&this->device);
	context.As(&this->deviceContext);

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
		this->device.Get(),
		reinterpret_cast<IUnknown*>(Window),
		&swapChainDesc,
		nullptr,
		&this->swapChain
	);
}

void Demo::Update() {
}

void Demo::Render() {
	swapChain->Present(1, 0);
}
