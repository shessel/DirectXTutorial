#pragma once

#include "DirectXMath.h"

using namespace Microsoft::WRL;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Core;
using namespace DirectX;

class Model {
	ComPtr<ID3D11Device1> device;
	ComPtr<ID3D11DeviceContext1> deviceContext;
	ComPtr<ID3D11VertexShader> vertexShader;
	ComPtr<ID3D11PixelShader> pixelShader;
	ComPtr<ID3D11InputLayout> inputLayout;
	ComPtr<ID3D11Buffer> indexBuffer;
	ComPtr<ID3D11Buffer> vertexBuffer;

	struct Vertex {
		XMFLOAT3 pos;
		XMFLOAT3 col;
	};
public:
	Model(ComPtr<ID3D11Device1> device, ComPtr<ID3D11DeviceContext1> deviceContext) :
		device(device), deviceContext(deviceContext) {};
	void Initialize();
	void InitializeShaders();
	void InitializeData();
	void InitializeVertexBuffer();
	void InitializeIndexBuffer();
	void Render();
};

