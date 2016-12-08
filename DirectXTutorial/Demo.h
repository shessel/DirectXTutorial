#pragma once

using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace DirectX;

class Demo {
	ComPtr<ID3D11Device1> device;
	ComPtr<ID3D11DeviceContext1> deviceContext;

public:
	void Initialize();
	void Update();
	void Render();
};

