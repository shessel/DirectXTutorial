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

	D3D11_INPUT_ELEMENT_DESC texCoord0InputElementDesc = { 0 };
	texCoord0InputElementDesc.SemanticName = "TEXCOORD";
	texCoord0InputElementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
	texCoord0InputElementDesc.AlignedByteOffset = 24;
	texCoord0InputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	D3D11_INPUT_ELEMENT_DESC inputElementDescriptors[] = {
		positionInputElementDesc,
		color0InputElementDesc,
		texCoord0InputElementDesc
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
	InitializeTextures();
	InitializeVertexBuffer();
	InitializeIndexBuffer();
}

void Model::InitializeTextures() {
	D3D11_TEXTURE2D_DESC textureDesc = { 0 };
	textureDesc.Width = 256;
	textureDesc.Height = 256;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	std::vector<byte> texData(textureDesc.Width * textureDesc.Height * 4);
	for (int y = 0; y < textureDesc.Height; y++) {
		for (int x = 0; x < textureDesc.Width; x++) {
			byte patternWidth = 8;
			byte value = ((y + x) / patternWidth) % patternWidth;
			value *= 256/patternWidth;
			texData[(y * textureDesc.Height + x) * 4] = value;
			texData[(y * textureDesc.Height + x) * 4 + 1] = value;
			texData[(y * textureDesc.Height + x) * 4 + 2] = value;
			texData[(y * textureDesc.Height + x) * 4 + 3] = value;
		}
	}

	D3D11_SUBRESOURCE_DATA textureSubresourceData = { 0 };
	textureSubresourceData.pSysMem = texData.data();
	textureSubresourceData.SysMemPitch = 1024;
	textureSubresourceData.SysMemSlicePitch = 0;

	device->CreateTexture2D(&textureDesc, &textureSubresourceData, texture.GetAddressOf());

	D3D11_SHADER_RESOURCE_VIEW_DESC textureViewDesc;
	ZeroMemory(&textureViewDesc, sizeof(textureViewDesc));
	textureViewDesc.Format = textureDesc.Format;
	textureViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	textureViewDesc.Texture2D.MipLevels = textureDesc.MipLevels;
	textureViewDesc.Texture2D.MostDetailedMip = 0;

	device->CreateShaderResourceView(texture.Get(), &textureViewDesc, textureView.GetAddressOf());

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxAnisotropy = 0;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	device->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf());
}

void Model::InitializeVertexBuffer() {
	Vertex vertices[] = {
		{ { -0.5f, 0.5f, -0.5f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } },
		{ { 0.5f, 0.5f, -0.5f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f } },
		{ { 0.5f, 0.5f,  0.5f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 1.0f } },
		{ { -0.5f, 0.5f,  0.5f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 1.0f } },

		{ { -0.5f, -0.5f,  0.5f },{ 0.0f, -1.0f, 0.0f },{ 0.0f, 0.0f } },
		{ { 0.5f, -0.5f,  0.5f },{ 0.0f, -1.0f, 0.0f },{ 1.0f, 0.0f } },
		{ { 0.5f, -0.5f, -0.5f },{ 0.0f, -1.0f, 0.0f },{ 1.0f, 1.0f } },
		{ { -0.5f, -0.5f, -0.5f },{ 0.0f, -1.0f, 0.0f },{ 0.0f, 1.0f } },

		{ { 0.5f,  0.5f,  0.5f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f } },
		{ { 0.5f,  0.5f, -0.5f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } },
		{ { 0.5f, -0.5f, -0.5f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 1.0f } },
		{ { 0.5f, -0.5f,  0.5f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 1.0f } },

		{ { -0.5f,  0.5f, -0.5f },{ -1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f } },
		{ { -0.5f,  0.5f,  0.5f },{ -1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } },
		{ { -0.5f, -0.5f,  0.5f },{ -1.0f, 0.0f, 0.0f },{ 1.0f, 1.0f } },
		{ { -0.5f, -0.5f, -0.5f },{ -1.0f, 0.0f, 0.0f },{ 0.0f, 1.0f } },

		{ { -0.5f,  0.5f, 0.5f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 0.0f } },
		{ { 0.5f,  0.5f, 0.5f },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 0.0f } },
		{ { 0.5f, -0.5f, 0.5f },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f } },
		{ { -0.5f, -0.5f, 0.5f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } },

		{ { 0.5f,  0.5f, -0.5f },{ 0.0f, 0.0f, -1.0f },{ 0.0f, 0.0f } },
		{ { -0.5f,  0.5f, -0.5f },{ 0.0f, 0.0f, -1.0f },{ 1.0f, 0.0f } },
		{ { -0.5f, -0.5f, -0.5f },{ 0.0f, 0.0f, -1.0f },{ 1.0f, 1.0f } },
		{ { 0.5f, -0.5f, -0.5f },{ 0.0f, 0.0f, -1.0f },{ 0.0f, 1.0f } },
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

		8, 9, 10,
		8, 10, 11,

		12, 13, 14,
		12, 14, 15,

		16, 17, 18,
		16, 18, 19,

		20, 21, 22,
		20, 22, 23
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
	deviceContext->PSSetShaderResources(0, 1, textureView.GetAddressOf());
	deviceContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());

	deviceContext->DrawIndexed(numIndices, 0, 0);
}