#include "pch.h"

#include <fstream>
#include <string>
#include <vector>

#include "Demo.h"

std::vector<byte> LoadShaderFile(std::string fileName) {
	std::vector<byte> fileData;
	std::ifstream fileStream(fileName, std::ios::binary | std::ios::ate);

	if (fileStream.is_open()) {
		size_t fileLength = fileStream.tellg();
		fileData.resize(fileLength);
		fileStream.seekg(0, std::ios::beg);
		fileStream.read(reinterpret_cast<char*>(fileData.data()), fileLength);
		fileStream.close();
	}

	return fileData;
}

void Demo::Initialize() {
	InitializeDeviceDependentResources();
	InitializeWindowSizeDependentResources();
}

void Demo::InitializeDeviceDependentResources() {
	InitializeDeviceAndDeviceContext();
	InitializeSwapChain();
	InitializeData();
	InitializePipeline();
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

void Demo::InitializeData() {
	InitializeVertexBuffer();
	InitializeIndexBuffer();
}

void Demo::InitializeVertexBuffer() {
	Vertex vertices[] = {
		{ 0.0f, 0.5f, 0.5f },
		{ 0.45f, -0.5f, 0.5f },
		{ -0.45f, -0.5f, 0.5f }
	};

	D3D11_BUFFER_DESC bufferDesc = { 0 };
	bufferDesc.ByteWidth = sizeof(Vertex) * ARRAYSIZE(vertices);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subresourceData = { 0 };
	subresourceData.pSysMem = vertices;

	device->CreateBuffer(&bufferDesc, &subresourceData, &vertexBuffer);
}

void Demo::InitializeIndexBuffer() {
	unsigned short indices[] = {
		0, 1, 2
	};

	D3D11_BUFFER_DESC indexBufferDesc = { 0 };
	indexBufferDesc.ByteWidth = sizeof(unsigned short) * ARRAYSIZE(indices);
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA indexBufferSubresourceData = { 0 };
	indexBufferSubresourceData.pSysMem = indices;

	device->CreateBuffer(&indexBufferDesc, &indexBufferSubresourceData, &indexBuffer);
}

void Demo::InitializePipeline() {
	std::vector<byte> vertexShaderData = LoadShaderFile("SimpleVertexShader.cso");
	std::vector<byte> pixelShaderData = LoadShaderFile("SimplePixelShader.cso");
	HRESULT vsRes = device->CreateVertexShader(vertexShaderData.data(), vertexShaderData.size(), nullptr, &vertexShader);
	HRESULT psRes = device->CreatePixelShader(pixelShaderData.data(), pixelShaderData.size(), nullptr, &pixelShader);
	deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
	deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);

	D3D11_INPUT_ELEMENT_DESC inputElementDesc = { 0 };
	inputElementDesc.SemanticName = "POSITION";
	inputElementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	device->CreateInputLayout(
		&inputElementDesc,
		1,
		vertexShaderData.data(),
		vertexShaderData.size(),
		&inputLayout
	);

	deviceContext->IASetInputLayout(inputLayout.Get());
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
	
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), nullptr);

	deviceContext->DrawIndexed(3, 0, 0);

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