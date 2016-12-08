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
	InitializeSwapChain();
	SetViewport();
	InitializeData();
	InitializePipeline();
}

void Demo::InitializeSwapChain() {
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
	this->device.As(&dxgiDevice);

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

	ComPtr<ID3D11Texture2D> backbuffer;
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backbuffer);
	this->device->CreateRenderTargetView(backbuffer.Get(), nullptr, &renderTargetView);
}

void Demo::SetViewport() {
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

void Demo::InitializeData() {
	Vertex vertices[] = {
		{0.0f, 0.5f, 0.5f},
		{0.45f, -0.5f, 0.5f},
		{-0.45f, -0.5f, 0.5f}
	};

	D3D11_BUFFER_DESC bufferDesc = { 0 };
	bufferDesc.ByteWidth = sizeof(Vertex) * ARRAYSIZE(vertices);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subresourceData = { 0 };
	subresourceData.pSysMem = vertices;

	device->CreateBuffer(&bufferDesc, &subresourceData, &vertexBuffer);
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

void Demo::Update() {
}

void Demo::Render() {
	deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), nullptr);
	
	float color[4] = { 0.1f, 0.2f, 0.3f, 1.0f };
	deviceContext->ClearRenderTargetView(renderTargetView.Get(), color);
	
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	deviceContext->Draw(3, 0);

	swapChain->Present(1, 0);
}

void Demo::Resize() {
	swapChain->ResizeBuffers(
		2,
		0,
		0,
		DXGI_FORMAT_B8G8R8A8_UNORM,
		0
	);
}
