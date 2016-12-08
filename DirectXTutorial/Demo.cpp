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
}

void Demo::Update() {
}

void Demo::Render() {
}
