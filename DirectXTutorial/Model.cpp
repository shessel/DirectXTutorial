#include "pch.h"
#include "Model.h"
#include <vector>
#include <fstream>

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

void Model::Initialize() {
	InitializeShaders();
	InitializeData();
}

void Model::InitializeShaders() {
	std::vector<byte> vertexShaderData = LoadShaderFile("SimpleVertexShader.cso");
	std::vector<byte> pixelShaderData = LoadShaderFile("SimplePixelShader.cso");
	HRESULT vsRes = device->CreateVertexShader(vertexShaderData.data(), vertexShaderData.size(), nullptr, &vertexShader);
	HRESULT psRes = device->CreatePixelShader(pixelShaderData.data(), pixelShaderData.size(), nullptr, &pixelShader);

	D3D11_INPUT_ELEMENT_DESC positionInputElementDesc = { 0 };
	positionInputElementDesc.SemanticName = "POSITION";
	positionInputElementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	positionInputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	D3D11_INPUT_ELEMENT_DESC color0InputElementDesc = { 0 };
	color0InputElementDesc.SemanticName = "COLOR";
	color0InputElementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	color0InputElementDesc.AlignedByteOffset = 12;
	color0InputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	D3D11_INPUT_ELEMENT_DESC inputElementDescriptors[] = {
		positionInputElementDesc,
		color0InputElementDesc
	};

	device->CreateInputLayout(
		inputElementDescriptors,
		ARRAYSIZE(inputElementDescriptors),
		vertexShaderData.data(),
		vertexShaderData.size(),
		&inputLayout
	);
}

void Model::InitializeData() {
	InitializeVertexBuffer();
	InitializeIndexBuffer();
}

void Model::InitializeVertexBuffer() {
	Vertex vertices[] = {
		{ { -0.5f, 0.5f, -0.5f },{ 1.0f, 0.0f, 0.0f } },
		{ { 0.5f, 0.5f, -0.5f },{ 1.0f, 0.0f, 0.0f } },
		{ { 0.5f, 0.5f,  0.5f },{ 1.0f, 0.0f, 0.0f } },
		{ { -0.5f, 0.5f,  0.5f }, { 1.0f, 0.0f, 0.0f } },
		{ { -0.5f, -0.5f,  0.5f },{ 0.0f, 1.0f, 0.0f } },
		{ { 0.5f, -0.5f,  0.5f },{ 0.0f, 1.0f, 0.0f } },
		{ { 0.5f, -0.5f, -0.5f },{ 0.0f, 1.0f, 0.0f } },
		{ { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } }
	};

	D3D11_BUFFER_DESC bufferDesc = { 0 };
	bufferDesc.ByteWidth = sizeof(Vertex) * ARRAYSIZE(vertices);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subresourceData = { 0 };
	subresourceData.pSysMem = vertices;

	device->CreateBuffer(&bufferDesc, &subresourceData, &vertexBuffer);
}

void Model::InitializeIndexBuffer() {
	unsigned short indices[] = {
		0, 1, 2,
		0, 2, 3,

		4, 5, 6,
		4, 6, 7,

		3, 2, 5,
		3, 5, 4,

		2, 1, 6,
		2, 6, 5,

		1, 7, 6,
		1, 0, 7,

		0, 3, 4,
		0, 4, 7
	};

	numIndices = ARRAYSIZE(indices);
	D3D11_BUFFER_DESC indexBufferDesc = { 0 };
	indexBufferDesc.ByteWidth = sizeof(unsigned short) * numIndices;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA indexBufferSubresourceData = { 0 };
	indexBufferSubresourceData.pSysMem = indices;

	device->CreateBuffer(&indexBufferDesc, &indexBufferSubresourceData, &indexBuffer);
}

void Model::Render() {
	deviceContext->IASetInputLayout(inputLayout.Get());
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
	deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);

	deviceContext->DrawIndexed(numIndices, 0, 0);
}